#!/usr/bin/env python3
"""Stage project-owned Aiko assets and shader sources in examples.

Runs from the repository root. Dry run by default. Copies (does not delete)
shared root files; deleting originals is a separate, post-validation cleanup.

Reads C++ literal references and common sidecar dependencies. Does not rewrite
source code or move engine/renderer built-ins.
"""
from __future__ import annotations
import argparse
import json
import re
import shutil
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

SOURCE_EXTS = {'.cpp', '.cc', '.cxx', '.h', '.hpp', '.inl'}
ASSET_EXTS = {'.png', '.jpg', '.jpeg', '.bmp', '.tga', '.hdr', '.exr', '.ktx', '.ktx2', '.dds', '.obj', '.mtl', '.fbx', '.gltf', '.glb', '.dae', '.wav', '.ogg', '.mp3', '.ttf', '.otf', '.json', '.nc', '.bin', '.rom', '.nes', '.glsl', '.vert', '.frag', '.comp', '.vs', '.fs', '.shadertoy'}
SHADER_EXTS = {'.vert', '.frag', '.comp', '.glsl', '.shadertoy'}
TEXTURE_WORDS = ('map_Ka', 'map_Kd', 'map_Ks', 'map_Ke', 'map_Bump', 'map_bump', 'bump', 'norm', 'disp', 'decal')
QUOTED = re.compile(r'"([^"\n]+)"')
SHADER_INCLUDE = re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]', re.M)
SHADER_INCLUDE_ALT = re.compile(r'^\s*#\s*pragma\s+include\s*[<"]([^>"]+)[>"]', re.M)
MTLLIB = re.compile(r'^\s*mtllib\s+(.+?)\s*$', re.M)
VALID_NAMESPACES = ('project://', 'engine://', 'renderer://')

