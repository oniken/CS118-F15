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

                 

            memset(buffer, 0, 256);  //reset memory

            //read client's message

            n = read(newsockfd,buffer,255);

            if (n < 0) error("ERROR reading from socket");

            printf("Here is the message:\n%s\n",buffer);

            //NEW STUFF

            //regex rgx("^(GET\s).+\..+");

            char filenamebuff[256];

            memset(filenamebuff, 0, 256);

            int len=0, i=0;

            for(i=5;i<256;i++){

              if(buffer[i]==' '&&buffer[i+1]=='H'&&buffer[i+2]=='T'&&buffer[i+3]=='T'&&buffer[i+4]=='P'&&buffer[i+5]=='/')

        {

                len=i-5;

    break;

        }

            }

            strncpy(filenamebuff, buffer+5, len);

      if(len==0)  

              filenamebuff[0]='\0';

      else filenamebuff[len+1]='\0';

      bool flg=(filenamebuff!=NULL&&filenamebuff[0]!='\0');

            if(flg) printf("Here is the file line:\n2%s2\n",filenamebuff);

      else printf("NO FILE ASKED FOR TRANSFER\n");

            FILE* file = fopen(filenamebuff, "r");

            int c;

            string body;

            int file_len;

            string statusHeader;

            ifstream f(filenamebuff, ios::in|ios::binary|ios::ate);

      if(flg) {

        /*if(file) {

          printf("ENTERED FILE FOUND\n");

          statusHeader="HTTP/1.1 200 OK\n";

          //TODO FILE TYPE HANDLE

          fseek(file, 0L, SEEK_END);

          file_len=ftell(file);

          fseek(file, 0L, SEEK_SET);

          i=0;

          c=getc(file);

          while(c!=EOF) {

            body+=c;

            c=getc(file);

          }

          fclose(file);





        }*/

        if(f.is_open())

      {

        printf("ENTERED FILE FOUND\n");

              statusHeader="HTTP/1.1 200 OK\n";

        streampos size=f.tellg();

        char* image=new char[size];

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

      //strcat(statusHeader, "Content-Type: text/plain");

            string response=statusHeader+"\n"+body;

      if(flg)

              cout<<"HTTP Response Message: \n"<< response<<endl;

            if(flg)

            {

              n = write(newsockfd, response.c_str(), response.size()); 

              if (n < 0) error("ERROR writing to socket");

            }

            //smatch match;

            //if(regex_search(str.begin(), str.end(), match, rgx)) printf("Here is the file line:\n%s\n", match[0]);

            //END NEW STUFF

            //reply to client

            else {

              n = write(newsockfd,"I got your message",18);

              if (n < 0) error("ERROR writing to socket");

            }

            close(newsockfd);//close connection 

      FD_CLR(newsockfd, &active_fd_set);

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

         

     memset(buffer, 0, 256);  //reset memory

      

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