# System Wide FD Tables

## Description

- This program displays the tables used by the OS to keep track of open files, assignation of File Descriptors (FD) and processess.
- The FD Tables include process FD table, system-wide FD table, Vnodes FD table, composed table for all the currently processes belong to the current user executing the program.
- Allows the user to enter a threshold value to indicate the display of the selected processes with the number of file descriptors greater than the threshold in the format PID (FD) below any FD tables indicated by the user.
- Allows the user to enter a specific process id number (PID), and displays tables including the target process.

## How did I solve the problem?

Firstly, I went through the '/proc' directory system to see what was stored in there. I realized that I coudl find the PID, FD & location from accessing different folders in this directory, however would have to use the 'sys/stat.h' library to find out about inode data. Then I created functions, to deal with all the positional arguments, as well as created a function to store all the current processes into a linked list. Using this list, I can then print whatever is required based on the command line arguments usign if statements & conditions. Finally, I also created functions to print out the composite tables into text and binary files using fprintf & fwrite.

As for the header file & makefile, after finishing the initial source code, I moved the function, struct definitions & library inclusions into the header file, and created a makefile with cleaning and compiling features. I simplified this makefile with special characters later on as well.

## Functions Overview

- newProcess: This functions creates an instance of the struct process, and assigns attribute values
- insert: This function inserts a process instance into the end of a linked list starting at 'head'
- AssessArgument: This function assesses one command line argument and prints out whatever is required based on that argument.
- StoreProcesses: This function iterates through the the '/proc' directory system as well as uses the 'sys/stat.h' library to create a linked list that stores instances of the struct process, which stores values of the PID, FD, Inode, and location. The head of this linked list is then returned.
- checkForPID: This function checks whether any of the arguments include a PID, as this would mean all the other remaining arguments would have to be based on ths PID only
- AssessArguments: This function loops through all the command line arguments, evaluate whath they require, and then prints out information as needed
- ThresholdPresent: This function checks if whether '--threshold=X' is one of the command line arguments inputted in
- digits_only: This checks if the string passed to this functions includes only numbers
- PrintPerProcessFDTable: Prints out the PID & FD of all current processes (or of a particular PID if provided)
- PrintSystemWideFDTable: Prints out the PID, FD, and location of all current processes (or of a particular PID if provided)
- PrintVnodesTable: Prints out the FD & Inode of all current processes (or of a particular PID if provided)
- PrintCompositeTable: Prints out the PID, FD, Inode, and location of all current processes (or of a particular PID if provided)
- PrintThresholdTable: Prints out the PID, FD, Inode, and location of all current processes (or of a particular PID if provided) and then prints out if any process has more FDs than the required threshold (**if particular PID provided, then prints out composite table for that PID but checks if all PIDs' FDs go past the threshold**)
- CreateCompositeBin: Creates a binary file and prints out a composite table for current processes onto that binary file
- CreateCompositeTxt: Creates a text file and prints out a composite table for current processes onto that text file

(**Note: For both Create functions, print out only particular PID info into text/binary file if PID given**) 

## How to run my program?

Download Makefile, showFDtables.c, and showFDtables.h files, storing them in the same directory. Then just go to that directory through the terminal using 'cd' and then type in 'make' to create executable file. After that, use './showFDtables' to print just the composite table (**default behavior**), or use that plus any other command line argument/arguments listed below:

--per-process
--systemWide
--Vnodes
--composite
--threshold=X
--output_TXT
--output_binary

Any PID as a positional argument

