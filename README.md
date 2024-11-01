![Drawing](https://github.com/user-attachments/assets/341a14bd-a23a-4742-ac67-bb30a29c6892)

# Goldspace
Crappy Unix-like kernel I'm making. We've got the bootloader, the kernel, the linker script, the Gash shell, the Makefile...
It's practically better than Linux.
# Gash
Now onto the shell, Gash. Gash is a portmanteau of "Gold" and "Bash", and is designed specifically for Goldspace. How the hell do I give it input, you ask? Well, the core of the kernel has a keyboard driver inside of it!
# Compilation instructions:
* `git clone https://github.com/Goldside543/goldspace.git`
* `cd goldspace`
* `make` <br> <br>
(for compiling as 64-bit, the following commands go between `cd goldspace` and `make`) <br> <br>
* `chmod +x scripts/64-bitifier.sh`
* `sed -i "s|path/to/kernel/source|$(pwd)|" scripts/64-bitifier.sh`
* `scripts/64-bitifier.sh`
# Command list:
* `help` displays help.
* `clear` clears the screen.
* `echo <message>` allows you to type in something, and it'll be repeated!
* `create <filename>` creates a file!
* `write <filename> <data>` writes data to a file!
* `read <filename>` reads a file!
* `delete <filename>` deletes a file!
* `render` does a rendering test on the GPU.
* `panic` causes a kernel panic.
* `builddate` prints build date and time.
* `switchvgamode` switches the VGA mode to 13h.
# For additional documentation...
See the user manual or developer manual, conveniently located in the Documentation directory!
# Version number: v2.9.8
