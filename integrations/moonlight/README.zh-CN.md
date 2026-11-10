# Switch 自己连接 WireGuard，再用 Moonlight 串流

这个独立构建使用 [UnderwaterOverground/wireguard-nx](https://github.com/UnderwaterOverground/wireguard-nx)
的 TCP/UDP 引擎与 homebrew BSD 接管实现，在你的仓库通过 **Build Moonlight packages** 工作流编译。
它使用模块 ID `4200000000005747`，是原有 `EAD0/EAD3` 实验模块的替代安装方案。
原管理程序的 IPC 协议与它不同，请使用本包的 **WireGuard Moonlight** 悬浮菜单管理连接。

源码和依赖固定在 `dependencies.json`，Atmosphère 库更新到明确支持系统 23.0.1 的官方提交，
libnx 使用匹配的新接口版本。编译成功不代表已经在你的 Switch、华硕路由器和 PC 上完成串流测试。

## 安装

1. 关机，备份 SD 卡上的 `/config/wireguard-nx` 和原模块目录。
2. 将下面两个文件改名为 `boot2.flag.off`，停用旧模块的开机启动：

   ```text
   /atmosphere/contents/010000000000EAD0/flags/boot2.flag
   /atmosphere/contents/010000000000EAD3/flags/boot2.flag
   ```

3. 将 `wireguard-nx-moonlight-sdcard.zip` 解压到 SD 卡根目录。
   本包只包含新模块、悬浮菜单、说明和示例配置，不附带 Atmosphère/fusee/hekate。
4. 继续使用华硕导出的原 WireGuard `.conf`，放在 `/config/wireguard-nx/`。
   保留原来的地址、密钥和 Endpoint。确认其 `[Peer]` 的 `AllowedIPs` 包含 `192.168.1.100`，
   例如 `192.168.1.0/24`；已有 `0.0.0.0/0` 就已覆盖。
5. 如果该目录有多份 `.conf`，在新悬浮菜单的 **Config** 页面选中正确配置；
   也可在 `/config/wireguard-nx/config.ini` 中设置 `conf=你的文件名.conf`。
   示例文件 `config.ini.example` 可供参考，`mitm=homebrew` 为默认接管范围。
6. 重启后，通过 Ultrahand 打开 **WireGuard Moonlight**，确认连接与握手。
   旧 `wireguard-nx.nro` 和旧悬浮菜单不管理这个模块。

旧模块与新模块不能同时启用：它们会争用配置、隧道身份和 BSD 接管。
如需退回原版，停用 `4200000000005747/flags/boot2.flag`，恢复两个旧标记并重启。

## 连接家里的 192.168.1.100

华硕 WireGuard 服务端保持“访问内网”开启。PC 上运行 Sunshine，并允许相应 VPN 来源通过
Windows 防火墙访问 Sunshine。Switch 当前外部 Wi-Fi 的网段也不要与家里 `192.168.1.0/24` 冲突。

1. 使用完整内存模式进入 hbmenu，再启动 Moonlight；初次验证优先使用这种标准启动方式。
2. 在 Moonlight 中手动添加主机 `192.168.1.100`。
3. 在 PC 的 Sunshine 页面输入 Moonlight 显示的 PIN 完成配对。
4. 首次从 **720p / 60 FPS / H.264 / 10 Mbps** 开始验证，再根据实际链路提高码率。

上游提示 Moonlight 会把私网 IP 当作本地网络并发送较大的数据包。若出现高码率丢帧，
可在已有 `.conf` 中加入这一注释行：

```ini
# Host = home-pc 192.168.1.100
```

它给路由器后面的 PC 定义名称。不要仅把路由器的 `[Peer]` 命名为 PC，因为 peer 的隧道地址
通常属于路由器。新模块默认将主机名映射到替身地址，以引导 Moonlight 使用较小的数据包。
使用此模式需要 Atmosphère 的 hosts 文件存在；保留已有 hosts 文件及其内容。
若 `/atmosphere/hosts/` 完全没有 hosts 配置，可创建空的 `default.txt`，然后重启模块。
之后在 Moonlight 里添加 `home-pc`。

## 出问题时保留这些信息

- `/config/wireguard-nx/sysmodule.log`，及存在时的 `sysmodule.log.1`。
- 新的 Atmosphère `.bin` 崩溃报告。
- 本次 Actions 的 `wireguard-nx-moonlight-debug-symbols`；必须与安装包来自同一次构建。
- Moonlight 的版本、启动方式、报错文字，以及是否能完成配对或进入串流。

不要发送 WireGuard 私钥或预共享密钥。启动兼容、握手、TCP 配对和持续 UDP 视频串流需要分别验证。

## 构建与来源

工作流运行 Rust 核心测试和 Switch 缓冲上限测试，从源码构建固定版本的 libnx、Atmosphère
库、Rust 引擎、系统模块和悬浮菜单，并运行上游的 2.5 MiB 模块内存预算检查。
`prepare.py` 在构建前更新嵌套 Atmosphère 检出并应用上游 MITM 修复，遇到冲突的本地修改会停止。
`package.py` 只收集本配置的产物，验证 ZIP 与调试文件配对，记录完整依赖与构建信息。

上游版权和许可证保留在 `upstream` 子模块及安装包的 `licenses/` 目录中。
