#include <stdio.h>
#include <cstring>
#include "Socket.h"
#include <iostream>

#define PORT 8080
#define BUFSIZE 512

void displayResponse(VSocket *socket, char* buffer) {
  memset(buffer, 0, BUFSIZE);
  while (socket->Read(buffer, BUFSIZE) > 0) {
    std::cout << buffer;
    memset(buffer, 0, BUFSIZE);
  }
}

void sendHttpRequest(VSocket *socket) {
  const char* request = "GET /book002 HTTP/1.0\r\n\r\n";
  socket->Write(request);
}

int main(int argc, char **argv) {
  VSocket *s;
  char buffer[BUFSIZE];

  s = new Socket('s');
  s->Connect("127.0.0.1", PORT);

  std::cout << "1. Lista de libros disponibles \n" 
    "2. salir\n" << std::endl;
  while (true) {
    int choice;
    std::cin >> choice;
    const char* request = "GET /libros HTTP/1.0\r\n\r\n";
    std::cout << request;
    s->Write(request);

   // Muestra los libros
    s->Read(buffer, BUFSIZE);
    std::cout << buffer;
    memset(buffer, 0, BUFSIZE);

    std::cout << "Ingrese el nombre de algún libro disponible: " << std::endl;
    std::string nameBook;
    std::cin >> nameBook;  // Ingresa el nombre del libro
    std::string url = "GET /" + nameBook + " HTTP/1.0\r\n\r\n";
    std::cout << url;
    s->Write(url.c_str()); // Pide el libro

    displayResponse(s, buffer);
    std::cout  << "1. Actualizar lista de libros\n" << "2. Salir\n" << std::endl;
  }

  delete s;
  return 0;
}


