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
#include <sys/wait.h>	/* for the waitpid() system call */
#include <signal.h>	/* signal name macros, and the kill() prototype */


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
     sockfd = socket(AF_INET, SOCK_STREAM, 0);	//create socket
     if (sockfd < 0) 
        error("ERROR opening socket");
     memset((char *) &serv_addr, 0, sizeof(serv_addr));	//reset memory
     //fill in address info
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     
     listen(sockfd,5);	//5 simultaneous connection at most
     fd_set active_fd_set;
     FD_ZERO(&active_fd_set);
     FD_SET(sockfd, &active_fd_set);

     while (1) {
         if(select(sockfd+1, &active_fd_set, NULL, NULL, NULL)<0) {exit(-1);} /*errors*/

         if(FD_ISSET(sockfd, &active_fd_set)) //new connection request
              {
                  socklen_t client_address_size = sizeof(cli_addr);
                  newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &client_address_size);
                  FD_SET(newsockfd, &active_fd_set);
              }

         if (FD_ISSET(newsockfd, &active_fd_set))
         {
             int n;
             char buffer[256];
            		 
             memset(buffer, 0, 256);	//reset memory
            
             //read client's message
             n = read(newsockfd,buffer,255);
             if (n < 0) error("ERROR reading from socket");
             printf("Here is the message:\n%s\n",buffer);
             
             //reply to client
             n = write(newsockfd,"I got your message",18);
             if (n < 0) error("ERROR writing to socket");
             close(newsockfd);//close connection 
         }
     }
     FD_ZERO(&active_fd_set);
     close(sockfd);
     
     /*
     //accept connections
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
         
     if (newsockfd < 0) 
       error("ERROR on accept");
         
     int n;
   	 char buffer[256];
   			 
   	 memset(buffer, 0, 256);	//reset memory
      
 		 //read client's message
   	 n = read(newsockfd,buffer,255);
   	 if (n < 0) error("ERROR reading from socket");
   	 printf("Here is the message:\n%s\n",buffer);
   	 
   	 //reply to client
   	 n = write(newsockfd,"I got your message",18);
   	 if (n < 0) error("ERROR writing to socket");
         
     
     close(newsockfd);//close connection 
     close(sockfd);
     */    
     return 0; 
}

