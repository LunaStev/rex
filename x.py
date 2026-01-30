#!/usr/bin/env python3
import os
import platform
import subprocess
import shutil
import sys
import time
import argparse

import psutil       # pip install psutil
import cpuinfo      # pip install py-cpuinfo

def run(cmd, cwd=None):
    print("> " + " ".join(cmd))
    subprocess.check_call(cmd, cwd=cwd)

def print_system_info():
    print("\n==============================")
    print("🖥️  SYSTEM INFORMATION")
    print("==============================")
    print(f"OS: {platform.system()} {platform.release()} ({platform.version()})")
    print(f"Architecture: {platform.machine()}")
    print(f"Python Version: {platform.python_version()}")
    print(f"Processor: {cpuinfo.get_cpu_info().get('brand_raw', 'Unknown')}")
    print(f"Physical Cores: {psutil.cpu_count(logical=False)}")
    print(f"Logical Cores: {psutil.cpu_count(logical=True)}")
    print(f"RAM Total: {round(psutil.virtual_memory().total / (1024**3), 2)} GB")

    try:
        if platform.system() == "Windows":
            out = subprocess.check_output(
                ["wmic", "path", "win32_videocontroller", "get", "name"],
                stderr=subprocess.DEVNULL,
            )
            gpus = [line.strip() for line in out.decode().split("\n") if line.strip() and "Name" not in line]
            print("GPU(s): " + ", ".join(gpus))
        elif platform.system() == "Darwin":
            out = subprocess.check_output(["system_profiler", "SPDisplaysDataType"], stderr=subprocess.DEVNULL)
            lines = [l.strip() for l in out.decode().split("\n") if "Chipset Model" in l]
            print("GPU(s): " + ", ".join([l.split(':')[1].strip() for l in lines]))
        else:
            out = subprocess.check_output(["bash", "-c", "lspci | grep -i vga || true"], stderr=subprocess.DEVNULL)
            gpus = [line.strip() for line in out.decode().split("\n") if line.strip()]
            print("GPU(s): " + (", ".join(gpus) if gpus else "Unknown"))
    except Exception as e:
        print(f"GPU: Unknown ({e})")

    try:
        out = subprocess.check_output(["bash", "-c", "sdl2-config --version 2>/dev/null || echo Unknown"])
        print(f"SDL2 Version: {out.decode().strip()}")
    except Exception:
        print("SDL2 Version: Unknown")

    print("==============================\n")

def detect_jobs():
    logical = psutil.cpu_count(logical=True)
    physical = psutil.cpu_count(logical=False)
    return logical or physical or 1

def configure(build_dir, cmake_args):
    os.makedirs(build_dir, exist_ok=True)
    run(["cmake", "-S", ".", "-B", build_dir] + cmake_args)

def build(build_dir, jobs, config):
    cmd = ["cmake", "--build", build_dir, "--parallel", str(jobs)]
    if config:
        cmd += ["--config", config]
    run(cmd)

def install(build_dir, prefix, config, destdir=None):
    cmd = ["cmake", "--install", build_dir, "--prefix", prefix]
    if config:
        cmd += ["--config", config]
    env = os.environ.copy()
    if destdir:
        env["DESTDIR"] = destdir
    print("> " + " ".join(cmd) + (f" (DESTDIR={destdir})" if destdir else ""))
    subprocess.check_call(cmd, env=env)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("cmd", nargs="?", default="build", choices=["build", "install", "clean"])
    parser.add_argument("--build-dir", default="build")
    parser.add_argument("--config", default="Release")  # Debug/Release
    parser.add_argument("--prefix", default="/usr/local")
    parser.add_argument("--destdir", default=None)
    parser.add_argument("--no-examples", action="store_true")
    parser.add_argument("--no-runtime", action="store_true")
    parser.add_argument("--sdk", action="store_true")
    args = parser.parse_args()

    print_system_info()
    jobs = detect_jobs()
    print(f"🧩 Using up to {jobs} threads for compilation\n")

    if args.cmd == "clean":
        if os.path.isdir(args.build_dir):
            print(f"🧹 removing {args.build_dir}")
            shutil.rmtree(args.build_dir)
        return

    cmake_args = [f"-DCMAKE_BUILD_TYPE={args.config}"]

    if args.no_examples:
        cmake_args += ["-DREX_BUILD_EXAMPLES=OFF"]
    if args.no_runtime:
        cmake_args += ["-DREX_INSTALL_RUNTIME=OFF"]
    if args.sdk:
        cmake_args += ["-DREX_INSTALL_SDK=ON"]

    cmake_args += ["-DREX_BUILD_LAUNCHER=ON"]

    start = time.time()
    try:
        configure(args.build_dir, cmake_args)
        build(args.build_dir, jobs, args.config)

        if args.cmd == "install":
            install(args.build_dir, args.prefix, args.config, args.destdir)

    except subprocess.CalledProcessError as e:
        print(f"❌ Failed with error code {e.returncode}")
        sys.exit(1)

    dur = time.time() - start
    print(f"\n⏱️ Done in {int(dur//60)}m {dur%60:.2f}s")
    print("✅ OK")

if __name__ == "__main__":
    main()