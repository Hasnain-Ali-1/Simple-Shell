I USED /proc FOR PROC COMMANDS

This project is about creating a C program that functions as a simple shell that takes 
the user input and attempts to follow it as a shell command. It should be able to 
dispay the 10 most recent commands eneterd by the user, it should be able to read 
proc files, it should be able to exit if the user enters nothing or a single number
after exit, and it should be able to execute commands if they are valid, like "ls".

My thought process was to create a while loop that would continue to seek user 
input until a valid exit command was entered. Regardless of if the input is valid, 
it gets added to a hidden file, so a file in the user's home directory will contain 
the user's commands. Once the input was given, I had specific functions handle 
certain scnearios, like if /proc, history, or exit was the first command entered. If 
it was not one of these three commands, then I would attempt to use the execvp 
function on the input, and a command might occur depending on if the user input was a 
valid command.

Ran into the typical issues of not properly freeing memory and having memory leaks. I
also accidentally freed already freed memory a couple of times. The only thing to 
truly get me stuck was parsing the user input, because you couldn't just split by 
space. The space may be in a quote, so you had to account for this. To get around 
this, I used strtok to split the string using space, tab, newline, and carriage return 
characters as delimiters. Then when it processs each token in a while loop, I check if 
the token starts with a quote. If it does, the code looks for the closing quote. As 
it looks for the closing quote, the tokens within the quotes are concatenated with 
a space between them (if there were multiple tokens within the quotes). Once the 
closing quote is found, the combined token gets added to an array and we move onto the
next token, if there is one. If the first character of a token does not have a quote, 
and we are not curently looking for a closing quote, then the token gets added to the 
array just as it is. This took me the most amount of time to figure out. Also, for 
some proc files, it would not print out the whole thing or the format would be 
unreadable. To get around this I replaced the '\0' character with '\n' whenever it was 
encountered when reading in the lines.

Sample Output:
$ ls
CMakeLists.txt Makefile README.md src
$ /bin/ls -la
total 60
drwxr-xr-x 6 lj lj 4096 Feb 13 17:34 .
drwxr-xr-x 3 lj lj 4096 Feb 13 17:13 ..
-rw-r--r-- 1 lj lj 15314 Feb 13 17:13 .clang-format
-rw-r--r-- 1 lj lj 958 Feb 13 17:30 CMakeLists.txt
-rw-r--r-- 1 lj lj 293 Feb 13 17:13 .editorconfig
drwxr-xr-x 8 lj lj 4096 Feb 13 17:35 .git
-rw-r--r-- 1 lj lj 19 Feb 13 17:13 .gitignore
drwxr-xr-x 3 lj lj 4096 Feb 13 17:13 .idea
-rw-r--r-- 1 lj lj 1236 Feb 13 17:31 Makefile
-rw-r--r-- 1 lj lj 2829 Feb 13 17:13 README.md
drwxr-xr-x 2 lj lj 4096 Feb 13 17:34 src
drwxr-xr-x 2 lj lj 4096 Feb 13 17:13 .vscode
$ ps -el
F S UID PID PPID C PRI NI ADDR SZ WCHAN TTY TIME CMD
4 S 0 1 0 0 80 0 - 222 - ? 00:00:00 init
5 S 0 99 1 0 80 0 - 222 - ? 00:00:00 init
1 S 0 100 99 0 80 0 - 222 - ? 00:00:00 init
4 S 1000 101 100 0 80 0 - 190697 futex_ pts/0 00:00:02 docker
1 Z 0 102 99 0 80 0 - 0 - ? 00:00:00 init
<defunct>
1 S 0 113 99 0 80 0 - 222 - ? 00:00:00 init
4 S 0 114 113 0 80 0 - 329161 - pts/1 00:00:00 dockerdesktop-
5 S 0 140 1 0 80 0 - 222 - ? 00:00:00 init
1 S 0 141 140 0 80 0 - 222 - ? 00:00:01 init
4 S 1000 142 141 0 80 0 - 1780 do_wai pts/2 00:00:00 bash
0 S 1000 299 296 0 80 0 - 2753 - pts/3 00:00:00 top
0 R 1000 350 142 0 80 0 - 2636 - pts/2 00:00:00 ps
 
$ proc 299/environ
SHELL=/bin/bashWSL_DISTRO_NAME=DebianWT_SESSION=7db2b336-f609-4f16-b8d3-
97cf50e3b759
NAME=akagiPWD=/mnt/c/Users/Lawrence
SebaldLOGNAME=ljHOME=/home/ljLANG=en_US.UTF-8TER
TERM=xterm-256colorUSER=ljPATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sb
in:/bin:/usr/games:/usr/local/games=/usr/bin/top
$ echo \x48\151\x20\157\165\164\040\x74\x68\x65\x72\x65\041
Hi out there!
$ echo Goodbye, \'World\'\a
Goodbye, 'World'
$ exit 0

