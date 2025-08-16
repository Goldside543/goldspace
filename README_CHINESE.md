![Drawing](https://github.com/user-attachments/assets/341a14bd-a23a-4742-ac67-bb30a29c6892)

# Goldspace
我正在制作的一个类Unix内核。我们已经有了引导加载程序、内核、链接脚本、Gash shell 和 Makefile...
它几乎比 Linux 更好。

# Gash
接下来是 shell，Gash。Gash 是 "Gold" 和 "Bash" 的合成词，专门为 Goldspace 设计。你问我怎么给它输入？好吧，内核的核心部分里面有一个键盘驱动程序！

# 编译说明：
* `git clone https://github.com/Goldside543/goldspace.git`
* `cd goldspace`
* `make -j4`

# 命令列表：
* `help` 显示帮助信息。
* `clear` 清除屏幕。
* `echo <消息>` 允许你输入一些内容，并将其重复显示！
* `render` 在 GPU 上进行渲染测试。
* `panic` 触发内核恐慌。
* `builddate` 打印构建日期和时间。

# 文档
请参阅 Documentation 目录中的用户手册和开发者手册。

# 版本号:
看 README.md
