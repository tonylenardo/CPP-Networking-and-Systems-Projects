
#ifndef CONTAINER_H
#define CONTAINER_H

#include <cstddef>
#include <iostream>
#include <string>

#include "Socket.h"

class Container {
  public:
    Container();
    ~Container();
    void insertBook(std::string name);
    std::string listBooks(/*VSocket *socket*/);
    void sendBook(VSocket *socket, const char *filename);
  private:
    std::string getLine(std::string path);
    std::string getBookPath(VSocket *socket, const char *num_book);
    bool hasNext(std::string &);
};

#endif
