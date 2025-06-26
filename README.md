# Cshell
---

## Short Description:
    Ths is an implementation of basic shell.This covers the idea of how a shell covers.NOTE that this shell does nit handles complex tasks. This project is just meant to implement a shell which covers the lifetime of a shell and explains how does a shell functions.

---

# Basic Lifetime of a Shell:

Before moving ahead let's look at how does a lifetime of a shell goes:
 * Initialize: In this step, the shell will load the configuration file. This ill change the aspects of the shell.
 * Interpret: Then the shell will read the commands from stdin(interactive or a file) and execute them.
 * Terminate: After the commands are executed the shell will ru any shutdown commands or free command and terminate the shell.

These steps are so general that they can be implemented in any program but for now we will look with just the context of shell.
Our shell is going to be so simple that there won'tbe a configuration file or any shutdown command.

---

# Basic loop:

So, Let's talk about how the loop of the shell works:
  * __Read:__ The shell takes the command from the standard input.
  * __Parse:__ The shell seprates the programm and the commands.
  * __Execute:__ Execute the parsed commands.

In this programm I am using a do-while loop as it runs at least once then check the condition. Note that in the program the check for the loop is set on the status which is a variable returned by the lsh_execute() loop.

---

# Reading a line:

Reading a line from stdin sounds easy butin c it can be a hassle. The pont is that you don't know beforehand how much data user would iput so you cannot just allocate a block of memory and hope the user doesn't exceed the limit. So it is better to allocate a block of memory and when the user does exceed then reallocating the memory.

Note: I have used a (apparantly infinite) while loop, where we take a char as an input and store it in an integer(the variable c) to make a check on c if it was char we couldn't check thatfor EOF or '\n'. 

---

# Parsing the line:

Now since the shell has red the input now t needs to seprate the different arguments. Here, for simplifications, say we wont allow quoting or backslash escaping. So the command echo "this message" would not call echo with a single argument butth two arguments "this and message". Now, that out of the way all that is remaining is to 'tokenize' the strings using whitespace delimiters.

If the lsh_split_line() code looks similar to the lsh_read_line() code then that is because it is, we go through the same logic but the difference is just that we use the strtok function here to tokenize.

---

# How shell starts processes:

There are only two way processes are started in a unix like system. First way(not exactly a way),is to be  INIT. when a unix like system starts up it first starts up the kernel which first runs init and it runs till the system turns off or the kermnel is closed.So there is no way any other program can be INIT. So this leaves us with only one way, and that is fork(). This means a unix shell to run a program first makes a duplicate, it calls the original programm 'parent' and the duplicated 'child', fork() returns 0 to the child and iit returns the parent thhe Process ID(PID) number of its child.
This might sound like a problem. Typically, when you want to run a new process, you don’t just want another copy of the same program – you want to run a different program. That’s what the exec() system call is all about. It replaces the current running program with an entirely new one. This means that when you call exec, the operating system stops your process, loads up the new program, and starts that one in its place. A process never returns from an exec() call (unless there’s an error).

In the child process, we want to run the command given by the user. So, we use one of the many variants of the exec system call, execvp. The different variants of exec do slightly different things. Some take a variable number of string arguments. Others take a list of strings. Still others let you specify the environment that the process runs with. This particular variant expects a program name and an array (also called a vector, hence the ‘v’) of string arguments (the first one has to be the program name). The ‘p’ means that instead of providing the full file path of the program to run, we’re going to give its name, and let the operating system search for the program in the path.

If the exec command returns -1 (or actually, if it returns at all), we know there was an error. So, we use perror to print the system’s error message, along with our program name, so users know where the error came from. Then, we exit so that the shell can keep running.

The second condition (pid < 0) checks whether fork() had an error. If so, we print it and keep going – there’s no handling that error beyond telling the user and letting them decide if they need to quit.

The third condition means that fork() executed successfully. The parent process will land here. We know that the child is going to execute the process, so the parent needs to wait for the command to finish running. We use waitpid() to wait for the process’s state to change. Unfortunately, waitpid() has a lot of options (like exec()). Processes can change state in lots of ways, and not all of them mean that the process has ended. A process can either exit (normally, or with an error code), or it can be killed by a signal. So, we use the macros provided with waitpid() to wait until either the processes are exited or killed. Then, the function finally returns a 1, as a signal to the calling function that we should prompt for input again.

---

# Shell Builtins:

The reason is actually pretty simple. If you want to change directory, you need to use the function chdir(). The thing is, the current directory is a property of a process. So, if you wrote a program called cd that changed directory, it would just change its own current directory, and then terminate. Its parent process’s current directory would be unchanged. Instead, the shell process itself needs to execute chdir(), so that its own current directory is updated. Then, when it launches child processes, they will inherit that directory too.

Similarly, if there was a program named exit, it wouldn’t be able to exit the shell that called it. That command also needs to be built into the shell. Also, most shells are configured by running configuration scripts, like ~/.bashrc. Those scripts use commands that change the operation of the shell. These commands could only change the shell’s operation if they were implemented within the shell process itself.

---

# Putting Together builtins and processes:

Now all is left to implement lsh_execute().

---

# Wrap Up:

Now, Call all the header files, and if everything is fine it will run by a simple command like ```gcc Cshell.c -o cshell ``` .

---
