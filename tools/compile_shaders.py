from pathlib import Path
import sys
import subprocess
import platform
from enum import Enum

SYSTEM = platform.system().lower()  # 'windows', 'linux', or 'darwin'

class Profile:
    def __init__(self, platform: str, profile: str, folder: str):
        self.platform = platform
        self.profile = profile
        self.folder = folder

# All backends + their shader profiles
profiles = [
    # Direct3D11
    Profile("windows", "vs_5_0", "dx11"),
    Profile("windows", "ps_5_0", "dx11"),

    # Direct3D12
    Profile("windows", "vs_5_1", "dx12"),
    Profile("windows", "ps_5_1", "dx12"),

    # OpenGL (GLSL 1.50 core)
    Profile("windows", "150", "glsl"),
    Profile("linux",   "150", "glsl"),
    Profile("osx",     "150", "glsl"),

    # OpenGLES
    Profile("windows", "300_es", "essl"),
    Profile("linux",   "300_es", "essl"),
    Profile("osx",     "300_es", "essl"),
    Profile("windows", "100_es", "essl"),
    Profile("linux",   "100_es", "essl"),
    Profile("osx",     "100_es", "essl"),

    # Vulkan (SPIR-V)
    Profile("windows", "spirv", "spirv"),
    Profile("linux",   "spirv", "spirv"),
    Profile("osx",     "spirv", "spirv"),

    # Metal
    Profile("osx", "metal", "metal"),
    Profile("ios", "metal", "metal"),
]

class Color(Enum):
    INFO    = "37"
    SECTION = "1;35"
    SUCCESS = "32"
    WARNING = "33"
    ERROR   = "31"

def colorPrint(severity: Color, msg: str):
    print(f"\033[{severity.value}m{msg}\033[0m")
    pass

def compileshader(shader: Path):

    colorPrint(Color.SECTION, shader)

    shaderc_path = getshadercpath()

    # detect platform constants
    if SYSTEM == "windows":
        folder = "dx11"
        platform = "windows"
        profile = "s_5_0"
    elif SYSTEM == "linux":
        folder = "spirv"
        platform = "linux"
        profile = "spirv"

    # detect type by filename suffiax
    if shader.suffix == ".vs":
        shader_type = "vertex"
    elif shader.suffix == ".fs":
        shader_type = "fragment"
    elif shader.suffix  == ".cs":
        shader_type = "compute"
    else:
        colorPrint(Color.INFO, f"Skipping {shader.name} (unknown shader type)")
        return

    # compile profiler
    output_dir = getprojectrootdir() / f"build/assets/shaders/{folder}"
    output_dir.mkdir(parents=True, exist_ok=True)
    output_file = output_dir / f"{shader.stem}.{shader.suffix[1:]}.bin"

    colorPrint(Color.ERROR,f"output: {output_file}")

     # determine varying.def.sc for this shader
    local_varying = shader.parent / "varying.def.sc"
    if not local_varying.exists():
        # fallback to common include if no local varying
        local_varying = getshaderincludesCommon() / "varying.def.sc"

    cmd = [
        str(shaderc_path),
        "-f", str(shader),
        "-o", str(output_file),
        "--type", shader_type,
        "--platform", platform,
        "--profile", profile,
        "-i", str(shader.parent),
        "-i", str(getshaderincludesBgfxShader()),
        "-i", str(getshaderincludesCommon()),
        "--varyingdef", str(local_varying),
    ]

    colorPrint(Color.WARNING, f"Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        colorPrint(Color.ERROR,"Shader compilation failed!")
        colorPrint(Color.ERROR,f"STDOUT: {result.stdout}")
        sys.exit(420);
    else:
        colorPrint(Color.SUCCESS, "Shader compiled successfully!")

    colorPrint(Color.SECTION,f"------------------------------------------------------------------")

def getprojectrootdir():
    return Path(__file__).parents[1].resolve()

def getLibsPath():
    if SYSTEM == "windows":
        return getprojectrootdir() / "build/debug/aiko/libs/"
    elif SYSTEM == "linux":
        return getprojectrootdir() / "build/debug/aiko/renderer/libs/"
    else:
        colorPrint(Color.ERROR,"UNKNOWN system")
        sys.exit(4)

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
        colorPrint(Color.ERROR,"Shaderc not compiled?")
        sys.exit(3)
    return shaderc_path

def find_shader_files(shader_dir: Path, recursive : bool) -> list[Path]:
    exts = { ".vs", ".fs", ".cs" }

    if recursive:
        it = shader_dir.rglob("*")
    else:
        it = shader_dir.iterdir()

    return [f for f in it if f.is_file() and f.suffix in exts]

def main():
    shader_dir = getprojectrootdir() / "assets/shaders/bgfx"
    onlyfiles = find_shader_files(shader_dir, False)

    if not onlyfiles:
        sys.exit(1)

    for file in onlyfiles:
        compileshader(file)

if __name__ == "__main__":
    main()
