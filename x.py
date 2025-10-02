import os
import platform
import subprocess
import shutil

def run(cmd):
    print("> " + " ".join(cmd))
    subprocess.check_call(cmd)

def main():
    # Ensure build dir exists
    build_dir = "build"
    os.makedirs(build_dir, exist_ok=True)
    os.chdir(build_dir)

    # Configure
    run(["cmake", ".."])

    system = platform.system()
    print(f"Detected OS: {system}")

    # --- Windows ---
    if system == "Windows":
        # Visual Studio / MSVC / MinGW
        run(["cmake", "--build", "."])

    # --- Linux ---
    elif system == "Linux":
        if os.path.exists("Makefile"):
            run(["make", f"-j{os.cpu_count()}"])
        elif os.path.exists("build.ninja"):
            run(["ninja"])
        else:
            run(["cmake", "--build", "."])

    # --- macOS ---
    elif system == "Darwin":
        if os.path.exists("Makefile"):
            run(["make", f"-j{os.cpu_count()}"])
        elif shutil.which("xcodebuild"):
            run(["xcodebuild", "-configuration", "Release"])
        elif os.path.exists("build.ninja"):
            run(["ninja"])
        else:
            run(["cmake", "--build", "."])

    # --- FreeBSD, OpenBSD, NetBSD ---
    elif "BSD" in system:
        if shutil.which("gmake"):
            run(["gmake", f"-j{os.cpu_count()}"])
        elif os.path.exists("Makefile"):
            run(["make", f"-j{os.cpu_count()}"])
        else:
            run(["cmake", "--build", "."])

    # --- Fallback ---
    else:
        print(f"⚠️ Unknown system {system}, using cmake --build")
        run(["cmake", "--build", "."])

    print("✅ Build succeeded!")

if __name__ == "__main__":
    main()
