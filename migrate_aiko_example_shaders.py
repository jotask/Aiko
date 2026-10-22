#!/usr/bin/env python3
"""Stage example-owned Vulkan shaders from root assets into examples.

Dry run by default. --apply COPIES, never deletes or edits code/CMake.
Shared Aiko shader headers, renderer built-ins and generic shaders stay at root.
The script reports unresolved shader references and shared include dependencies.
"""
from __future__ import annotations

import argparse
import re
import shutil
from collections import defaultdict
from pathlib import Path

SOURCE_EXTS = {'.cpp', '.cc', '.cxx', '.h', '.hpp', '.inl'}
STAGE_EXTS = {'.vert', '.frag', '.comp', '.geom', '.tesc', '.tese'}
LOGICAL_EXTS = {'.vs': '.vert', '.fs': '.frag'}
QUOTED = re.compile(r'"([^"\n]+)"')
SHADER_CALL = re.compile(r'\b(?:loadShader|registerShader|loadComputeShader|registerComputeShader)\s*\(\s*"([^"]+)"(?:\s*,\s*"([^"]+)")?', re.S)
INCLUDE = re.compile(r'^\s*#\s*(?:include|pragma\s+include)\s*[<"]([^>"]+)[>"]', re.M)

# These belong to the renderer/engine or are shared library shader programs.
# They must not be copied into examples simply because one example uses them.
SHARED_PROGRAMS = {
    'model', 'model_instanced', 'pbr', 'ui', 'shader',
    'passthrough', 'gpu_vertex', 'gpu_billboard',
    'gpu_billboard_default', 'gpu_billboard_default_sorted',
    'gpu_billboard_sorted', 'particles_gpuinst', 'particles_gpuinst_sorted',
    'post_process',
}
SHARED_FILES = {
    'aiko_color.glsl', 'aiko_compute.glsl', 'aiko_descriptor_abi.glsl',
    'aiko_graphics.glsl', 'aiko_instance_order.glsl', 'aiko_model_lighting.glsl',
    'gpu_billboard_common.glsl', 'particles_gpuinst_common.glsl',
    'ui_material.glsl',
}

# Deliberately scoped explicit ownership; helps with shaders referenced by
# indirect code paths, runtime selection, or names that cannot be inferred.
# Add more groups here after reviewing their consumers.
EXAMPLE_SHADER_GROUPS: dict[str, tuple[str, ...]] = {
    'render_lab': (
        'uniform_validation.vert', 'uniform_validation.frag',
        'custom_shader_validation.vert', 'custom_shader_validation.frag',
        'multi_texture_validation.vert', 'multi_texture_validation.frag',
        'gpu_vertex_validation.vert', 'gpu_vertex_validation.frag',
    ),
    'visual_lab': (
        'color_space_validation.vert', 'color_space_validation.frag',
        'material_color_validation.frag',
        'multi_texture_validation.vert',
    ),
    'planet_earth': ('atmosphere.frag', 'atmosphere_scattering.frag'),
    'nbody': ('nbody_gpuinst.vert', 'nbody_init.comp', 'nbody_update.comp'),
    'smoothed_particle_hydrodynamics': ('sph/',),
    'shader_toy_vulkan': ('shadertoy/',),
}


def is_shared(rel: Path) -> bool:
    if rel.name in SHARED_FILES or rel.name.startswith('aiko_'):
        return True
    if rel.parts and rel.parts[0] == 'post_process':
        return True
    if rel.stem in SHARED_PROGRAMS:
        return True
    return False


def reference_to_files(reference: str, shaders: Path) -> list[Path]:
    if reference.startswith(('project://', 'engine://', 'renderer://')):
        return []
    if '://' in reference or '\\' in reference or reference.startswith(('/', '../')):
        return []
    name = Path(reference)
    if '..' in name.parts or name.is_absolute():
        return []
    if name.suffix in LOGICAL_EXTS:
        candidates = [name.with_suffix(LOGICAL_EXTS[name.suffix])]
    elif name.suffix in STAGE_EXTS:
        candidates = [name]
    elif not name.suffix:
        candidates = [Path(str(name) + ext) for ext in ('.vert', '.frag', '.comp')]
    else:
        return []
    return [shaders / p for p in candidates if (shaders / p).is_file()]


