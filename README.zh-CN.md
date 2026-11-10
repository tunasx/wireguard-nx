# WireGuard-NX 简体中文版

本 fork 为管理程序（NRO）和 Ultrahand 悬浮菜单（OVL）增加英文 / 简体中文支持，并通过 GitHub Actions 编译四个组件。

## 下载与安装

1. 打开本仓库的 **Actions → Build Switch packages**，选择成功的运行。
2. 下载 `wireguard-nx-sdcard` artifact。解开 artifact 后，再解压里面的 `wireguard-nx-sdcard.zip` 到 SD 卡根目录。
3. 按 `config/wireguard-nx/example.conf.example` 的示例建立自己的 WireGuard 配置。示例文件不会覆盖现有配置。
4. 重启 Switch 加载系统模块。管理程序位于 `switch/wireguard-nx/wireguard-nx.nro`；菜单位于 `switch/.overlays/wireguard-nx.ovl`，需要已有的 Ultrahand 环境。

安装包沿用上游 `make dist` 行为：两个系统模块均包含 `boot2.flag`，重启时自动加载。

## 语言

默认跟随 Switch 系统语言：中文系统显示简体中文，其他语言显示英文。
可创建 `sd:/config/wireguard-nx/language.txt`，第一行填入：

- `zh-CN`：强制简体中文（也可以将 `language.txt.example` 复制为 `language.txt`）。
- `en`：强制英文。
- `auto` 或删除该文件：跟随系统。

更改后重新打开管理程序或重新加载悬浮菜单。这里只提供简体中文翻译，繁体系统默认也使用简体中文。

翻译表位于 `common/include/wgnx/i18n.hpp`，不存在的翻译回退到原始英文。协议字段、配置关键字、服务名及系统模块日志保持稳定。界面中的节点名称和地址仍显示用户自己的配置。

菜单复用 libultrahand 的中文字库支持。管理程序改用 UTF-8 字体渲染，直接读取 Switch 系统共享字体；安装包不包含 Nintendo 字体。管理程序按方向键上/下滚动，+/- 退出，其余调试操作与上游一致。

## 构建及验证

Actions 支持推送、Pull Request 和手动运行。使用 devkitPro 的 `devkita64` 容器，递归拉取固定提交的子模块，编译 WireGuard 系统模块、MITM 系统模块、管理程序和悬浮菜单，并输出 SD 卡目录结构的 ZIP。宿主机协议测试、MITM 测试、语言回退测试和格式占位符检查在独立任务中执行。

编译成功不等于真机验证完成。安装前请备份现有配置；本项目上游仍标注为实验性，不能仅凭隧道握手成功就假定所有游戏或 Moonlight 流量均可通过。有关透明流量接管的限制，请查阅上游 README 和 `mitm-sysmodule/README.md`。
