from pathlib import Path
import sys
import os
import subprocess
from rich import print

class bcolors:
    FAIL = '\033[91m'
    ENDC = '\033[0m'

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

def compileshader(shader: Path):
    print(f"[purple]{shader}[/purple]")

    shaderc_path = getshadercpath()

    print(shader.suffix)

    # detect type by filename suffiax
    if shader.suffix == ".vs":
        shader_type = "vertex"
        profile = "s_5_0"
    elif shader.suffix == ".fs":
        shader_type = "fragment"
        profile = "s_5_0"
    elif shader.suffix  == ".cs":
        shader_type = "compute"
    else:
        print(f"Skipping {shader.name} (unknown shader type)")
        return
    
    folder = "dx11"
    platform = "windows"

    # compile profiler
    output_dir = Path(__file__).parents[1].resolve() / f"assets/build/shaders/{folder}"
    output_dir.mkdir(parents=True, exist_ok=True)
    output_file = output_dir / f"{shader.stem}.{shader.suffix[1:]}.bin"

    print(f"[red]output:[/red] {output_file}")

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
    ]

    print("[bold yellow]Running:[/bold yellow]", " ".join(cmd))
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(bcolors.FAIL + "Shader compilation failed!" + bcolors.ENDC)
        print("[bold red]STDOUT:[/bold red]", result.stdout)
        sys.exit(420);
    else:
        print("[bold green]Shader compiled successfully![/bold green]")

    print("[bold yellow]" + "-" * 150 + "[/bold yellow]")

def getshaderincludesBgfxShader() -> Path:
    return Path(__file__).parents[1].resolve() / "deps_cache/bgfx-src/bgfx/src"

def getshaderincludesCommon() -> Path:
    return Path(__file__).parents[1].resolve() / "deps_cache/bgfx-src/bgfx/examples/common"

def getshadercpath() -> Path:
    currentpath = Path(__file__).parents[1].resolve() / "deps_cache/bgfx-build/cmake/bgfx/Debug"
    shaderc_path = Path(currentpath) / "shaderc.exe"
    if not shaderc_path.exists():
        print("Shaderc not compiled?")
        sys.exit(3)
    return shaderc_path

def main():
    shader_dir = Path(__file__).parents[1].resolve() / "assets/shaders/bgfx"
    onlyfiles = [f for f in shader_dir.iterdir() if f.is_file() and f.suffix in (".vs", ".fs", ".cs")]

    if not onlyfiles:
        sys.exit(1)

    for file in onlyfiles:
        compileshader(file)

if __name__ == "__main__":
    main()