def shader_files_in_group(shaders: Path, spec: str) -> list[Path]:
    if spec.endswith('/'):
        directory = shaders / spec
        return sorted(p for p in directory.rglob('*') if p.is_file()) if directory.is_dir() else []
    path = shaders / spec
    return [path] if path.is_file() else []


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--apply', action='store_true', help='Copy shaders (default is preview only)')
    ap.add_argument('--root', type=Path, default=Path.cwd(), help='Aiko repository root')
    args = ap.parse_args()
    root = args.root.resolve()
    shaders = root / 'assets/shaders/vulkan'
    examples_root = root / 'examples'
    if not (root / '.git').exists() or not shaders.is_dir() or not examples_root.is_dir():
        ap.error('--root must point to the Aiko repository root')

    owners: dict[Path, set[str]] = defaultdict(set)
    warnings: set[str] = set()
    examples = sorted(p for p in examples_root.iterdir() if (p / 'CMakeLists.txt').is_file())
    example_names = {p.name for p in examples}

    for example in examples:
        src = example / 'src'
        if not src.exists():
            continue
        for f in src.rglob('*'):
            if not f.is_file() or f.suffix not in SOURCE_EXTS:
                continue
            content = f.read_text(encoding='utf-8', errors='replace')
            # Shader API calls capture name-only, explicit stage pair, and compute references.
            for call in SHADER_CALL.finditer(content):
                references = [v for v in call.groups() if v]
                for ref in references:
                    for p in reference_to_files(ref, shaders):
                        rel = p.relative_to(shaders)
                        if is_shared(rel):
                            continue
                        owners[p].add(example.name)
            # Also collect stage-like literals found outside directly recognized calls.
            for ref in QUOTED.findall(content):
                suffix = Path(ref).suffix
                if suffix not in STAGE_EXTS and suffix not in LOGICAL_EXTS:
                    continue
                for p in reference_to_files(ref, shaders):
                    rel = p.relative_to(shaders)
                    if not is_shared(rel):
                        owners[p].add(example.name)

    for owner, entries in EXAMPLE_SHADER_GROUPS.items():
        if owner not in example_names:
            warnings.add(f'Configured example missing; skipped: {owner}')
            continue
        for entry in entries:
            matches = shader_files_in_group(shaders, entry)
            if not matches:
                warnings.add(f'Configured shader missing: {owner} -> {entry}')
            for p in matches:
                rel = p.relative_to(shaders)
                if is_shared(rel):
                    warnings.add(f'Shared shader deliberately skipped: {rel}')
                    continue
                owners[p].add(owner)

    operations: list[tuple[Path, Path]] = []
    for source, project_names in sorted(owners.items(), key=lambda x: str(x[0])):
        rel = source.relative_to(shaders)
        for owner in sorted(project_names):
            target = examples_root / owner / 'assets/shaders' / rel
            if target.exists():
                if source.read_bytes() != target.read_bytes():
                    warnings.add(f'CONFLICT (not overwritten): {target.relative_to(root)}')
                continue
            operations.append((source, target))
        if source.suffix in STAGE_EXTS or source.suffix == '.glsl':
            content = source.read_text(encoding='utf-8', errors='replace')
            for name in INCLUDE.findall(content):
                candidate = source.parent / name
                if candidate.is_file():
                    candidate = candidate.resolve()
                else:
                    candidate = (shaders / name).resolve()
                if not candidate.is_file():
                    warnings.add(f'MISSING include: {rel} -> {name}')
                elif candidate.is_relative_to(shaders.resolve()):
                    include_rel = candidate.relative_to(shaders.resolve())
                    if is_shared(include_rel):
                        warnings.add(f'SHARED INCLUDE: {rel} -> {include_rel} (configure compiler include path; do not duplicate)')
                    elif candidate not in owners:
                        warnings.add(f'LOCAL INCLUDE NOT STAGED: {rel} -> {include_rel}')

    print(f'Examples scanned: {len(examples)} | shader source files selected: {len(owners)} | copies needed: {len(operations)}')
    for src, dst in operations:
        print(f'{"COPY" if args.apply else "PLAN"} {src.relative_to(root)} -> {dst.relative_to(root)}')
    print('\nWarnings / manual review:')
    if warnings:
        for warning in sorted(warnings):
            print('  - ' + warning)
    else:
        print('  (none)')

    if args.apply:
        for source, target in operations:
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, target)
        print('\nCopied project shader sources; originals retained. No code/CMake edits performed.')
    else:
        print('\nDry run. Re-run with --apply to copy shader sources.')
    print('IMPORTANT: project shader compilation may fail until shared GLSL include paths and shader ownership are fixed.')
    print('Do not remove root shaders until all examples build and run with local shader sources.')


if __name__ == '__main__':
    main()
