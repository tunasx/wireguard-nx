#!/usr/bin/env python3
"""Prepare the pinned Moonlight backend without discarding local changes."""

import json
from pathlib import Path
import subprocess
import sys

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
ENGINE = HERE / "upstream"
DEPENDENCIES = json.loads((HERE / "dependencies.json").read_text(encoding="utf-8"))


def git(repo: Path, *args: str, check: bool = True) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        ["git", "-C", str(repo), *args], capture_output=True, text=True,
        encoding="utf-8", errors="replace", check=False,
    )
    if check and result.returncode:
        raise RuntimeError(result.stderr.strip())
    return result


def require_repository(repo: Path) -> None:
    result = git(repo, "rev-parse", "--show-toplevel", check=False)
    if result.returncode or Path(result.stdout.strip()).resolve() != repo.resolve():
        raise RuntimeError(f"Initialize the submodule first: {repo}")


def require_revision(repo: Path, revision: str) -> None:
    require_repository(repo)
    actual = git(repo, "rev-parse", "HEAD").stdout.strip()
    if actual != revision:
        raise RuntimeError(f"Unexpected revision at {repo}: {actual}, expected {revision}")


def apply_patch(repo: Path, patch: Path) -> None:
    if git(repo, "apply", "--reverse", "--check", str(patch), check=False).returncode == 0:
        return
    git(repo, "apply", "--check", "--whitespace=error-all", str(patch))
    git(repo, "apply", "--whitespace=error-all", str(patch))


def main() -> int:
    try:
        require_revision(ENGINE, DEPENDENCIES["engine_commit"])
        require_revision(ROOT / "common/lib/libnx", DEPENDENCIES["libnx_commit"])
        require_revision(ENGINE / "third_party/libtesla", DEPENDENCIES["libtesla_commit"])
        atmosphere = ENGINE / "third_party/Atmosphere"
        require_repository(atmosphere)
        revision = DEPENDENCIES["atmosphere_commit"]
        if git(atmosphere, "rev-parse", "HEAD").stdout.strip() != revision:
            if git(atmosphere, "status", "--porcelain").stdout.strip():
                raise RuntimeError("Atmosphere has local changes; refusing to change its revision.")
            if git(atmosphere, "cat-file", "-e", revision + "^{commit}", check=False).returncode:
                git(atmosphere, "fetch", "--depth", "1", "origin", revision)
            git(atmosphere, "checkout", "--detach", revision)
        require_revision(atmosphere, revision)
        apply_patch(ENGINE, HERE / "firmware-guard.patch")
        for patch in sorted((ENGINE / "patches").glob("atmosphere-*.patch")):
            apply_patch(atmosphere, patch)
        print("Moonlight backend ready: pinned TCP/UDP implementation and HOS 23.0.1 libraries")
        return 0
    except (OSError, RuntimeError) as error:
        print(f"Moonlight preparation failed: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
