from pathlib import Path
import sys
import subprocess
import platform
import os
from enum import Enum
from concurrent.futures import ThreadPoolExecutor, as_completed

SYSTEM = platform.system().lower()  # 'windows', 'linux', or 'darwin'

class Profile:
    def __init__(self, platform: str, profile: str, folder: str):
        self.platform = platform
        self.profile = profile
        self.folder = folder

# (Your profiles list is currently unused in compileshader, keeping it as-is)
profiles = [
    Profile("windows", "vs_5_0", "dx11"),
    Profile("windows", "ps_5_0", "dx11"),
    Profile("windows", "vs_5_1", "dx12"),
    Profile("windows", "ps_5_1", "dx12"),
    Profile("windows", "150", "glsl"),
    Profile("linux",   "150", "glsl"),
    Profile("osx",     "150", "glsl"),
    Profile("windows", "300_es", "essl"),
    Profile("linux",   "300_es", "essl"),
    Profile("osx",     "300_es", "essl"),
    Profile("windows", "100_es", "essl"),
    Profile("linux",   "100_es", "essl"),
    Profile("osx",     "100_es", "essl"),
    Profile("windows", "spirv", "spirv"),
    Profile("linux",   "spirv", "spirv"),
    Profile("osx",     "spirv", "spirv"),
    Profile("osx", "metal", "metal"),
    Profile("ios", "metal", "metal"),
]

class Color(Enum):
    INFO    = "37"
    SECTION = "1;35"
    SUCCESS = "32"
    WARNING = "33"
    ERROR   = "31"

def colorPrint(severity: Color, msg):
    print(f"\033[{severity.value}m{msg}\033[0m")

def getprojectrootdir():
    return Path(__file__).parents[1].resolve()

def getLibsPath():
    if SYSTEM == "windows":
        return getprojectrootdir() / "build/debug/aiko/libs/"
    elif SYSTEM == "linux":
        return getprojectrootdir() / "build/debug/aiko/renderer/libs/"
    else:
        colorPrint(Color.ERROR, "UNKNOWN system")
        raise RuntimeError("Unknown system")

def getshaderincludesBgfxShader() -> Path:
    return getLibsPath() / "bgfx-src/bgfx/src"

def getshaderincludesCommon() -> Path:
    return getLibsPath() / "bgfx-src/bgfx/examples/common"

def getshadercpath() -> Path:
    currentpath = getLibsPath() / "bgfx-build/cmake/bgfx"
    shaderc_path = Path(currentpath) / "shaderc"
    if SYSTEM == "windows":
        shaderc_path = shaderc_path.with_suffix(".exe")

    if not shaderc_path.exists():
        raise FileNotFoundError(f"shaderc not found at {shaderc_path} (Shaderc not compiled?)")
    return shaderc_path

def find_shader_files(shader_dir: Path, recursive: bool) -> list[Path]:
    exts = {".vs", ".fs", ".cs"}
    it = shader_dir.rglob("*") if recursive else shader_dir.iterdir()
    return [f for f in it if f.is_file() and f.suffix in exts]

def detect_backend_for_system():
    # NOTE: Your original code hard-coded dx11 for windows, spirv for linux.
    # Keep behavior identical unless you want to use the profiles list.
    if SYSTEM == "windows":
        return ("dx11", "windows", "s_5_0")  # folder, platform, profile
    elif SYSTEM == "linux":
        return ("spirv", "linux", "spirv")
    else:
        raise RuntimeError(f"Unsupported SYSTEM={SYSTEM}")

def shader_type_from_suffix(shader: Path) -> str | None:
    if shader.suffix == ".vs":
        return "vertex"
    if shader.suffix == ".fs":
        return "fragment"
    if shader.suffix == ".cs":
        return "compute"
    return None

def compile_one(shader: Path, shaderc_path: Path, folder: str, platform_name: str, profile: str):
    shader_type = shader_type_from_suffix(shader)
    if shader_type is None:
        return (shader, False, f"Skipping {shader.name} (unknown shader type)")

    output_dir = getprojectrootdir() / f"build/assets/shaders/{folder}"
    output_dir.mkdir(parents=True, exist_ok=True)
    output_file = output_dir / f"{shader.stem}.{shader.suffix[1:]}.bin"

    local_varying = shader.parent / "varying.def.sc"
    if not local_varying.exists():
        local_varying = getshaderincludesCommon() / "varying.def.sc"

    cmd = [
        str(shaderc_path),
        "-f", str(shader),
        "-o", str(output_file),
        "--type", shader_type,
        "--platform", platform_name,
        "--profile", profile,
        "-i", str(shader.parent),
        "-i", str(getshaderincludesBgfxShader()),
        "-i", str(getshaderincludesCommon()),
        "--varyingdef", str(local_varying),
    ]

    # Run
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        msg = (
            f"FAILED: {shader}\n"
            f"cmd: {' '.join(cmd)}\n"
            f"stdout:\n{result.stdout}\n"
            f"stderr:\n{result.stderr}\n"
        )
        return (shader, False, msg)

    return (shader, True, f"OK: {shader} -> {output_file}")

def main():
    shader_dir = getprojectrootdir() / "assets/shaders/bgfx"
    onlyfiles = find_shader_files(shader_dir, recursive=False)

    if not onlyfiles:
        return 1

    shaderc_path = getshadercpath()
    folder, platform_name, profile = detect_backend_for_system()

    # Pick worker count: leave 1 core free, but at least 1.
    cpu = os.cpu_count() or 4
    workers = max(1, cpu - 1)

    colorPrint(Color.SECTION, f"Compiling {len(onlyfiles)} shaders with {workers} workers...")

    failures = []
    with ThreadPoolExecutor(max_workers=workers) as ex:
        futures = {
            ex.submit(compile_one, f, shaderc_path, folder, platform_name, profile): f
            for f in onlyfiles
        }
        for fut in as_completed(futures):
            shader = futures[fut]
            try:
                _, ok, msg = fut.result()
            except Exception as e:
                ok = False
                msg = f"EXCEPTION: {shader}\n{e}"

            if ok:
                colorPrint(Color.SUCCESS, msg)
            else:
                colorPrint(Color.ERROR, msg)
                failures.append(shader)

    if failures:
        colorPrint(Color.ERROR, f"{len(failures)} shader(s) failed.")
        return 420

    colorPrint(Color.SUCCESS, "All shaders compiled successfully!")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())