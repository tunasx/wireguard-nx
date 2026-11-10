#!/usr/bin/env python3
"""Apply the retained WireGuard-NX fixes to the pinned official Atmosphere-libs."""

from pathlib import Path
import subprocess
import sys


ROOT = Path(__file__).resolve().parents[1]
LIBRARIES = ROOT / "common/lib/Atmosphere-libs"
PATCH = ROOT / "common/patches/atmosphere-libs-wgnx.patch"
REVISION = "cf48d23375cf36742876f4b014971b2ebabc6dc8"


def git(*args: str) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        ["git", "-C", str(LIBRARIES), *args],
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def main() -> int:
    revision = git("rev-parse", "HEAD")
    if revision.returncode != 0 or revision.stdout.strip() != REVISION:
        print(
            "Atmosphere-libs is not at the supported revision. Run "
            "'git submodule sync --recursive' and "
            "'git submodule update --init --recursive' first.",
            file=sys.stderr,
        )
        if revision.returncode != 0:
            print(revision.stderr, file=sys.stderr)
        return 1

    if git("apply", "--reverse", "--check", str(PATCH)).returncode == 0:
        print("Atmosphere-libs: official 23.0.1 support and WGNX fixes ready")
        return 0

    check = git("apply", "--check", "--whitespace=error-all", str(PATCH))
    if check.returncode != 0:
        print(
            "Cannot apply the WGNX library fixes; existing changes were preserved.\n"
            + check.stderr,
            file=sys.stderr,
        )
        return 1

    apply = git("apply", "--whitespace=error-all", str(PATCH))
    if apply.returncode != 0:
        print(apply.stderr, file=sys.stderr)
        return 1

    print("Atmosphere-libs: applied WGNX fixes to official 23.0.1 support")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
