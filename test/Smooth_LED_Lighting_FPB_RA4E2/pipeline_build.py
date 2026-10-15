import os
import subprocess
import sys

# Set environment variables
os.environ["ARM_LLVM_TOOLCHAIN_PATH"] = (
    r"/opt/ATfE-21.1.1-Linux-AArch64/bin"
)
#os.environ["RASC_EXE_PATH"] = (
#    r"C:\Renesas\RA\sc_v2026-04.2_fsp_v6.5.0\eclipse\rasc.exe"
#)

# Cmake configuration
config_cmd = [
    "cmake",
    "-DCMAKE_TOOLCHAIN_FILE=cmake/llvm.cmake",
    "-DCMAKE_BUILD_TYPE=Debug",
    "-G",
    "Ninja",
    "-B",
    "build/Debug",
]

print("Starting CMake configuration...")
result = subprocess.run(config_cmd)
if result.returncode != 0:
    sys.exit(result.returncode)

# Cmake build
build_cmd = ["cmake", "--build", "build/Debug"]

print("Building the project...")
result = subprocess.run(build_cmd)
sys.exit(result.returncode)
