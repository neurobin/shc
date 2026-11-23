# eshc — Enhanced Shell Script Compiler (增强型 Shell 脚本编译器)

**eshc** is a security-hardened fork of the classic `shc` (Shell Script Compiler) for modern Linux production environments, eliminating plaintext source exposure and 128KB size limits while preserving interactive `stdin` and `$0` compatibility. 

**eshc** 是经典工具 `shc` 针对现代 Linux 生产环境深度安全加固的增强分支，彻底解决了进程列表中暴露源码明文以及 128KB 脚本体积上限的问题，同时完整保留交互式标准输入（`stdin`）、`$0` 进程名及跨架构兼容性。

---

## 📖 Background & Motivation | 分支起因
* **Plaintext Exposure / 源码明文暴露**：Vanilla `shc` exposes the script via `bash -c "..."` in `systemctl status` and `ps aux`. 原版通过 `-c` 导致源码在 `ps aux` 中明文可见。
* **128KB Size Limit / 脚本体积限制**：Bypasses Linux `MAX_ARG_STRLEN` (`~128KB`) execution limits. 突破了 Linux 单参数最大长度（约 128KB）的限制。

---

## 🔒 Core Security Enhancements | 核心增强特性
* **Execution Transport / 传输方式**：Replaces `bash -c` with anonymous memory pipes (`pipe` + `bash -s`) for zero-exposure streaming. 使用匿名内存管道流式注入，进程列表零暴露。
* **Memory & Core Dump / 内存防护**：Enforces `prctl(PR_SET_DUMPABLE, 0)` and zeroes plaintext buffers after piping. 内核级禁用 Core Dump 并立即擦除内存明文。
* **Stdin & `$0` / 输入与参数**：Backs up `stdin` via high FD (`fd 199`) and restores `$0` via `BASH_ARGV0`. 通过高位句柄 `fd 199` 无损还原标准输入，并通过 `BASH_ARGV0` 还原 `$0`。
* **Cross-Architecture / 跨架构**：Seamlessly works on x86-64 and ARM / Raspberry Pi. 无缝兼容 x86-64 与 ARM/树莓派。

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

## 🚀 Usage & Deployment | 使用与部署示例
1. **Encrypting a Script / 加密脚本**：`shc -U -r -f your_script.sh -o your_app`
2. **Recommended `systemd` Hardening / 推荐服务配置**：
   ```ini
   [Service]
   ExecStart=/usr/local/bin/your_app
   LimitCORE=0
   CapabilityBoundingSet=~CAP_SYS_PTRACE
   SystemCallFilter=~@debug
   ```

---

## ⚠️ Limitations & Requirements | 缺陷与环境要求
* **Interpreter Version / 解释器版本**：Full `\$0` and high FD support requires **Bash 5.0+**. Minimal POSIX shells (e.g., `dash`, `ash`) execute the script but may show the interpreter path for `\$0`. 完整加固特性建议使用 Bash 5.0 及以上版本。
* **Process Tree Depth / 进程树层级**：Adds one extra process level due to `fork()` + `pipe()`; verify ancestor PIDs up to `PID 1`. 进程树层级会比原版多出一层，校验时建议向上追溯至 `PID 1`。
* **OS Kernel / 操作系统限制**：`prctl(PR_SET_DUMPABLE, 0)` is Linux-specific. 仅在 Linux 内核下生效。

---

## 🙏 Acknowledgments & License | 致谢与开源协议
* **Acknowledgments / 致谢**：Special thanks to Francisco Javier Rosales García, neurobin, and open-source contributors. 特别感谢原版 `shc` 创始人及社区贡献者。
* **License / 开源协议**：Based on original `shc`, distributed under the **GNU General Public License v3.0 (GPLv3)**.

