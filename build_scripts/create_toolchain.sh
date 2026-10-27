#!/usr/bin/env bash

# i hate shell

set -euo pipefail

ROOT="$(pwd)"

TARGET="i686-elf"
BINUTILS_VERSION="2.40"
GCC_VERSION="11.2.0"

PREFIX="$ROOT/toolchain/$TARGET"
SOURCES="$ROOT/toolchain-src"
BUILD="$ROOT/toolchain-build"

die() {
    printf 'Error: %s\n' "$*" >&2
    exit 1
}

command -v sudo >/dev/null 2>&1 || die "sudo is required to install system dependencies."
command -v wget >/dev/null 2>&1 || true

# find package manager
if command -v apt-get >/dev/null 2>&1; then
    PM="apt"
elif command -v dnf >/dev/null 2>&1; then
    PM="dnf"
elif command -v yum >/dev/null 2>&1; then
    PM="yum"
elif command -v pacman >/dev/null 2>&1; then
    PM="pacman"
elif command -v zypper >/dev/null 2>&1; then
    PM="zypper"
elif command -v apk >/dev/null 2>&1; then
    PM="apk"
else
    die "No supported package manager found (apt, dnf, yum, pacman, zypper, apk). Install the dependencies manually."
fi

printf 'Detected package manager: %s\n' "$PM"

case "$PM" in
    apt)
        PACKAGES=(
            build-essential bison flex
            libgmp-dev libmpfr-dev libmpc-dev
            texinfo wget xz-utils
        )
        INSTALL=(sudo apt-get install -y)
        sudo apt-get update
        ;;
    dnf)
        PACKAGES=(
            gcc gcc-c++ make bison flex
            gmp-devel mpfr-devel libmpc-devel
            texinfo wget xz
        )
        INSTALL=(sudo dnf install -y)
        ;;
    yum)
        PACKAGES=(
            gcc gcc-c++ make bison flex
            gmp-devel mpfr-devel libmpc-devel
            texinfo wget xz
        )
        INSTALL=(sudo yum install -y)
        ;;
    pacman)
        PACKAGES=(
            base-devel bison flex
            gmp mpfr libmpc
            texinfo wget xz
        )
        INSTALL=(sudo pacman -S --needed --noconfirm)
        ;;
    zypper)
        PACKAGES=(
            gcc gcc-c++ make bison flex
            gmp-devel mpfr-devel mpc-devel
            texinfo wget xz
        )
        INSTALL=(sudo zypper --non-interactive install)
        ;;
    apk)
        PACKAGES=(
            build-base bison flex
            gmp-dev mpfr-dev mpc1-dev
            texinfo wget xz
        )
        INSTALL=(sudo apk add)
        ;;
esac

"${INSTALL[@]}" "${PACKAGES[@]}"

# check if commands exist
for tool in make gcc g++ bison flex wget tar xz; do
    command -v "$tool" >/dev/null 2>&1 || die "Required command '$tool' is missing after dependency installation."
done

mkdir -p "$SOURCES" "$BUILD" "$PREFIX"

cd "$SOURCES"

wget -nc "https://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.xz"
wget -nc "https://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.xz"

# extract if doesnt exist already
if [[ ! -d "$SOURCES/binutils-$BINUTILS_VERSION" ]]; then
    tar -xf "binutils-$BINUTILS_VERSION.tar.xz"
fi

if [[ ! -d "$SOURCES/gcc-$GCC_VERSION" ]]; then
    tar -xf "gcc-$GCC_VERSION.tar.xz"
fi

# build binutils
mkdir -p "$BUILD/binutils"
cd "$BUILD/binutils"

"$SOURCES/binutils-$BINUTILS_VERSION/configure" \
    --target="$TARGET" \
    --prefix="$PREFIX" \
    --with-sysroot \
    --disable-nls \
    --disable-werror

make -j"$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 1)"
make install

# build C cross-compiler.
mkdir -p "$BUILD/gcc"
cd "$BUILD/gcc"

"$SOURCES/gcc-$GCC_VERSION/configure" \
    --target="$TARGET" \
    --prefix="$PREFIX" \
    --disable-nls \
    --enable-languages=c \
    --without-headers \
    --disable-shared \
    --disable-threads \
    --disable-libssp \
    --disable-libquadmath \
    --disable-libgomp

make -j"$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 1)" all-gcc all-target-libgcc
make install-gcc install-target-libgcc

"$PREFIX/bin/$TARGET-gcc" --version
"$PREFIX/bin/$TARGET-gcc" -dumpmachine
"$PREFIX/bin/$TARGET-ld" --version

rm -rf "$SOURCES" "$BUILD"

