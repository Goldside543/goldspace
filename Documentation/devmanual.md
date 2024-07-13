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
