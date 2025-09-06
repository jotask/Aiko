from os import listdir
from os.path import isfile, join
from pathlib import Path
import sys
import os
import subprocess
from rich import print

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def compileshader(shader: Path):

    print(f"[purple]{shader}[/purple]")

    shaderc_path = getshadercpath()

    # detect type by filename convention
    if shader.suffix == ".vs":
        shader_type = "vertex"
        shader_profile = "50"
    elif shader.suffix == ".fs":
        shader_type = "fragment"
        shader_profile = "50"
    elif shader.suffix == ".cs":
        shader_type = "compute"
        shader_profile = "cs_5_0"
    else:
        print(f"Skipping {shader.name} (unknown shader type)")
        return

    output_dir = Path(__file__).parents[1].resolve() / "assets/build/shaders"
    output_dir.mkdir(parents=True, exist_ok=True)
    output_file = output_dir / f"{shader.stem}.{shader.suffix[1:]}.bin"

    print   (f"[red]output:[/red] {output_file}")

    varyingfile = str(shader.parent)
    print(varyingfile)

    cmd = [
        str(shaderc_path),
        "-f", str(shader),
        "-o", str(output_file),
        "--type", shader_type,
        "--platform", "windows",   # adjust if targeting another platform
        "--profile", shader_profile,        # GLSL profile, e.g., 120, 150, 300_es
        "-i", varyingfile,  # add folder containing varying.def.sc
        "-i", str(getshaderincludesBgfxShader()),
        "-i", str(getshaderincludesCommon()),

    ]

    print("[bold yellow]Running:[/bold yellow]", " ".join(cmd))
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(bcolors.FAIL + "Shader compilation failed!" + bcolors.ENDC)
        print("[bold red]STDOUT:[/bold red]", result.stdout)
    else:
        print("[bold green]Shader compiled successfully![/bold green]")

    print("[bold yellow]------------------------------------------------------------------------------------------------------------------------------------------------------------[/bold yellow]")

def getshaderincludesBgfxShader() -> Path:
    currentpath = Path(__file__).parents[1].resolve() / "deps_cache/bgfx-src/bgfx/src"
    return currentpath

def getshaderincludesCommon() -> Path:
    currentpath = Path(__file__).parents[1].resolve() / "deps_cache/bgfx-src/bgfx/examples/common"
    return currentpath

def getshadercpath() -> Path:
    currentpath = Path(__file__).parents[1].resolve() / "deps_cache/bgfx-build/cmake/bgfx/Debug"
    shaderc_path = Path(currentpath) / "shaderc.exe"
    if( os.path.exists(shaderc_path) == False):
        print("Shaderc not compiled?")
        sys.exit(3)
    
    return shaderc_path

def getoutpath() -> Path:
    shaderc_path = Path("C:/path/to/bgfx/.build/win64_vs2022/bin/shaderc.exe")
    return shaderc_path

def main():


    currentpath = Path(__file__).parents[1].resolve() / "assets/shaders/bgfx"
    onlyfiles = [f for f in currentpath.iterdir() if f.is_file() and f.suffix in (".vs", ".fs", ".cs")]

    if(len(onlyfiles) == 0):
        sys.exit(1)

    for file in onlyfiles:
        compileshader(file)

if __name__ == "__main__":
    main()
