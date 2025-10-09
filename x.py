import os
import platform
import subprocess
import shutil
import psutil  # pip install psutil
import cpuinfo  # pip install py-cpuinfo
import sys
import time

def run(cmd):
    print("> " + " ".join(cmd))
    subprocess.check_call(cmd)

def print_system_info():
    print("\n==============================")
    print("🖥️  SYSTEM INFORMATION")
    print("==============================")
    print(f"OS: {platform.system()} {platform.release()} ({platform.version()})")
    print(f"Architecture: {platform.machine()}")
    print(f"Python Version: {platform.python_version()}")
    print(f"Processor: {cpuinfo.get_cpu_info()['brand_raw']}")
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

def main():
    build_dir = "build"
    os.makedirs(build_dir, exist_ok=True)
    os.chdir(build_dir)

    print_system_info()

    logical_cores = psutil.cpu_count(logical=True)
    physical_cores = psutil.cpu_count(logical=False)
    max_cores = logical_cores or physical_cores or 1
    print(f"🧩 Using up to {max_cores} threads for compilation\n")

    run(["cmake", ".."])

    system = platform.system()
    print(f"Detected OS: {system}\n")

    start_time = time.time()

    try:
        if system == "Windows":
            if shutil.which("msbuild"):
                run(["msbuild", "ALL_BUILD.vcxproj", f"/m:{max_cores}"])
            elif shutil.which("ninja"):
                run(["ninja", f"-j{max_cores}"])
            else:
                run(["cmake", "--build", ".", "--", f"/m:{max_cores}"])

        elif system == "Linux":
            if os.path.exists("build.ninja"):
                run(["ninja", f"-j{max_cores}"])
            elif os.path.exists("Makefile"):
                run(["make", f"-j{max_cores}"])
            else:
                run(["cmake", "--build", ".", "--", f"-j{max_cores}"])

        elif system == "Darwin":
            if os.path.exists("build.ninja"):
                run(["ninja", f"-j{max_cores}"])
            elif os.path.exists("Makefile"):
                run(["make", f"-j{max_cores}"])
            elif shutil.which("xcodebuild"):
                run(["xcodebuild", "-configuration", "Release", "-parallelizeTargets", f"-jobs", str(max_cores)])
            else:
                run(["cmake", "--build", ".", "--", f"-j{max_cores}"])

        elif "BSD" in system:
            if shutil.which("gmake"):
                run(["gmake", f"-j{max_cores}"])
            elif os.path.exists("Makefile"):
                run(["make", f"-j{max_cores}"])
            else:
                run(["cmake", "--build", ".", "--", f"-j{max_cores}"])

        else:
            print(f"⚠️ Unknown system {system}, using cmake --build")
            run(["cmake", "--build", ".", "--", f"-j{max_cores}"])

    except subprocess.CalledProcessError as e:
        print(f"❌ Build failed with error code {e.returncode}")
        sys.exit(1)

    end_time = time.time()
    duration = end_time - start_time
    minutes = int(duration // 60)
    seconds = duration % 60

    print(f"\n⏱️ Build completed in {minutes}m {seconds:.2f}s ({duration:.2f} seconds total)")

    print("\n✅ Build succeeded!")

if __name__ == "__main__":
    main()
