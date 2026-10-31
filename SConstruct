import shutil
import subprocess
import sys
import time
import os

from SCons.Script import *

from build_scripts.config import apply_environment_variables
from build_scripts.config import DEPENDENCIES, INSTALL_COMMANDS

SetOption("no_exec", False)
SetOption("silent", True)

pm = None
DMOS_APT_NO_UPDATE = int(os.environ.get("DMOS_APT_NO_UPDATE", 0))
GCC = "toolchain/i686-elf/bin/i686-elf-gcc"
FIRST_BUILD_TEST = "rebuild.txt"

FOLDERS = [
    "build",
    "build/fs/boot/grub",
    "dist",
]

if not os.path.exists(FIRST_BUILD_TEST):
    f = open(FIRST_BUILD_TEST, "w")
    f.close()
    print("=" * 60)
    print("\nThis build may install packages that require root permissions")
    print("=" * 60)
    try:
        input("Press enter to continue, or ctrl-c to exit: ")
    except KeyboardInterrupt:
        print("Exiting")
        try:
            subprocess.run(["rm", "rebuild.txt"], check=True)
        except:
            pass
        sys.exit(0)

###############################################
# Misc.
###############################################
def create_folders(folders, p=False):
    for folder in folders:
        cmd = ["mkdir", folder]
        if p:
            cmd.insert(1, "-p")
        run_silent(cmd, check=True)

def clean_build():
    if os.path.exists("build"):
        print("[+] Deleting previous build")
        run_silent(["rm", "-rf", "build"])

def cp(src, dest, silent=False):
    run_silent(["cp", src, dest], silent=silent, check=True)

def run_silent(command, silent=False, **kwargs):
    if not silent:
        print(f"[=]\t{" ".join(command)}")
    try:
        return subprocess.run(command, stdout=subprocess.DEVNULL, **kwargs)
    except:
        print("[-] Error while running " + " ".join(command))
        sys.exit(1)

def create_directory_structure():
    create_folders(FOLDERS, p=True)

###############################################
# scons s
###############################################
if "s" in COMMAND_LINE_TARGETS:
    clean_build()
    result = subprocess.run(["scons"], check=False)
    Exit(result.returncode)


if os.path.exists("dist"):
    print("[+] Deleting previous dist")
    run_silent(["rm", "-rf", "dist"])

###############################################
# Packages
###############################################
def initialize_package_manager():
    global pm
    pm = get_package_manager()
    if DMOS_APT_NO_UPDATE:
        print("[+] DMOS_APT_NO_UPDATE in use")
    elif pm == "apt":
        print("[+] Updating...")
        run_silent(["sudo", "apt-get", "update"], check=True)
        print("[+] Updated")

def get_package_manager():
    managers = {
        "apt": "apt-get",
        "dnf": "dnf",
        "pacman": "pacman",
        "zypper": "zypper",
        "apk": "apk"
    }
    for manager, executable in managers.items():
        if shutil.which(executable):
            return manager

def toolchain_check():
    try:
        result = subprocess.run([GCC, "--version"], capture_output=True, check=True)
    except FileNotFoundError:
        print("[-] \tThis build requires a special toolchain to be installed")
        print("[-] \tWithout it, the build will not run.")
        print("[-] \tIt may take 7-20 minutes to install\n")
        yn = input("[-] (Y/n) Install? ")
        if yn.lower().startswith("y"):
            print("[+] Installing toolchain...")
            subprocess.run(["./build_scripts/create_toolchain.sh"], check=True)
        else:
            print("[-] No toolchain found, exiting...")
            sys.exit(2)

def install_packages(*names):
    run_silent(["sudo", *INSTALL_COMMANDS[pm], *names], check=True)

def ensure_dependencies():
    uninstalled = []
    for package in DEPENDENCIES[pm]:
        if not shutil.which(DEPENDENCIES[pm][package]):
            print(f"[-] Could not find {package}")
            uninstalled.append(package)
    if uninstalled:
        install_packages(*uninstalled)

