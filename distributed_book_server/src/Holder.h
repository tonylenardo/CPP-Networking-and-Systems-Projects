#ifndef HOLDER_H
#define HOLDER_H

#include <iostream>
#include <cstring>
#include <thread>

class Holder{
  public:
    Holder();
    ~Holder();
    void receiveBroadcast();
    void sendBroadcast(const char* serverIP, int port, const char* message);
    void showListBooks();
    char **BooksList;
    char **bookPiece;
    int pieces;
    int numberBooks;
};

#endif