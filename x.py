#!/usr/bin/env python3
from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

COMMANDS = {"all", "configure", "build", "clean", "rebuild", "install", "run"}
DEFAULT_BUILD_TYPE = "Debug"
DEFAULT_BUILD_DIR = "build"


def _require_tool(name: str) -> None:
    if shutil.which(name):
        return
    raise RuntimeError(
        f"Required tool `{name}` was not found in PATH. "
        "Install it first and retry."
    )


def _run(cmd: list[str], cwd: Path) -> None:
    print("+", " ".join(cmd))
    subprocess.run(cmd, cwd=str(cwd), check=True)


def _resolve_path(root: Path, value: str) -> Path:
    path = Path(value)
    if path.is_absolute():
        return path
    return root / path


def _configure(args: argparse.Namespace) -> None:
    _require_tool("cmake")
    _require_tool("cargo")
    _require_tool("rustc")

    build_dir = _resolve_path(args.project_root, args.build_dir)
    build_dir.mkdir(parents=True, exist_ok=True)

    cmd = [
        "cmake",
        "-S",
        str(args.project_root),
        "-B",
        str(build_dir),
        f"-DCMAKE_BUILD_TYPE={args.build_type}",
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
    ]
    if args.generator:
        cmd.extend(["-G", args.generator])

    _run(cmd, args.project_root)


def _build(args: argparse.Namespace) -> None:
    _require_tool("cmake")
    _require_tool("cargo")
    _require_tool("rustc")

    build_dir = _resolve_path(args.project_root, args.build_dir)
    cmd = ["cmake", "--build", str(build_dir)]

    if args.jobs and args.jobs > 0:
        cmd.extend(["--parallel", str(args.jobs)])
    if args.target:
        cmd.extend(["--target", args.target])
    if args.build_type:
        cmd.extend(["--config", args.build_type])

    _run(cmd, args.project_root)


def _clean(args: argparse.Namespace) -> None:
    build_dir = _resolve_path(args.project_root, args.build_dir)
    if build_dir.exists():
        shutil.rmtree(build_dir)
        print(f"Removed build directory: {build_dir}")
    else:
        print(f"Build directory does not exist: {build_dir}")


def _install(args: argparse.Namespace) -> None:
    _require_tool("cmake")

    build_dir = _resolve_path(args.project_root, args.build_dir)
    cmd = ["cmake", "--install", str(build_dir)]
    if args.prefix:
        cmd.extend(["--prefix", args.prefix])
    if args.build_type:
        cmd.extend(["--config", args.build_type])
    _run(cmd, args.project_root)


def _rebuild(args: argparse.Namespace) -> None:
    _clean(args)
    _configure(args)
    _build(args)


def _run_binary(args: argparse.Namespace) -> None:
    build_dir = _resolve_path(args.project_root, args.build_dir)
    target_name = "rex-editor" if args.run_target == "editor" else "rex-runtime"
    binary = build_dir / target_name
    if sys.platform == "win32":
        binary = binary.with_suffix(".exe")

    if not binary.exists():
        raise FileNotFoundError(
            f"Cannot find {target_name} at {binary}. Run `python3 x.py` first."
        )

    forwarded = list(args.run_args)
    if forwarded and forwarded[0] == "--":
        forwarded = forwarded[1:]

    _run([str(binary), *forwarded], args.project_root)


def _build_all(args: argparse.Namespace) -> None:
    if args.clean_first:
        _clean(args)
    _configure(args)
    _build(args)


def _parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Rex build utility",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "command",
        nargs="?",
        choices=sorted(COMMANDS),
        default="all",
        help="Action to execute",
    )
    parser.add_argument(
        "run_target",
        nargs="?",
        choices=["editor", "runtime"],
        help="Binary to run when command is `run`",
    )
    parser.add_argument(
        "run_args",
        nargs=argparse.REMAINDER,
        help="Arguments forwarded to the selected binary (for `run`)",
    )
    parser.add_argument(
        "--project-root",
        default=str(Path(__file__).resolve().parent),
        help="Project root path",
    )
    parser.add_argument("-B", "--build-dir", default=DEFAULT_BUILD_DIR)
    parser.add_argument(
        "-t",
        "--build-type",
        default=DEFAULT_BUILD_TYPE,
        choices=["Debug", "Release", "RelWithDebInfo", "MinSizeRel"],
    )
    parser.add_argument("-j", "--jobs", type=int, default=os.cpu_count() or 4)
    parser.add_argument("-G", "--generator", default="")
    parser.add_argument("--target", default="")
    parser.add_argument("--prefix", default="")
    parser.add_argument(
        "--clean-first",
        action="store_true",
        help="Delete build directory before running the default `all` command",
    )

    args = parser.parse_args()
    args.project_root = Path(args.project_root).resolve()
    return args


def main() -> int:
    try:
        args = _parse_args()
        print(f"Rex Build Tool (root={args.project_root})")

        if args.command == "all":
            _build_all(args)
        elif args.command == "configure":
            _configure(args)
        elif args.command == "build":
            _build(args)
        elif args.command == "clean":
            _clean(args)
        elif args.command == "rebuild":
            _rebuild(args)
        elif args.command == "install":
            _install(args)
        elif args.command == "run":
            if not args.run_target:
                args.run_target = "editor"
            _run_binary(args)
        else:
            raise ValueError(f"Unknown command: {args.command}")
    except subprocess.CalledProcessError as exc:
        print(f"Command failed with exit code {exc.returncode}", file=sys.stderr)
        return exc.returncode
    except Exception as exc:  # keep error handling concise for CLI
        print(f"Error: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
