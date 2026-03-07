#!/usr/bin/env bash
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "Compiling native library..."

# ── Detect OS ────────────────────────────────────────────────────────────────
OS="$(uname -s)"
case "$OS" in
    Linux*)   PLATFORM=linux;  JNI_PLATFORM=linux;  LIB_NAME=libcanalize_native.so  ;;
    Darwin*)  PLATFORM=macos;  JNI_PLATFORM=darwin; LIB_NAME=libcanalize_native.dylib ;;
    *)        echo "ERROR: Unsupported OS: $OS"; exit 1 ;;
esac
echo "Detected platform: $PLATFORM"

# ── Detect JAVA_HOME ─────────────────────────────────────────────────────────
if [ -z "$JAVA_HOME" ]; then
    echo "JAVA_HOME not set. Attempting to detect..."
    if command -v java &>/dev/null; then
        JAVA_HOME="$(java -XshowSettings:properties -version 2>&1 \
            | grep 'java.home' | awk '{print $3}')"
        # On macOS, java.home may point to jre/ subdirectory; go up one level if needed
        [ ! -f "$JAVA_HOME/include/jni.h" ] && JAVA_HOME="$(dirname "$JAVA_HOME")"
    fi
fi
if [ -z "$JAVA_HOME" ] || [ ! -f "$JAVA_HOME/include/jni.h" ]; then
    echo "ERROR: JAVA_HOME not set or jni.h not found at $JAVA_HOME/include/jni.h"
    exit 1
fi
echo "Using JAVA_HOME: $JAVA_HOME"

# ── Detect compiler ──────────────────────────────────────────────────────────
if command -v g++ &>/dev/null; then
    GPP=g++
elif command -v clang++ &>/dev/null; then
    GPP=clang++
else
    echo "ERROR: No C++ compiler found (g++ or clang++)."
    exit 1
fi
echo "Using compiler: $GPP"

# ── Detect AVX2 support ───────────────────────────────────────────────────────
AVX2_FLAG="-mavx2"
if [ "$PLATFORM" = "macos" ]; then
    # Apple Silicon (arm64) does not support AVX2; skip on non-x86_64
    ARCH="$(uname -m)"
    if [ "$ARCH" != "x86_64" ]; then
        AVX2_FLAG=""
        echo "Note: AVX2 disabled on $ARCH (Apple Silicon)."
    fi
fi

# ── Compile ───────────────────────────────────────────────────────────────────
SOURCES="jni.cpp src/TerrainGen.cpp src/Carver.cpp src/Decorator.cpp src/WorldLoader.cpp"
INCLUDES="-I\"$JAVA_HOME/include\" -I\"$JAVA_HOME/include/$JNI_PLATFORM\" -Iinclude"

OUTDIR="../../../build/libs"
NATIVES_DIR="../../../src/main/resources/natives"

mkdir -p "$OUTDIR"
mkdir -p "$NATIVES_DIR"

EXTRA_FLAGS=""
if [ "$PLATFORM" = "macos" ]; then
    # -undefined dynamic_lookup allows JNI symbols to resolve at runtime
    EXTRA_FLAGS="-undefined dynamic_lookup"
fi

eval "$GPP" -shared -fPIC -std=c++17 -O3 $AVX2_FLAG $EXTRA_FLAGS \
    $INCLUDES \
    $SOURCES \
    -o "$OUTDIR/$LIB_NAME"

echo "Build successful: $OUTDIR/$LIB_NAME"

# ── Copy to resources ─────────────────────────────────────────────────────────
cp "$OUTDIR/$LIB_NAME" "$NATIVES_DIR/"
echo "Copied to: $NATIVES_DIR/$LIB_NAME"
