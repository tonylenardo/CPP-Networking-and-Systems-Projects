#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <thread>
#include <fstream>

#include "Container.h"
#include "Socket.h"

#define PORTCLIENT 8080
#define BUFSIZE 512
#define BROADCAST_PORT 8000
#define BROADCAST_IP "127.0.0.1" //172.16.123.31

char buffer[BUFSIZE];

void handleRequest(VSocket* socket, const std::string &request);

//void sendBook(const std::string &book_number, Container* container) {
//  container->sendBook(book_number.c_str());
//}

void sendBroadcast(const char* bookName, const char* serverIP, int port, const char* broadcastMessage) {

  int UDPSockFd = socket(AF_INET, SOCK_DGRAM, 0);
  if (UDPSockFd ==  -1) {
    perror("ERROR: Coult not create socket");
    exit(EXIT_FAILURE);
  }

  int broadcastEnable = 1;
  if (setsockopt(UDPSockFd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable
    , sizeof(broadcastEnable)) == -1) {
    perror("setsockopt (SO_BROADCAST) failed");
    close(UDPSockFd);
    exit(EXIT_FAILURE);
  }

  int disableLoopback = 0;
  if (setsockopt(UDPSockFd, IPPROTO_IP, IP_MULTICAST_LOOP, &disableLoopback
    , sizeof(disableLoopback)) == -1) {
    perror("setsockopt (IP_MULTICAST_LOOP) failed");
    close(UDPSockFd);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in broadcastAddr;
  memset(&broadcastAddr, 0, sizeof(broadcastAddr));
  broadcastAddr.sin_family = AF_INET;
  broadcastAddr.sin_port = htons(BROADCAST_PORT);
  broadcastAddr.sin_addr.s_addr = inet_addr(BROADCAST_IP);

  if (sendto(UDPSockFd, broadcastMessage, strlen(broadcastMessage), 0
    , (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr)) == -1) {
    perror("sendto failed");
    close(UDPSockFd);
    exit(EXIT_FAILURE);
  }

  std::cout /*<< "Broadcast enviado desde Servidor" << std::endl  
    << "--------------------------------" << 
    std::endl */<< broadcastMessage << std::endl;

  close(UDPSockFd);
}

void receiveBroadcast() {
  int UDPSockFd;
  struct sockaddr_in recvAddr;


  if ((UDPSockFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  int enableReuse = 1;
  if (setsockopt(UDPSockFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse
    , sizeof(enableReuse)) == -1) {
    perror("setsockopt (SO_REUSEADDR) failed");
    close(UDPSockFd);
    exit(EXIT_FAILURE);
  }

  int disableLoopback = 0;
  if (setsockopt(UDPSockFd, IPPROTO_IP, IP_MULTICAST_LOOP, &disableLoopback
    , sizeof(disableLoopback)) == -1) {
    perror("setsockopt (IP_MULTICAST_LOOP) failed");
    close(UDPSockFd);
    exit(EXIT_FAILURE);
  }

  memset(&recvAddr, 0, sizeof(recvAddr));
  recvAddr.sin_family = AF_INET;
  recvAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  
  recvAddr.sin_port = htons(BROADCAST_PORT);


  if (bind(UDPSockFd, (struct sockaddr*)&recvAddr, sizeof(recvAddr)) == -1) {
    perror("bind failed");
    close(UDPSockFd);
    exit(EXIT_FAILURE);
  }

  std::cout << "Esperando mensajes de broadcast en 127.0.0.1..." << std::endl;

  while (true) {
    memset(buffer, 0, BUFSIZE);
    int recvLen = recvfrom(UDPSockFd, buffer, BUFSIZE, 0, NULL, NULL);
    if (recvLen < 0) {
      perror("recvfrom failed");
      close(UDPSockFd);
      exit(EXIT_FAILURE);
    }
    buffer[recvLen] = '\0';

    VSocket * s1, * s2;
    s1 = new Socket( 's' );
    s1->Bind( 8080 );
    s1->Listen( 5 );

    for( ; ; ) {

      s2 = s1->Accept();

      handleRequest(s2, std::string(buffer));

      s2->Close();			// Close socket s2 in parent, then go wait for a new conection
    }
  }

  close(UDPSockFd);
}

void handleRequest(VSocket* socket, const std::string &request) {
  std::istringstream requestStream(request);
  std::string method, uri, version;
  requestStream >> method >> uri >> version;

  if (method == "GET") {

    if (uri.find("/") == 0) {
      
      std::string book_number = uri.substr(1);
      std::cout << "HTTP/1.0 200 OK\r\n\r\n";

      Container container;
      if (book_number == "UnicaMirandoAlMar") {

        container.sendBook(socket, "book001");

      } else if (book_number == "Cocori") {
      
        container.sendBook(socket, "book002");

      }else {
      std::cout << "HTTP/1.0 404 Not Found\r\n\r\n";
      }
    } else {
      std::cout << "HTTP/1.0 404 Not Found\r\n\r\n";
    }
  } else {
    //std::cout << "HTTP/1.0 404 Not Found\r\n\r\n";
  }
}

int main(int argc, char **argv) {

  Container container;
  char broadcastMessage[BUFSIZE];

  snprintf(broadcastMessage, BUFSIZE, "Libro: %s\nIP del Servidor: %s\nPuerto: %d\n"
    , container.listBooks().c_str() , BROADCAST_IP, BROADCAST_PORT);

  sendBroadcast( container.listBooks().c_str(), BROADCAST_IP, BROADCAST_PORT, broadcastMessage);

  std::thread broadcastThread(receiveBroadcast);

  broadcastThread.join();

  return 0;
}