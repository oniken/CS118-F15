/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
*/
#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <strings.h>
#include <sys/wait.h> /* for the waitpid() system call */
#include <signal.h> /* signal name macros, and the kill() prototype */
#include <iostream>
#include <fstream>
#include <string.h>
#include <regex.h>
#include <sys/stat.h>
#include <errno.h>
using namespace std;
using std::ios;
void error(char *msg)
{
 perror(msg);
 exit(1);
}

int main(int argc, char *argv[])
{
 int sockfd, newsockfd, portno, pid;
 socklen_t clilen;
 struct sockaddr_in serv_addr, cli_addr;
 if (argc < 2) {
   fprintf(stderr,"ERROR, no port provided\n");
   exit(1);
 }
   sockfd = socket(AF_INET, SOCK_STREAM, 0);  //create socket
   if (sockfd < 0) 
    error("ERROR opening socket");
   memset((char *) &serv_addr, 0, sizeof(serv_addr)); //reset memory
   //fill in address info
   portno = atoi(argv[1]);
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   if (bind(sockfd, (struct sockaddr *) &serv_addr,
    sizeof(serv_addr)) < 0) 
    error("ERROR on binding");
   listen(sockfd,5);  //5 simultaneous connection at most
   fd_set active_fd_set;
   FD_ZERO(&active_fd_set);
   FD_SET(sockfd, &active_fd_set);
   while (1) {
       // At this point sockfd is only socket in active_fd_set
       if(select(sockfd+1, &active_fd_set, NULL, NULL, NULL)<0) {exit(-1);} /*errors*/
       if(FD_ISSET(sockfd, &active_fd_set)) //new connection request
       {
        socklen_t client_address_size = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &client_address_size);
        FD_SET(newsockfd, &active_fd_set);
      }
      // Process connection
      if (FD_ISSET(newsockfd, &active_fd_set))
      {
        int n;
        char buffer[256];
          memset(buffer, 0, 256);  //reset memory
          //read client's message
          n = read(newsockfd,buffer,255);
          if (n < 0) error("ERROR reading from socket");
          printf("Here is the message:\n%s\n",buffer);
          char filenamebuff[256];
          memset(filenamebuff, 0, 256);
          int len=0, i=0;
          // Manual regex to find file name
          for(i=5;i<256;i++){
            if(buffer[i]==' '&&buffer[i+1]=='H'&&buffer[i+2]=='T'&&buffer[i+3]=='T'&&buffer[i+4]=='P'&&buffer[i+5]=='/')
            {
              len=i-5;
              break;
            }
          }
          // Copy everything after "GET /"
          strncpy(filenamebuff, buffer+5, len);
          if(len==0)  
            filenamebuff[0]='\0';
          else filenamebuff[len+1]='\0';
          bool flg=(filenamebuff!=NULL&&filenamebuff[0]!='\0');
          if (!flg) printf("NO FILE ASKED FOR TRANSFER\n");
          int c;
          string body;
          int file_len;
          // Will contain reponse code (200/404) and Content-Type
          string statusHeader;
          // Open file stream to send data over newsockfd
          ifstream f(filenamebuff, ios::in|ios::binary|ios::ate);
          if(flg) {
            if(f.is_open())
            {
              statusHeader="HTTP/1.1 200 OK\n";
              // Detect content type from extension
              string path(filenamebuff);
              size_t dot = path.find_last_of(".");
              string ext = path.substr(dot + 1, path.size() - dot);

              statusHeader+="Content-Type: ";

              // Handle html, gif, and jpeg files. If any other extension assume plain text file
              if (ext == "html") {
                  statusHeader += "text/html";
              }
              else if (ext == "gif") {
                  statusHeader += "image/gif";
              }
              else if (ext == "jpeg") {
                  statusHeader += "image/jpeg";
              }
              else {
                  statusHeader += "text/plain";
              }
              statusHeader += "\n";
              streampos size=f.tellg();
              char* image=new char[size];
              
              // Stream operations
              f.seekg(0, ios::beg);
              f.read(image, size);
              f.close();
              i=0;
              while(i<size)
              {
                body+=image[i++];
              }
              delete image;
            }
            else {
              printf("ENTERED FILE NOT FOUND\n");
              statusHeader="HTTP/1.1 404 Not Found\n";
              body="<!DOCTYPE html><html><body><h1>404 - Page Not Found</h1></body></html>";
            }
          }
          // No file requested, just append default message
          else {
            statusHeader = "HTTP/1.1 200 OK\n";
            body = "I got your message";
          }
          string response=statusHeader+"\n"+body;
          cout<<"HTTP Response Message: \n"<< response<<endl;
          n = write(newsockfd, response.c_str(), response.size()); 
          if (n < 0) error("ERROR writing to socket");
          close(newsockfd);//close connection 
          // Remove socket from active_fd_set once done serving
          FD_CLR(newsockfd, &active_fd_set);
        }
      }
      FD_ZERO(&active_fd_set);
      close(sockfd);
      return 0;
    }
