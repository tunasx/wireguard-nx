#include <stratosphere.hpp>
#include <switch.h>

#include "ipc_service.hpp"
#include "logger.hpp"
#include "wgnx/build_info.hpp"

static_assert(ams::hos::Version_Max >= ams::hos::Version_23_0_1, "WireGuard-NX requires Atmosphere-libs with HOS 23.0.1 support");

namespace ams {
void Main() {
    wgnx::sysmodule::logger::Initialize();
    wgnx::sysmodule::logger::Log("Main entered");
    wgnx::sysmodule::logger::Log("Build: %s", wgnx::build_info::VersionWithBuild);
    wgnx::sysmodule::logger::Log("Starting IPC server");
    if (!wgnx::sysmodule::StartIpcServer()) {
        wgnx::sysmodule::logger::Log("IPC server startup failed");
        return;
    }

    wgnx::sysmodule::WaitForIpcServerShutdownRequest();
    wgnx::sysmodule::logger::Log("IPC graceful shutdown begin");
    wgnx::sysmodule::StopIpcServer();
}
} // namespace ams
