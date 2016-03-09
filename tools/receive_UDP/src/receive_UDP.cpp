//============================================================================
// Name        : receive_udp.cpp
// Author      : Juergen Pfadt
// Version     :
// Copyright   : 
// Description :
//============================================================================

#include <iostream>
using namespace std;

/* UDP client in the internet domain */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
   int sock, length, n;
   socklen_t fromlen;
   struct sockaddr_in server;
   struct sockaddr_in from;
   char buf[1024];

    if (argc != 3)
    {
      std::cout << "Usage: ./receive_UDP server port" << std::endl;
      exit(1);
    }

   sock=socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) error("Opening socket");
   length = sizeof(server);
   bzero(&server,length);
   server.sin_family=AF_INET;
   server.sin_addr.s_addr=INADDR_ANY;

   server.sin_port = htons(atoi(argv[2]));
   if (bind(sock,(struct sockaddr *)&server,length)<0)
   {
     error("binding");
   }
   fromlen = sizeof(struct sockaddr_in);
   while (true)
   {
     n = recvfrom(sock,buf,1024,0,(struct sockaddr *)&from,&fromlen);

     if (n < 0)
     {
       error("recvfrom");
     }
     // cut the received string
     buf[n]='\0';
     std::cout << "answer[" << std::string(buf).length() <<"]: " << std::string(buf) << std::endl;
   }

   return 0;
}
