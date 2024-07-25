# Welcome! This script exits the goldspace directory, deletes it, clones it back, and goes into it, resetting your environment to the stable branch!

#!/usr/bin/perl
use strict;
use warnings;

# Change directory to the parent directory
chdir '..' or die "Cannot change directory to parent: $!";

# Remove the 'goldspace' directory recursively
system('rm -r goldspace') == 0 or die "Failed to remove 'goldspace': $!";

# Clone the GitHub repository
system('git clone https://github.com/Goldside543/goldspace.git') == 0 or die "Failed to clone repository: $!";

# Change directory to the 'goldspace' directory
chdir 'goldspace' or die "Cannot change directory to 'goldspace': $!";
