![Drawing](https://github.com/user-attachments/assets/341a14bd-a23a-4742-ac67-bb30a29c6892)

# Goldspace
Crappy Unix-like kernel I'm making. We've got the bootloader, the kernel, the linker script, the Gash shell, the Makefile...
It's practically better than Linux.

# Gash
Now onto the shell, Gash. Gash is a portmanteau of "Gold" and "Bash", and is designed specifically for Goldspace. How the hell do I give it input, you ask? Well, the core of the kernel has a keyboard driver inside of it!

# Compilation instructions:
* `git clone https://github.com/Goldside543/goldspace.git`
* `cd goldspace`
* `make -j4`

# Command list:
* `help` displays help.
* `clear` clears the screen.
* `echo <message>` allows you to type in something, and it'll be repeated!
* `render` does a rendering test on the GPU.
* `panic` causes a kernel panic.
* `builddate` prints build date and time.

# Documentation
See the Documentation directory for user and developer manuals.
# Donate
Hey, thanks for considering donating! Below are some crypto addresses you can donate to!

* BTC: bc1qn9zjpgvl63j0dtm29hzkhfe6vxul7nn3w34xek
* LTC: ltc1q524te5cj87ylffcq43jwx7rw8r4l8yl3nrqyc3
* DOGE: DSsFPmvePCAAUJyt6u1Smwz8tJLSrs4nz4
* BCH: qrw04k2gt8gkggqg6sgu3s3y436nlawapgv4t4h87g
* XMR: 44JqnEYZcCQViedo82qWP2Cp2H9jkfatWKdGPfdBK3Lybe46XEdqMVgNTKWwLAhDHDTR7dtum3hUNRRMd2SPZfxME7m8hYL
# Version number: Beta 1.2.0
Yeah, my versioning is totally screwed up, but I don't care.

Now that Goldspace can enter ring 3, it will be entering the Beta stage :)

Full release will be entered once I have a decent amount of work done, like program support and a vast array of hardware support and system calls :D
