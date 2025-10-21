#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>

#include "Holder.h"
#include "Socket.h"

#define BUFSIZE 512
#define BROADCAST_PORT 8000
#define PORT_CLIENT 8080
#define BROADCAST_IP "127.0.0.1"
#define SIZELISTBOOKS 100

char buffer[BUFSIZE];

Holder::Holder() {
  this->BooksList = (char **) malloc(SIZELISTBOOKS * sizeof(char));
  this->bookPiece = (char **) malloc(SIZELISTBOOKS * sizeof(char));
  if (!this->BooksList) { perror("ERROR: coult not allocate memory\n");}

  this->numberBooks = 0;
  this->pieces = 0;
}


Holder::~Holder() {
  for (int i = 0; i < this->numberBooks; ++i) {
    if (this->BooksList[i]) { free(BooksList[i]); }
  }
  free(this->BooksList);
  this->numberBooks = 0;
}


void Holder::sendBroadcast(const char* serverIP, int port, const char* message) {
  
  int UDPSockFd = socket(AF_INET, SOCK_DGRAM, 0);
  if (UDPSockFd == -1) {
    perror("ERROR: Coult not create socket");
    exit(EXIT_FAILURE);
  }

  int broadcastEnable = 1;
  if (setsockopt(UDPSockFd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable
    , sizeof(broadcastEnable)) == -1) {
    perror("ERROR: setting socket option SO_BROADCAST");
    close(UDPSockFd);
    exit(EXIT_FAILURE);
  }

  int DisableLoopback = 0;
  if (setsockopt(UDPSockFd, IPPROTO_IP, IP_MULTICAST_LOOP, &DisableLoopback
    , sizeof(DisableLoopback)) == -1) {
    perror("ERROR: setting socket option IP_MULTICAST_LOOP");
    close(UDPSockFd);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in broadcastAddr;
  
  memset(&broadcastAddr, 0, sizeof(broadcastAddr));
  broadcastAddr.sin_family = AF_INET;
  broadcastAddr.sin_port = htons(BROADCAST_PORT);
  broadcastAddr.sin_addr.s_addr = inet_addr(BROADCAST_IP);

  char broadcastMessage[BUFSIZE];
  snprintf(broadcastMessage, BUFSIZE, message, serverIP, port);

  if (sendto(UDPSockFd, broadcastMessage, strlen(broadcastMessage), 0
    , (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr)) == -1) {
    perror("sendto failed");
    close(UDPSockFd);
    exit(EXIT_FAILURE);
  }

  std::cout << "Broadcast enviado desde Tenedor" << std::endl 
    << "--------------------------------" << std::endl 
    << broadcastMessage << std::endl;

  close(UDPSockFd);
}


void Holder::receiveBroadcast() {

  int UDPSockFd = socket(AF_INET, SOCK_DGRAM, 0);
  if (UDPSockFd == -1) {
    perror("ERROR: Coult not create socket");
    exit(EXIT_FAILURE);
  }

  int enableReuse = 1;
  if (setsockopt(UDPSockFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse
    , sizeof(enableReuse)) == -1) {
    perror("ERROR: setting socket option SO_REUSEADDR");
    close(UDPSockFd);
    exit(EXIT_FAILURE);
  }

  int disableLoopback = 0;
  if (setsockopt(UDPSockFd, IPPROTO_IP, IP_MULTICAST_LOOP, &disableLoopback
    , sizeof(disableLoopback)) == -1) {
  perror("ERROR: setting socket option IP_MULTICAST_LOOP");
    close(UDPSockFd);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in recvAddr;
  memset(&recvAddr, 0, sizeof(recvAddr));
  recvAddr.sin_family = AF_INET;
  recvAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Only receves in this IP
  recvAddr.sin_port = htons(BROADCAST_PORT);

  if (bind(UDPSockFd, (struct sockaddr*)&recvAddr, sizeof(recvAddr)) < 0) {
    perror("ERROR: bind failed");
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

    std::string strBuffer = buffer;

    if (strBuffer.find("Libro:") == std::string::npos) {  // recibe el libro

      this->bookPiece[this->pieces] = (char *)malloc((std::string(buffer).length() + 1) * sizeof(char));

      strcpy(this->bookPiece[this->pieces], buffer);
      ++this->pieces;

    } else { // recibe la lista de libros
      this->BooksList[this->numberBooks] = (char *)malloc((std::string(buffer).length() + 1) * sizeof(char));

      size_t start = strBuffer.find("Libro: ") + 7;
      std::string nameBook = strBuffer.substr(start,  strBuffer.find('\n', start) - start);
      strcpy(this->BooksList[this->numberBooks], nameBook.c_str());

      ++this->numberBooks;
    }

    std::cout << "Broadcast recibido "  << std::endl << buffer << std::endl;
  }

  close(UDPSockFd);
}


int main() {
  Holder holder;

  holder.sendBroadcast(BROADCAST_IP, BROADCAST_PORT, "IP del Servidor: %s\nPuerto: %d\n");
  
  std::thread broadcastThread(&Holder::receiveBroadcast, &holder);

  VSocket *s1, *s2;
  char bufferClient[BUFSIZE];
  char bufferServer[BUFSIZE];

  s1 = new Socket('s');

  s1->Bind(PORT_CLIENT);            
  s1->Listen(5);

  for( ; ; ) {

    s2 = s1->Accept();	
    
    while (true) {

      int bytesRead = s2->Read(bufferClient, BUFSIZE);

      std::istringstream requestStream(bufferClient);
      std::string method, uri, version;
      requestStream >> method >> uri >> version;
    

      if (uri == "/libros") { // el cliente solicita el libro
        for (int i = 0; i < holder.numberBooks; ++i) {
          s2->Write(holder.BooksList[i]);
        }
      } else {  // el cliente solicita la lista de libros

        VSocket *s3;
        s3 =  new Socket('s');
        s3->Connect("127.0.0.1", 8080);
        s3->Write(bufferClient);
        s3->Read(bufferServer, BUFSIZE);
        std::cout << bufferServer << std::endl;


        for (int i = 0; i < holder.pieces; ++i) {
          s2->Write(holder.bookPiece[i]);
        }
        memset(bufferServer, 0, BUFSIZE);
      }
      memset(bufferClient, 0, BUFSIZE);

      if (bytesRead <= 0) { break; }
    }
    s2->Close();	
  }

  broadcastThread.join();

  return 0;
}