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
#include "PacketStream.h"
using namespace std;
using std::ios; // Required for ifstream

void error(char *msg)
{
 perror(msg);
 exit(1);
}

int main(int argc, char *argv[])
{
 int sockfd, portno, pid;
 socklen_t clilen;
 struct sockaddr_in serv_addr, cli_addr;
 if (argc < 2) {
   fprintf(stderr,"ERROR, no port provided\n");
   exit(1);
 }
   sockfd = socket(AF_INET, SOCK_DGRAM, 0);  //create socket
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
   //fd_set active_fd_set;
   //FD_ZERO(&active_fd_set);
   //FD_SET(sockfd, &active_fd_set);
  // while (1) {
       // At this point sockfd is only socket in active_fd_set
    Packet* test;
    
    char* image;
    socklen_t client_address_size;
       //if(select(sockfd+1, &active_fd_set, NULL, NULL, NULL)<0) {exit(-1);} /*errors*/
      // if(FD_ISSET(sockfd, &active_fd_set)) //new connection request
      // {
        client_address_size = sizeof(cli_addr);
        //newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &client_address_size);
        //FD_SET(newsockfd, &active_fd_set);
      //}
      int s=0;
      // Process connection
      //if (FD_ISSET(newsockfd, &active_fd_set))
      //{
        int n;
        char buffer[18];
          memset(buffer, 0, 18);  //reset memory
          //read client's message
          n = recvfrom(sockfd,buffer,18, 0, (struct sockaddr*)&cli_addr, &client_address_size);
          if (n < 0) error("ERROR reading from socket");
          printf("Here is the message:\n%s\n",buffer);

          // Open file stream to send data over newsockfd
          ifstream f("empty.txt", ios::in|ios::binary|ios::ate);

          if(f.is_open())
          {
            streampos size=f.tellg();
            image=new char[size];

              // Stream operations
            f.seekg(0, ios::beg);
            f.read(image, size);
            printf ("%s\n", image);
            f.close();
            test=new Packet(image);
            s=size;
          }
          else {
            image="ENTERED FILE NOT FOUND\n";
            printf(image);
            test=new Packet(image);
            s=23;
          }
          char* response=(char *)test;
          n = sendto(sockfd, image, s, 0, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in)); 
          if (n < 0) error("ERROR writing to socket");
          //close(sockfd);//close connection 
          // Remove socket from active_fd_set once done serving
          //FD_CLR(newsockfd, &active_fd_set);
        //}
//          delete test;
//          delete image;
      //}
      //FD_ZERO(&active_fd_set);
      close(sockfd);
      return 0;
    }
