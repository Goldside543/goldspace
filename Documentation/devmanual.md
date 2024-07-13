# Preface
Welcome to the Goldspace Developer Manual. This manual is intended for wanna-be developers for the Goldspace OS/kernel. 
The world of developing a new kernel like this is, forgive my vulgarity, absolute hell. You're navigating through the frontier of a new kernel, trying to set things up to be hospitable for the end user, the people who truly need it! It'll be cold, unforgiving, mind-numbing, and NOT beginner friendly. <br> <br>

Still not discouraged? Great, welcome aboard.
# Chapter I: Goldspace information
Goldspace is a 32-bit command-line-interface operating system, with a monolithic kernel and built-in bootloader. As of July 12th, 2024, it's around 30 MB compiled.
Packed with it is a custom shell that inherits commands from Bash, named "Gash" as a portmanteau of "Bash" and "Gold".
# Chapter II: Where to begin
You can begin anywhere you want. Write a driver, sort out bugs with Gash, upgrade the kernel with extra cool features. You do you, as long as it works.
We request that you always test changes you make. To clone the repository using GitHub CLI, use `gh repo clone Goldside543/goldspace`. Optionally, if you're wanting to mess with a branch, add the `-- --branch` flags to that command.
# Chapter III: How to submit a change
Unlike Linux, there's no massive mailing list to sort through, no. Unlike Torvalds, I'm not too picky about the GitHub pull request system. Granted, he has his reasons because Linux is massive, but I'm cool with pull requests.
Just make the change and submit a pull request explaining:
* What you changed
* Why you changed it
* Why it's a good change <br> <br>

If it passes all of those, chances are, it'll be merged.
# Chapter IV: Application development
But what's that? What if you don't want to do kernel development and just want to make an app or two? Totally cool.
In your fork, go to/make a directory named "applications". This is where I store applications built into Goldspace.
You will need to compile it without a file name, and as a 32-bit binary. For example, if it's a C application and you're using GCC, `gcc -m32 source.c -o program_name`.
As for languages I support, the list is C, C++, C#, Java, and Rust.
# Chapter V: Goldspace structure
Back on topic, the kernel. If you're adding files, put bootloader things in boot, shell things in gash, and kernel things in kernel. This will allow the source code to be more organized and readable.
# Notice
The following chapter will go into detail on each directory. Skip to whichever one you need.
# Chapter VI; Subchapter I: Boot
Boot's purpose is bootloader files. Not really much to cover here, considering it's just one file.
# Chapter VI; Subchapter II: Gash
Gash's purpose is Gash source code files. In here, you'll mostly be working on shell.c, where commands are defined.
# Chapter VI; Subchapter III: Kernel
Kernel... ooh, boy. Kernel contains core.c, the main part of the kernel, which includes the keyboard driver, the welcome message, command execution, etc. Then there's io.h, which is how the kernel sends info to devices. Then linker.ld, how the compiler links it all together. Then multiboot.h, which contains a part of the header file that core.c complained about having. Then string.c, where string functions from the standard C library are defined, and string.h, where they can be recognized by other files.
# Chapter VII: Makefile
The Makefile is further instructions for the compiler. It allows the convenience of typing `make` and watching it work. When including source files, make sure to note the syntax and style, and imitate it. 
