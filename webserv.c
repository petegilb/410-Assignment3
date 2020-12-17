#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

//respond to http GET
//text/plain
//text/html
//image/gif
//image/jpeg
//client request:
//GET filename HTTP/version
//optional arguments
//a blank line

//server reply:
//HTTP/version status-code status-message
//additional info
//blank line
//content
//static vs dynamic content

//key functions: socket, bind, listen, accept, connect
//then open a pair of sockets and read/write corresponding socket descriptors
//SOCK_STREAM SOCK_DGRAM
//man 7 socket

//open file on the server get a fd
//establish connect with client
//duplicate output from file onto socket descriptor
//that will let you write information to the client

void writeToSocket(int new_sd, char* filePath, char* firstLine);

//server code provided by the class
void servConn (int port) {

  int sd, new_sd;
  struct sockaddr_in name, cli_name;
  int sock_opt_val = 1;
  int cli_len;
  char data[1000];        /* Our receive data buffer. */

  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("(servConn): socket() error");
    exit (-1);
  }

  if (setsockopt (sd, SOL_SOCKET, SO_REUSEADDR, (char *) &sock_opt_val,
          sizeof(sock_opt_val)) < 0) {
    perror ("(servConn): Failed to set SO_REUSEADDR on INET socket");
    exit (-1);
  }

  name.sin_family = AF_INET;
  name.sin_port = htons (port);
  name.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind (sd, (struct sockaddr *)&name, sizeof(name)) < 0) {
    perror ("(servConn): bind() error");
    exit (-1);
  }

  listen (sd, 5);

  for (;;) {
      cli_len = sizeof (cli_name);
      new_sd = accept (sd, (struct sockaddr *) &cli_name, (socklen_t*) &cli_len);
      printf ("Assigning new socket descriptor:  %d\n", new_sd);

      if (new_sd < 0) {
          perror ("(servConn): accept() error");
          exit (-1);
      }

      if (fork () == 0) {    /* Child process. */
          close (sd);
          //read the get
          read (new_sd, &data, 1000);
          printf ("Received string = %s\n", data);
          char *endofLine = strchr(data, '\n');
          char firstLine[100];
          //get firstLine so we can check for the ending
          memcpy(firstLine, data, endofLine-data);
          printf("%s\n", firstLine);

          //finding the host ip so we can find the file path
          /*char dataCpy[1000];
          //copy so we don't change the data string
          memcpy(dataCpy, data, strlen(data));
          char *hostLine;
          hostLine = strtok(dataCpy, "'\n'");
          hostLine = strtok(NULL, "'\n'");
          char* hostIP;
          hostIP = strtok(hostLine, " ");
          hostIP = strtok(NULL, " ");*/
          char firstLinecpy[100];
          memcpy(firstLinecpy, firstLine, strlen(firstLine));
          char* filePath;
          filePath = strtok(firstLinecpy, " ");
          filePath = strtok(NULL, " ");
          //append . to the front
          char finalPath[100];
          finalPath[0] ='\0';
          strcat(finalPath, ".");
          strcat(finalPath, filePath);
          printf("file path is: %s\n", finalPath);
          //first we will check if the file exists
          if(access(finalPath, F_OK|R_OK) == 0){
              //printf("file exists\n");
              writeToSocket(new_sd, finalPath, firstLine);
          }
          else{ //404 error not found
              char* err404 = "HTTP/1.1 404 Not Found\nContent-Type: text/plain\nContent-Length: 10\n\nError 404!\n";
              if(write(new_sd, err404, strlen(err404)) < 0){
                  perror("error writing to socket\n");
              }
          }

          //sample response to the get call
          /*char* sample = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 14\n\nHello browser!\n";
          printf("writing to socket %d\n", new_sd);
          if(write(new_sd, sample, strlen(sample)) < 0){
              perror("error writing to socket\n");
          }*/
          exit (0);
      }
  }
}

void writeToSocket(int new_sd, char* filePath, char* firstLine){
    if(strstr(firstLine, "favicon") != NULL){
        close(new_sd);
        exit(0);
    }
    if(strstr(firstLine, ".html") != NULL){
        printf("HTML REQUEST\n");
        char* htmlBase = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
        char send_buffer[1000];
        FILE *toSend;
        if(!(toSend = fopen("./index.html", "rb"))){
            perror("error opening file\n");
        }
        //while we haven't gotten an eof
        while(!feof(toSend)){
            int numread = fread(send_buffer, sizeof(unsigned char), 1000, toSend);
            if(numread < 1){
                perror("error reading file\n");
            }
        }
        //printf("%s\n", send_buffer);
        //get file descriptor
        int fd = fileno(toSend);
        struct stat st;
        fstat(fd, &st);
        int size = st.st_size;

        //this val is the size of the string after making size into a string
        //solution sourced from: https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
        int sizeofSizeStr = (int)((ceil(log10(size))+1)*sizeof(char));
        char sizeStr[sizeofSizeStr+2];
        sprintf(sizeStr, "%d", size);
        strcat(sizeStr, "\n\n");
        sizeofSizeStr += 2; //maybe 4
        //printf("size of the small char is %ld\n", strlen(sizeStr));

        //we have found the size of the file and the string before it so we know what to allocate
        int finalSize = strlen(htmlBase) + strlen(sizeStr) + size + 1;
        char *finalBuffer = (char*) malloc(strlen(htmlBase) + strlen(sizeStr) + size + 1);
        memcpy(finalBuffer, htmlBase, strlen(htmlBase));
        memcpy(finalBuffer+strlen(htmlBase), sizeStr, strlen(sizeStr));
        memcpy(finalBuffer+strlen(htmlBase)+strlen(sizeStr), send_buffer, size);
        //printf("current val: %s\n", finalBuffer);
        if(write(new_sd, finalBuffer, finalSize) < 0){
            perror("error writing to socket\n");
        }
        //printf("final size of buffer: %d\n", finalSize);

    }
    else if(strstr(firstLine, ".cgi") != NULL){

    }
    else{ //501 Not Implemented
        char* err501 = "HTTP/1.1 501 Not Implemented\nContent-Type: text/plain\nContent-Length: 10\n\nError 501!\n";
        if(write(new_sd, err501, strlen(err501)) < 0){
            perror("error writing to socket\n");
        }
    }
}

int main(int argc, char** argv){
    servConn (8080);
    return 0;
}
