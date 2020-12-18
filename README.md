### 410 Final Assignment
Members: Peter Gilbert and Alex Thomas

# Dependencies

***GNUPLOT***
```bash
sudo apt-get update -y
sudo apt-get install -y gnuplot
```

***APUE Textbook Source Code*** </br>
apue.h </br>
error.c </br>
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
localhost:8080/my_histogram.cgi?testFolder
```

# Web Server Implementation
directories: ls -l is run for whatever directory is indicated in the url </br>
cgi files: cgi is run based on the #!/bin/... on the first line and output is redirected to the client </br>
html files: displays html files based on their content length and writes to the client </br>
image files: displays image files and writes to the client socket </br>
plain text: plain text will just be written to the client in the same way </br>

to run the web server do: </br>
```
./webserv
```

# Running Dynamic GNUPLOT
Example: </br> 
```
localhost:8080/my_histogram.cgi?testFolder
```

# Shortcomings

1. Additional Features not yet implemented </br>
2. Cgi files follow our file convention -> ?value&value rather than ?name=val&name=val ... </br>
3. Fairly simple Arduino sketch