def git_tracked(root: Path, path: Path) -> bool:
    return subprocess.run(['git', 'ls-files', '--error-unmatch', '--', str(path.relative_to(root))], cwd=root, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode == 0

def read(path: Path) -> str:
    return path.read_text(encoding='utf-8', errors='replace')

def root_resource(reference: str, assets: Path, shaders: Path):
    if reference.startswith(VALID_NAMESPACES):
        return None
    if '://' in reference or reference.startswith(('/', '~', '../')) or '\\' in reference:
        return None
    r = reference.strip().replace('\\\\', '/')
    p = Path(r)
    if not r or '..' in p.parts:
        return None
    candidates = [assets / p, assets / 'models' / p, assets / 'textures' / p]
    if r.endswith(('.vs', '.fs')):
        # Logical shader extension convention in Aiko
        ext = '.vert' if r.endswith('.vs') else '.frag'
        candidates.append(shaders / (r[:-3] + ext))
    if p.suffix in SHADER_EXTS or p.suffix == '.spv':
        candidates.extend([shaders / p, shaders / p.with_suffix('')])
    for candidate in candidates:
        if candidate.is_file() and candidate.resolve().is_relative_to(assets.resolve()):
            return candidate
    return None

def gather_sidecars(source: Path, assets: Path, queue: set[Path], warnings: list[str]):
    if source.suffix.lower() == '.obj':
        for name in MTLLIB.findall(read(source)):
            m = (source.parent / name.strip()).resolve()
            if m.is_file() and m.is_relative_to(assets.resolve()):
                queue.add(m)
            else:
                warnings.append(f'OBJ material missing: {source.relative_to(assets)} -> {name}')
    elif source.suffix.lower() == '.mtl':
        for line in read(source).splitlines():
            parts = line.strip().split()
            if len(parts) >= 2 and parts[0] in TEXTURE_WORDS:
                # Common MTL form: texture is last token; warn if unsupported options
                item = parts[-1]
                p = (source.parent / item).resolve()
                if p.is_file() and p.is_relative_to(assets.resolve()):
                    queue.add(p)
                else:
                    warnings.append(f'MTL texture needs inspection: {source.relative_to(assets)} -> {line.strip()}')
    elif source.suffix.lower() == '.gltf':
        try:
            d = json.loads(read(source))
            for entry in d.get('buffers', []) + d.get('images', []):
                uri = entry.get('uri', '')
                if uri and not uri.startswith(('data:', 'http:', 'https:')):
                    p = (source.parent / uri).resolve()
                    if p.is_file() and p.is_relative_to(assets.resolve()):
                        queue.add(p)
                    else:
                        warnings.append(f'glTF dependency missing: {source.relative_to(assets)} -> {uri}')
        except (ValueError, TypeError) as exc:
            warnings.append(f'Cannot parse glTF {source.relative_to(assets)}: {exc}')
    elif source.suffix.lower() in SHADER_EXTS:
        for inc in SHADER_INCLUDE.findall(read(source)) + SHADER_INCLUDE_ALT.findall(read(source)):
            candidates = [source.parent / inc, assets / 'shaders' / 'vulkan' / inc]
            found = next((p.resolve() for p in candidates if p.is_file() and p.resolve().is_relative_to(assets.resolve())), None)
            if found is None:
                warnings.append(f'Shader include not found locally: {source.relative_to(assets)} -> {inc}')
            else:
                queue.add(found)

def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--apply', action='store_true', help='Copy files to example assets (default previews only)')
    ap.add_argument('--root', type=Path, default=Path.cwd(), help='Repository root')
    args = ap.parse_args()
    root = args.root.resolve()
    if not (root / '.git').exists() or not (root / 'examples').is_dir() or not (root / 'assets').is_dir():
        ap.error('--root must be the Aiko repository root')
    assets = root / 'assets'
    shaders = assets / 'shaders' / 'vulkan'
    uses: dict[Path, set[str]] = defaultdict(set)
    warnings: list[str] = []
    examples = sorted(p for p in (root / 'examples').iterdir() if p.is_dir() and (p / 'CMakeLists.txt').exists())
    for example in examples:
        for source in (example / 'src').rglob('*') if (example / 'src').exists() else []:
            if not source.is_file() or source.suffix not in SOURCE_EXTS:
                continue
            for literal in QUOTED.findall(read(source)):
                resolved = root_resource(literal, assets, shaders)
                if resolved is not None:
                    uses[resolved.resolve()].add(example.name)
    # Shader source dependencies and asset model sidecars propagate to the same projects
    todo = list(uses.items())
    for resource, owners in todo:
        additions: set[Path] = set()
        gather_sidecars(resource, assets, additions, warnings)
        for extra in additions:
            if extra not in uses:
                todo.append((extra, set(owners)))
            uses[extra].update(owners)

    
    operations = []

    for source, owners in sorted(uses.items(), key=lambda x: str(x[0])):
        rel = source.relative_to(assets)

        # Shader migration is handled separately.
        if rel.parts[:2] == ("shaders", "vulkan"):
            continue

        # Fonts are shared Aiko resources, not example assets.
        if rel.parts[0] == "fonts":
            continue

        for owner in sorted(owners):
            dest = root / "examples" / owner / "assets" / rel

            if dest.exists():
                if source.read_bytes() != dest.read_bytes():
                    warnings.append(
                        f"CONFLICT: {dest.relative_to(root)} "
                        "exists with different contents"
                    )
                continue

            operations.append((source, dest))

    for source, dest in operations:
        print(f'{"COPY" if args.apply else "PLAN"} {source.relative_to(root)} -> {dest.relative_to(root)}')
    print('\nWarnings / manual review:')
    if warnings:
        for w in sorted(set(warnings)):
            print('  - ' + w)
    else:
        print('  (none detected by this scanner)')
    if args.apply:
        for source, dest in operations:
            dest.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, dest)
        print('\nStaged copies only. Original root assets deliberately retained.')
    else:
        print('\nDry run. Re-run with --apply to stage copies.')
    print('NOTE: source scanning cannot prove an asset is unused. Review dynamic references and shader includes manually before deleting root assets.')
    print('NOTE: shader migration will be handled separately.')

if __name__ == '__main__':
    main()
