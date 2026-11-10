#pragma once

#include <cstdio>
#include <cstring>
#ifdef __SWITCH__
#include <switch.h>
#endif

namespace wgnx::i18n {
enum class Language { English, ChineseSimplified };
inline Language language = Language::English;
struct Translation { const char* english; const char* chinese; };
inline constexpr Translation translations[] = {
    {"Off", "关闭"},
    {"On", "开启"},
    {"WireGuard-NX SysModule\n          is not running!", "WireGuard-NX 系统模块\n          未运行！"},
    {"No servers configured!", "尚未配置服务器！"},
    {"Servers  |   Auto Start  |   Toggle", "服务器  |   自启动  |   开关"},
    {" Only one peer can be running! Stop others first.", " 同时只能运行一个节点，请先停止其他节点。"},
    {" Only one peer can be enabled for auto-start.", " 只能为一个节点启用自动启动。"},
    {"Stop MITM", "停止 MITM"},
    {"Connection Info", "连接信息"},
    {"No active peer", "没有活动节点"},
    {"State: ", "状态："},
    {"Detail: ", "详情："},
    {"Address: ", "地址："},
    {"Endpoint: ", "端点："},
    {"Last Handshake: ", "上次握手："},
    {"Last RX: ", "上次接收："},
    {",   Last TX: ", "，上次发送："},
    {"RX: ", "接收："},
    {",   TX: ", "，发送："},
    {"Keepalive: ", "保活："},
    {"false", "关闭"},
    {"Probe: ", "探测："},
    {"Error: ", "错误："},
    {"Never", "从未"},
    {"s ago", " 秒前"},
    {"m ago", " 分钟前"},
    {"h ago", " 小时前"},
    {"inactive", "未启用"},
    {"resolving", "解析中"},
    {"handshaking", "握手中"},
    {"active", "已启用"},
    {"error", "错误"},
    {"unknown", "未知"},
    {"configured but stopped", "已配置，未启动"},
    {"resolving endpoint", "正在解析端点"},
    {"established", "已建立连接"},
    {"local failure", "本地故障"},
    {"none", "无"},
    {"config", "配置"},
    {"resolve", "解析"},
    {"handshake", "握手"},
    {"transport", "传输"},
    {"internal", "内部"},
    {"config invalid", "配置无效"},
    {"endpoint missing", "缺少端点"},
    {"endpoint malformed", "端点格式错误"},
    {"endpoint resolution failed", "端点解析失败"},
    {"transport init failed", "传输初始化失败"},
    {"internal failure", "内部故障"},
    {"key invalid", "密钥无效"},
    {"handshake init failed", "握手初始化失败"},
    {"transport open failed", "传输打开失败"},
    {"transport send failed", "传输发送失败"},
    {"transport receive failed", "传输接收失败"},
    {"handshake timed out", "握手超时"},
    {"ping-tunnel-peer", "隧道节点 Ping"},
    {"ping-public-dns", "公网 DNS Ping"},
    {"queued", "已排队"},
    {"sent", "已发送"},
    {"reply validated", "响应验证通过"},
    {"reply rejected", "响应被拒绝"},
    {"timed out", "已超时"},
    {"stale activation", "激活已过期"},
    {"invalid state", "状态无效"},
    {"build failed", "构造失败"},
    {"send failed", "发送失败"},
    {"failed", "失败"},
    {"Initializing socket\n", "正在初始化套接字\n"},
    {"Initializing nifm\n", "正在初始化网络服务\n"},
    {"Exit by pressing + or -\n", "按 + 或 - 退出；方向键上/下滚动\n"},
    {"Press ZL to send a debug ping to 10.13.13.1 through the tunnel\n", "按 ZL 通过隧道向 10.13.13.1 发送调试 Ping\n"},
    {"Press ZR to send a debug ping to 1.1.1.1 through the tunnel\n", "按 ZR 通过隧道向 1.1.1.1 发送调试 Ping\n"},
    {"Press X to recreate the active tunnel's UDP binding\n", "按 X 重新创建活动隧道的 UDP 绑定\n"},
    {"%d arguments\n", "%d 个参数\n"},
    {"Manager Version: %s\n", "管理程序版本：%s\n"},
    {"nxlink host is %s\n", "nxlink 主机：%s\n"},
    {"Current IP: %u.%u.%u.%u\n", "当前 IP：%u.%u.%u.%u\n"},
    {"WireGuard IPC API: v%u\n", "WireGuard IPC API：v%u\n"},
    {"Sysmodule Version: %s-%s\n", "系统模块版本：%s-%s\n"},
    {"WireGuard peers: %u | active=%d | autostart=%d | flags=0x%08X\n", "WireGuard 节点：%u | 活动=%d | 自启动=%d | 标志=0x%08X\n"},
    {"[%u] %s | %s | endpoint=%s | local_pub=%s | state=%s stage=%s flags=0x%02X | hs=%d rx_age=%d tx_age=%d", "[%u] %s | %s | 端点=%s | 本地公钥=%s | 状态=%s 阶段=%s 标志=0x%02X | 握手=%d 接收=%d 发送=%d"},
    {"<unavailable>", "<不可用>"},
    {" | probe=%s/%s/%ds", " | 探测=%s/%s/%d秒"},
    {" | ka=%us err=%s (0x%08X)\n", " | 保活=%u秒 错误=%s (0x%08X)\n"},
    {"WireGuard IPC service '%s' is not running.\n", "WireGuard IPC 服务 '%s' 未运行。\n"},
    {"HOS Version: %u.%u.%u", "HOS 版本：%u.%u.%u"},
    {"TriggerDebugPayload: IPC service is not running.\n", "调试探测：IPC 服务未运行。\n"},
    {"TriggerDebugPayload(%s): %s (0x%08X)\n", "调试探测(%s)：%s (0x%08X)\n"},
    {"BumpUdpBinding: IPC service is not running.\n", "重建 UDP 绑定：IPC 服务未运行。\n"},
    {"BumpUdpBinding: %s (0x%08X)\n", "重建 UDP 绑定：%s (0x%08X)\n"},
};

inline const char* tr(const char* text) {
    if (language == Language::ChineseSimplified) {
        for (const auto& entry : translations) {
            if (std::strcmp(text, entry.english) == 0) return entry.chinese;
        }
    }
    return text;
}

// Returns false for unknown values so callers can retain the system-language fallback.
inline bool setLanguage(const char* value) {
    if (std::strcmp(value, "en") == 0) { language = Language::English; return true; }
    if (std::strcmp(value, "zh-CN") == 0) { language = Language::ChineseSimplified; return true; }
    return false;
}

inline void initialize() {
    language = Language::English;
#ifdef __SWITCH__
    if (R_SUCCEEDED(setInitialize())) {
        u64 code = 0;
        if (R_SUCCEEDED(setGetSystemLanguage(&code))) {
            // Language codes are packed ASCII (zh-CN, zh-TW, zh-Hans, zh-Hant).
            if ((code & 0xffff) == (static_cast<u64>('h') << 8 | 'z'))
                language = Language::ChineseSimplified;
        }
        setExit();
    }
    if (FILE* file = std::fopen("sdmc:/config/wireguard-nx/language.txt", "r")) {
        char value[32] = {};
        if (std::fgets(value, sizeof(value), file)) {
            value[std::strcspn(value, "\r\n")] = '\0';
            setLanguage(value);
        }
        std::fclose(file);
    }
#endif
}
} // namespace wgnx::i18n
