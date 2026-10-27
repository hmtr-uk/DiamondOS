import os

root = os.getcwd()

VARIABLES = {
    "DMOS_BUILD_DIR": f"{root}/build",
    "DMOS_SRC_DIR": f"{root}/src",
}


DEPENDENCIES = {
    "apt": {
        "nasm": "nasm",
        "grub-pc-bin": "grub-install",
        "grub-common": "grub-mkrescue",
        "xorriso": "xorriso",
        "qemu-system-x86": "qemu-system-i386",
        "mtools": "mtools",
    },
    "dnf": {
        "nasm": "nasm",
        "grub2-tools": "grub-install",
        "grub2-tools-extra": "grub2-mkrescue",
        "xorriso": "xorriso",
        "qemu-system-x86": "qemu-system-i386",
        "mtools": "mtools",
    },
    "pacman": {
        "nasm": "nasm",
        "grub": "grub-install",
        "xorriso": "xorriso",
        "qemu-system-x86": "qemu-system-i386",
        "mtools": "mtools",
    },
    "zypper": {
        "nasm": "nasm",
        "grub2": "grub-install",
        "xorriso": "xorriso",
        "qemu-x86": "qemu-system-i386",
        "mtools": "mtools",
    },
    "apk": {
        "nasm": "nasm",
        "grub": "grub-install",
        "xorriso": "xorriso",
        "qemu-system-x86_64": "qemu-system-i386",
        "mtools": "mtools",
    },
}


INSTALL_COMMANDS = {
    "apt": ["apt-get", "install", "-y"],
    "dnf": ["dnf", "install", "-y"],
    "pacman": ["pacman", "-S", "--needed", "--noconfirm"],
    "zypper": ["zypper", "--non-interactive", "install"],
    "apk": ["apk", "add"],
}


def apply_environment_variables():
    for key in VARIABLES:
        os.environ[key] = VARIABLES[key]
