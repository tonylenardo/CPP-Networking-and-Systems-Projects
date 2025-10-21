#include "Container.h"

#include <algorithm>
#include <dirent.h>  // For directory operations
#include <fstream>   // For file operations
#include <sys/stat.h>  // Para mkdir
#include <cstring>     // Para strcat
#include <iostream>
#include <sstream>

#define ROOT_PATH "../Disk/roots/root.txt"
#define BUFSIZE 512

Container::Container() {}

Container::~Container() {}

/**
 * @brief Create a dir and divided the book in parts of 256 bytes.
 * 
 * @param name of the book.
 */
void Container::insertBook(std::string name) {
  std::string  path = "../Disk/" + std::string(name);
  int st = mkdir(path.c_str(), 0777);
  if ( st != 0) { perror("Could not create dir of the book"); }

  std::string split = "split -b 256 -d --additional-suffix=\".txt\" " + name +".txt "+ path +"/book.part";
  st = system(split.c_str());
  if (st != 0) { perror("The book could not be divided"); }

}

/**
 * @brief 
 * 
 * @param path 
 * @return std::string 
 */
std::string Container::getLine(std::string path) { 
  std::ifstream file(path);
  std::string line = "\0";

  if (file.is_open()) { 

    while (getline(file, line)) {}
    file.close(); 

  } else { perror(("Error: Could not open file{"+path+"}.\n").c_str()); }

  return line;

}

/**
 * @brief 
 * 
 * @param buffer 
 * @param size 
 */
std::string Container::listBooks(/*VSocket *socket*/) {
  std::string path = getLine(ROOT_PATH); 
  std::ifstream file(path);

  if (!file.is_open()) {
    std::string error_msg = "Error: Could not open file " + path + ".\n";
    //socket->Write(error_msg.c_str());
    return error_msg;
  }

  std::string line;
  std::ostringstream response;
  while (getline(file, line)) {
    std::string nameBook = std::string(line.c_str(), strstr(line.c_str(), "|"));
    const char* bookNumber = strstr(line.c_str(), "books/") + strlen("books/");
    response << nameBook << " - ";
  }

  file.close();

  //socket->Write(response.str().c_str());
  return response.str();
}

/**
 * @brief 
 * 
 * @param num_book 
 * @return std::string 
 */
std::string Container::getBookPath(VSocket *socket, const char *num_book) {
  std::string path = getLine(ROOT_PATH);
  std::ifstream file(path);
  bool isAvaible = false;
  if (file.is_open()) { 
    while (getline(file, path)) { // each line is type "name|../Disk/books/book00n"
      if (std::string(strstr(path.c_str(), "books/") + strlen("books/")) == num_book){ 
        isAvaible = true;
        break; 
      }
    }
    if (!isAvaible) { socket->Write("Error: book is not available\n"); }
    file.close(); 
    path = strstr(path.c_str(), "|") + 1; //path = ..Disk/books/book001
    path = path + ".part00.txt";

  } else { perror(("Error: Could not open file{"+path+"}.\n").c_str()); return "\0"; }
  return path;
}


/**
 * @brief 
 * 
 * @param socket 
 * @param filename 
 */
void Container::sendBook(VSocket *socket, const char *book_number) {
  std::string path = getBookPath(socket, book_number);
  std::ifstream file(path);

  if (file.is_open()) {
    std::string partPath;
    while (getline(file, partPath)) { // Recorre las n partes del libro
      if (partPath.empty()) {
        continue; // Ignorar líneas vacías
      }
      
      if (partPath.find('|') != std::string::npos) {
        partPath.erase(0, 1); // Delete "|" from path
        file.close();
        file.open(partPath);
        continue;
      }
      std::ifstream partBook(partPath);
      if (partBook.is_open()) {
        std::string line;
        while (getline(partBook, line)) { // lee la n-sima parte del libro
          socket->Write(line.c_str());
          //completeBook += line;
        }
        partBook.close();
      } else {
        std::cerr << "Error: Could not open part file " << partPath << std::endl;
      }
    }
    file.close();
  } else {
    std::string error_msg = "Error: Could not open file " + path + ".\n";
    socket->Write(error_msg.c_str());
  }
}

bool Container::hasNext(std::string &path) {
  // |../Disk/books/book.part01
  if (path.find('|') != std::string::npos) {
    path = path.substr(path.find('|') + 1); // Extract the path after '|'
    return true;
  }
  return false;
}
