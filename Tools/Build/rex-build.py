#!/usr/bin/env python3
import os
import sys
import subprocess

def main():
    print("Rex Engine Build Tool v1.0")
    if len(sys.argv) < 2:
        print("Usage: rex-build <project_path>")
        return

    project_path = sys.argv[1]
    build_path = os.path.join(project_path, "build")
    
    if not os.path.exists(build_path):
        os.makedirs(build_path)
    
    # Run CMake
    subprocess.run(["cmake", "-S", project_path, "-B", build_path])
    # Build
    subprocess.run(["cmake", "--build", build_path])
    
    print(f"Build completed in {build_path}")

if __name__ == "__main__":
    main()
