
## How to Run 

1. Compile the C++ code using `g++`:

```bash
g++ your_shell.cpp -o my_shell
```

2. Run your shell executable:

```bash
./my_shell
```

Now you're inside your own mini Unix shell!  
Type commands like:

```bash
ls -l
pwd
echo "Hello from my shell!"
```


#  Simple Unix Shell Emulator — Written in C++ (With Love & Syscalls)

> "Why write a shell?"  
> Because real developers eventually get tired of just using one.

---

## Overview 

Welcome to my humble attempt at building a mini Unix Shell from scratch using C++ — diving deep into the world of processes, system calls, file handling, and of course... a little bit of pain.

This project aims to mimic core features of a Unix Shell — the things we often take for granted when typing commands in our terminal.

Here’s what I tackled head-on:

- Command Execution (your classic `ls`, `pwd`, `echo`)
- Command Searching in `PATH`
- File Redirection (both `>` and `>>`)
- Standard Error Redirection (`2>`, `2>>`)
- Handling Empty Files (cleaning up after ourselves)
- Quoting in Commands (single, double — no discrimination)
- Debugging Outputs (because I believe in talking to myself while coding)

---

## Core Concepts & Features 

---

### Command Execution 

The shell reads user input, searches for the command in `PATH`, and executes it like a responsible citizen.

Example:

```bash
ls -l
pwd
echo "Unix is love"
```

Behind the scenes?  
We used good old:

```cpp
fork();  // to create a child process
execvp(); // to execute the command
wait();  // to wait for the child process to finish
```

Simple. Clean. Reliable. Just like your friendly neighborhood shell.

---

### Command Search Logic 🔍

Ever wondered how your terminal magically finds where `ls` lives?  

We didn’t hardcode anything. We split the `PATH` environment variable, loop through the directories, and check if the command exists and is executable.

```cpp
if (access(fullpath.c_str(), X_OK) == 0)
```

Boom.  
Found it? Execute it.  
Didn’t find it? Cry about it — or print an error.

---

### File Redirection: The Real Fun 🎯

Handling redirection felt like unlocking a new level in Shell-building.

| Symbol | Purpose                           | Implementation Details                  |
|--------|-----------------------------------|------------------------------------------|
| `>`    | Truncate file & write output      | `open()` with `O_WRONLY | O_CREAT | O_TRUNC` |
| `>>`   | Append output to file             | `open()` with `O_WRONLY | O_CREAT | O_APPEND` |
| `2>`   | Redirect Standard Error to file  | Same as `>` but for `stderr`             |
| `2>>`  | Append Standard Error to file    | Same as `>>` but for `stderr`            |

---

### Empty Files Because Cleanliness Matters 

Sometimes before writing new content, we need to get rid of the old junk.

```cpp
ofstream file(filename, ios::trunc);
file.close();
```

Old data gone. New data ready.

Minimalist vibes only.

---

## Quoting: Single vs Double Quotes Drama 

I learned the hard way that quoting in shell commands isn’t just for aesthetics.

- Single Quotes `' '` — Treat *everything* inside literally.
- Double Quotes `" "` — Allow expansion (variables, escaping).

Example:

```bash
echo 'This is literal text'
echo "Current directory is: $PWD"
```

Don’t worry — our shell respects both of them like a true diplomat.

---

## Debugging & Tracing 🐞

Of course, I had some lovely debug print statements sprinkled everywhere:

```cpp
cout << "readfrom ----> " << readfrom << endl;
cout << "writeto  ----> " << writeto << endl;
```

Helps me sleep better at night.

---

## System Calls Used 

| System Call | Purpose |
|-------------|---------|
| `fork()`    | Create child process |
| `execvp()`  | Execute command |
| `wait()`    | Wait for child |
| `access()`  | Check file existence & permissions |
| `open()`    | Open file for writing/appending |
| `chdir()`   | Change directory |
| `getcwd()`  | Get current working directory |

---

## Example Commands I Proudly Support

```bash
ls -l
echo "Hello World"
cat file.txt > output.txt
ls -1 nonexistent 2>> error.log
pwd >> current_dir.txt
cd ..
```

---
