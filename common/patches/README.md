# Atmosphere-libs integration

The submodule follows the official `Atmosphere-NX/Atmosphere-libs` repository,
pinned to `cf48d23375cf36742876f4b014971b2ebabc6dc8` (Atmosphere 1.12.1,
Horizon 23.0.1). Both sysmodules statically link this library. The previous fork
was based on 22.1.0 and aborted during version initialization on 23.x, before
the module's `ams::Main` could run.

`common/lib/libnx` pins the official Switchbrew library at
`b156b4774fb2df7b93402169509266da7547fc4c`. Its 23.x filesystem APIs, including
`FsStorageForBatchRead`, are required by the new Atmosphere-libs. Actions builds
and installs this version into its disposable devkitPro container before building
any project component, rather than relying on the container's older bundled libnx.

`atmosphere-libs-wgnx.patch` retains these project dependencies from
`chrisbraucker/Atmosphere-libs` at `03a6b211c8d3934bf40f367da227437aaf2bde2e`:

- Bounded storage for MITM query objects (no newlib heap).
- Bounded allocation and lifetime management for forwarded service objects.
- Initialized address lengths in `RecvFrom` and `GetSockName`.

The old fork's copy-handle cleanup patch is intentionally omitted: official
upstream now closes the forwarded request's copy handles itself. Applying both
implementations would risk closing a handle twice.

The `libs` target in each sysmodule runs `tools/prepare_atmosphere_libs.py` before
compilation. The script checks the pinned revision, applies the patch atomically,
and accepts an already applied patch. A conflicting local edit stops the build
without resetting or discarding files. The patched submodule consequently shows
local changes; the tracked patch is their reproducible source.

After updating an existing checkout, run:

```sh
git submodule sync --recursive
git submodule update --init --recursive
make -C common/lib/libnx install -j2
make -C wg-sysmodule clean
make -C mitm-sysmodule clean
make -C wg-sysmodule dist
make -C mitm-sysmodule dist
make -C wg-sysmodule resource-report
```

The libnx installation needs write access to `$DEVKITPRO/libnx` in the development
environment. It is required once when this pinned dependency changes.

The official firmware compatibility guard remains enabled. Both module entry
points also require 23.0.1 support at compile time, so an older dependency cannot
silently produce a module that will reject this firmware at boot.

Building successfully is not on-device validation. Verify EAD0 alone first on
HOS 23.0.1 / Atmosphere 1.12.1, then enable EAD3, test tunnel traffic and shutdown,
and collect any new fatal reports with the matching ELF artifact. The separate
`2021-0003` fatal/service-manager session exhaustion requires its own device test.
