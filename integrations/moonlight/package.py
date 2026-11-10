#!/usr/bin/env python3
"""Package only the Moonlight module and its matching symbols, never CFW files."""

import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import zipfile

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
ENGINE = HERE / "upstream"


def revision(path: Path) -> str:
    return subprocess.check_output(["git", "-C", str(path), "rev-parse", "HEAD"], text=True).strip()


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", type=Path, default=ROOT / "moonlight-artifacts")
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    module_dir = ENGINE / "switch/sysmodule"
    overlay_dir = ENGINE / "switch/overlay"
    nsp = module_dir / "wireguard-nx.nsp"
    overlay = overlay_dir / "wireguard-nx.ovl"
    if nsp.read_bytes()[:4] != b"PFS0" or overlay.read_bytes()[16:20] != b"NRO0":
        raise RuntimeError("Invalid module or overlay output")
    symbols = {
        "wireguard-nx.elf": module_dir / "wireguard-nx.elf",
        "wireguard-nx.map": module_dir / "build/wireguard-nx.map",
        "wireguard-nx-overlay.elf": overlay_dir / "wireguard-nx.elf",
        "heap_size.h": module_dir / "source/heap_size.h",
    }
    for name, path in symbols.items():
        if not path.is_file() or path.stat().st_size == 0:
            raise RuntimeError(f"Missing debug artifact: {path}")
        if name.endswith(".elf") and path.read_bytes()[:4] != b"\x7fELF":
            raise RuntimeError(f"Invalid ELF: {path}")

    metadata = json.loads((HERE / "dependencies.json").read_text(encoding="utf-8"))
    metadata.update({
        "project_commit": revision(ROOT),
        "engine_commit": revision(ENGINE),
        "atmosphere_commit": revision(ENGINE / "third_party/Atmosphere"),
        "libnx_commit": revision(ROOT / "common/lib/libnx"),
        "module_sha256": hashlib.sha256(nsp.read_bytes()).hexdigest(),
        "overlay_sha256": hashlib.sha256(overlay.read_bytes()).hexdigest(),
        "firmware_guard_sha256": hashlib.sha256((HERE / "firmware-guard.patch").read_bytes()).hexdigest(),
        "heap_size_header": symbols["heap_size.h"].read_text(encoding="utf-8"),
    })
    info = json.dumps(metadata, indent=2) + "\n"
    title = "atmosphere/contents/4200000000005747"
    files = {
        f"{title}/exefs.nsp": nsp,
        f"{title}/toolbox.json": module_dir / "res/toolbox.json",
        "switch/.overlays/wireguard-nx-moonlight.ovl": overlay,
        "config/wireguard-nx/config.ini.example": module_dir / "res/config.ini.example",
        "config/wireguard-nx/moonlight-asus.conf.example": HERE / "moonlight-asus.conf.example",
        "INSTALL-MOONLIGHT.zh-CN.md": HERE / "README.zh-CN.md",
        "licenses/wireguard-nx-GPL-2.0.txt": ENGINE / "LICENSE",
        "licenses/engine-BSD-3-Clause.txt": ENGINE / "crates/LICENSE",
        "licenses/THIRD_PARTY_NOTICES.md": ENGINE / "THIRD_PARTY_NOTICES.md",
    }
    archives = [
        (args.output / "wireguard-nx-moonlight-sdcard.zip", files),
        (args.output / "wireguard-nx-moonlight-debug-symbols.zip", symbols),
    ]
    for destination, entries in archives:
        with zipfile.ZipFile(destination, "w", zipfile.ZIP_DEFLATED) as archive:
            for name, path in entries.items():
                archive.write(path, name)
            archive.writestr("BUILD-DEPENDENCIES.json", info)
            if destination.name.endswith("-sdcard.zip"):
                archive.writestr(f"{title}/flags/boot2.flag", b"")
        with zipfile.ZipFile(destination) as archive:
            if archive.testzip() is not None:
                raise RuntimeError(f"Invalid ZIP: {destination}")
        print(f"Packaged {destination.name}: {destination.stat().st_size} bytes")


if __name__ == "__main__":
    main()