###############################################
# discover sources
###############################################
def discover_sources():
    asm_sources = []
    c_sources = []

    for root, dirs, files in os.walk("src"):
        dirs.sort()
        files.sort()

        for filename in files:
            path = os.path.join(root, filename)

            if filename.endswith(".asm"):
                asm_sources.append(path)
            elif filename.endswith(".c"):
                c_sources.append(path)

    return asm_sources, c_sources

def object_path(source):
    relative = os.path.relpath(source, "src")
    return os.path.join(
        "build",
        "obj",
        os.path.splitext(relative)[0] + ".o"
    )

###############################################
# Compilation
###############################################
def compile_asm(target, source, env):
    os.makedirs(os.path.dirname(str(target[0])), exist_ok=True)

    run_silent([
        "nasm",
        "-f", "elf32",
        str(source[0]),
        "-o", str(target[0]),
    ], check=True, silent=True)

    print(f"NASM {str(source[0]):<50} --> {target[0]}")

    return 0

def compile_c(target, source, env):
    os.makedirs(os.path.dirname(str(target[0])), exist_ok=True)

    run_silent([
        GCC,
        "-c", str(source[0]),
        "-o", str(target[0]),
        "-std=c99",
        "-g", "-O3",
        "-ffreestanding",
        "-fno-stack-protector",
        "-fno-pie",
        "-m32",
        "-Isrc/kernel",
        "-Isrc/kernel/include"
    ], check=True, silent=True)

    print(f"CC   {str(source[0]):<50} --> {target[0]}")

    return 0

def link_kernel(target, source, env):
    objects = [
        str(item)
        for item in source
        if str(item) != "src/kernel/linker.ld"
    ]

    run_silent([
        GCC,
        "-T", "src/kernel/linker.ld",
        "-o", str(target[0]),
        "-g",
        "-ffreestanding",
        "-nostdlib",
        "-no-pie",
        "-m32",
        "-Wl,--build-id=none",
        "-Wl,-Map,build/kernel.map",
        *objects,
        "-lgcc",
    ], check=True, silent=True)

    run_silent(["toolchain/i686-elf/bin/i686-elf-strip", "--strip-debug", str(target[0])])

    print(f"LD   {'...':<50} --> {target[0]}")

    return 0

###############################################
# Kernel targets
###############################################
asm_sources, c_sources = discover_sources()
objects = []

for source in asm_sources:
    target = object_path(source)
    target = target.rsplit(".", 1)[0] + "_asm.o"

    objects.append(
        Command(
            target,
            source,
            compile_asm,
        )
    )

for source in c_sources:
    objects.append(
        Command(
            object_path(source),
            source,
            compile_c,
        )
    )

if not objects:
    print("[-] No .asm or .c files found under src/")
    Exit(1)

kernel = Command(
    "build/kernel.elf",
    objects + ["src/kernel/linker.ld"],
    link_kernel,
)

###############################################
# GRUB and ISO creation
###############################################
grub_cfg = Command(
    "build/fs/boot/grub/grub.cfg",
    "boot/grub.cfg",
    Copy("$TARGET", "$SOURCE"),
)

kernel_copy = Command(
    "build/fs/boot/kernel.elf",
    kernel,
    Copy("$TARGET", "$SOURCE"),
)

def create_iso(target, source, env):
    print(f"GRUB {'build/fs':<50} --> {target[0]}\n\n")
    run_silent([
        "grub-mkrescue",
        "-o", str(target[0]),
        "build/fs",
    ], check=True, silent=True)
    return 0

iso = Command(
    "dist/DiamondOS.iso",
    [grub_cfg, kernel_copy],
    create_iso,
)

###############################################
# Setup
###############################################
def setup():
    initialize_package_manager()
    print(f"[+] Found package manager: {pm}")
    create_directory_structure()
    ensure_dependencies()
    toolchain_check()
    print(f"[+] Toolchain found at {GCC}")
    apply_environment_variables()
    print("[+] Environment variables applied\n\n")

setup()

Default(iso)
