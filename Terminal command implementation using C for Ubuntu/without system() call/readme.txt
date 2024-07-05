    
This terminal can execute ls,ps,cp,mv and grep with few options:

1)ls:
	ls [OPTION]
	This works as original command ls.
	This can be executed with following options:
	1) -l 2) -s 3) -m 4) -i 5) -a
2)ps:
	ps [OPTION]
	This works as original command ps.
	This can be executed with following options:
	1) -a 2) -f 3) -e 4) -d
3)cp:
	cp [OPTION] [SOURCE] [TARGET]
	This works as original command cp.
	This can be executed with following options:
	1) -l 2) -n 3) -u 4) -v
4)mv:
	mv [OPTION] [SOURCE] [TARGET]
	This works as original command mv.
	This can be executed with following options:
	1) -f 2) -n 3) -u 4) -v
5)grep:
	grep [OPTION] PATTERN [FILE] 
	This works as original command grep.
	This can be executed with following options:
	1) -v 2) -n 3) -o 4) -m

	
Other commands are getting executed through system().

The requirements to execute this terminal are :
-> A Linux Ubuntu machine.
-> GCC should be installed in the machine:
	If GCC is not installed in your machine, do the following:
	Open the terminal on your machine and run the following commands:
		-> "sudo apt update"
		-> "sudo apt install build-essential"
		OR
		-> "sudo apt install gcc"
	To check if install then run the following command:
	        -> "gcc --version"
-> libreadline :
Install libreadline in your machine:
For that run the following command on your terminal:
		-> "sudo apt-get install libreadline8 libreadline-dev"
Now you are ready to run this terminal!

Steps to execute:

-> Change the directory to the directory where the files are located.
-> Now run the following commands: 
    -> "make" (To compile the executable files.)
    -> "./myshell2" (To execute the terminal.)

Now you can run whichever command you want to run from the above mentioned commands.



