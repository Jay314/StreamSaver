//============================================================================
// Name        : send_UDP.cpp
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
   int sock, n;
   unsigned int length;

   struct sockaddr_in server;
   struct hostent *hp;
   char buffer[256];

   if (argc != 4)
   {
     std::cout << "Usage: ./send_UDP server port message" << std::endl;
     exit(1);
   }

   sock= socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0)
   {
     error("socket");
   }

   server.sin_family = AF_INET;
   hp = gethostbyname(argv[1]);
   if (hp==0)
   {
     error("Unknown host");
   }

   bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
   server.sin_port = htons(atoi(argv[2]));
   length=sizeof(struct sockaddr_in);
   bzero(buffer,256);

   // sending packet
   n=sendto(sock,argv[3], strlen(argv[3]),0,(const struct sockaddr *)&server,length);
   if (n < 0)
   {
     error("Sendto");
   }

   close(sock);

   return 0;
}
