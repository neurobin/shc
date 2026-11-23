# eshc — Enhanced Shell Script Compiler (增强型 Shell 脚本编译器)

**eshc** is a security-hardened fork of the classic `shc` (Shell Script Compiler). It eliminates the critical plaintext source exposure vulnerability in original `shc` during script execution, while preserving full compatibility with interactive `stdin`, `$0` process arguments, and cross-architecture Linux environments.

**eshc** 是经典工具 `shc` 的深度安全加固分支。它彻底修复了原版 `shc` 在运行时通过命令行参数暴露明文源码的严重安全缺陷，同时完整保留了对交互式标准输入（`stdin`）、`$0` 进程名以及跨架构 Linux 环境的兼容性。

---

## 🔒 Core Security Enhancements | 核心安全增强

| Feature / 特性 | Original `shc` / 原版 `shc` | Hardened `eshc` / 加固版 `eshc` |
| :--- | :--- | :--- |
| **Execution Transport / 源码传输方式** | Passes plaintext via `bash -c "..."`, exposing source code in `ps aux` and `/proc/PID/cmdline`.<br>通过 `-c` 参数传递明文，源码直接暴露在进程列表中。 | Streams source via **anonymous memory pipe (`pipe` + `bash -s`)**.<br>通过**匿名内存管道**流式注入，进程列表零暴露。 |
| **Memory & Core Dump / 内存与转储防护** | Vulnerable to Core Dump memory leaks on crash.<br>进程崩溃时可能生成包含明文的 Core Dump 文件。 | Enforces **`prctl(PR_SET_DUMPABLE, 0)`** and zeroes plaintext buffers immediately after piping.<br>内核级禁用 Core Dump，传输完毕立即擦除内存明文。 |
| **Stdin & `$0` Preservation / 输入与参数还原** | Supported natively.<br>原生支持。 | Backs up `stdin` via high FD (`fd 199`) to prevent collisions with script FDs (`3–100`), and restores `$0` via `BASH_ARGV0`.<br>使用高位描述符 `fd 199` 还原标准输入（避免与业务脚本的 `flock` 句柄冲突），并通过 `BASH_ARGV0` 还原 `$0`。 |
| **Cross-Architecture / 跨架构兼容** | POSIX / Unix | Uses standard POSIX syscalls (`pipe`, `fork`, `dup2`, `waitpid`). Works seamlessly on **x86-64** and **ARM / Raspberry Pi (32/64-bit)**.<br>纯标准系统调用，无缝兼容 **x86-64** 与 **树莓派/ARM**。 |

---

## 🛠️ Build & Installation | 编译与安装

```bash
git clone https://github.com<your-username>/eshc.git
cd eshc
./autogen.sh
./configure
make CFLAGS="-static -O2"
sudo make install
```

---

## 🚀 Usage | 使用方法

Compile a shell script into a hardened standalone binary (`-U` enables anti-debugging protection; `-r` allows redistribution across identical architectures):

将脚本编译为加固后的独立二进制文件（`-U` 开启反调试保护，`-r` 支持跨同架构机器分发）：

```bash
shc -U -r -f your_script.sh -o your_app
```

---

## 🛡️ Recommended `systemd` Hardening | 推荐服务加固配置

```ini
[Service]
ExecStart=/usr/local/bin/your_app
LimitCORE=0
CapabilityBoundingSet=~CAP_SYS_PTRACE
SystemCallFilter=~@debug
```

---

## 📜 License | 开源协议

Based on original `shc` by Francisco Javier Rosales García and `neurobin/shc`. Distributed under the **GNU General Public License v3.0 (GPLv3)**.

