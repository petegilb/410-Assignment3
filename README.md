### 410 Final Assignment
Members: Peter Gilbert and Alex Thomas

# Dependencies

***GNUPLOT***
```bash
sudo apt-get update -y
sudo apt-get install -y gnuplot
```

***APUE Textbook Source Code***
apue.h
error.c
pathalloc.c

***Inspiration From:***
- Stackoverflow

# Usage

The below command will make all object files, and executables:

```bash
make
```
The below command will delete all generated object files and executable files:

```bash
make clean
```

Besides those two commands, you may also type in "make" followed by the name of the file
you would like to make. Example down below:

```bash
make webserv
```

# Passing Arguments On The Webserver

We are using the convention "url?arg1&arg2&arg3" as our method of passing arguments on the URL

</br>

Example:
```
localhost:8080/my_histogram?testFolder
```

# Shortcomings

1. I wasn't able to get the multithreading solution working perfectly. I could make MxN threads but didn't know how to swap context in order for the program to work
