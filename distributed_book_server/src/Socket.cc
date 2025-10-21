/**
 *   CI0123 PIRO
 *   Clase para utilizar los "sockets" en Linux
 *
 **/

#include <stdio.h>	// for perror
#include <stdlib.h>	// for exit
#include <string.h>	// for memset
#include <arpa/inet.h>	// for inet_pton
#include <sys/types.h>	// for connect 
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>	// for errno
#include <stdexcept>	// for std::runtime_error

#include "VSocket.h"
#include "Socket.h"


/**
  *  Class constructor
  *     use Unix socket system call
  *
  *  @param	char type: socket type to define
  *     's' for stream
  *     'd' for datagram
  *  @param	bool ipv6: if we need a IPv6 socket
  *
 **/
Socket::Socket( char type, bool IPv6 ){

   this->CreateVSocket( type, IPv6 );

}


/**
  *  Class constructor
  *
  *  @param     int id: socket descriptor
  *
 **/
Socket::Socket( int id ) {

   this->CreateVSocket( id );

}


/**
  * Class destructor
  *
 **/
Socket::~Socket(){
   this->Close();
}


/**
  * Connect method
  *   use "MakeConnection" from base class
  *
  * @param	char * host: host address in dot notation, example "10.1.104.187"
  * @param	int port: process address, example 80
  *
 **/
int Socket::Connect( const char * host, int port ) {

   return this->MakeConnection( host, port );

}


/**
  * Connect method
  *   use "MakeConnection" from base class
  *
  * @param      char * host: host address in dns notation, example "os.ecci.ucr.ac.cr"
  * @param      char * service: process address, example "http"
  *
 **/
int Socket::Connect( const char *host, const char *service ) {

   return this->MakeConnection( host, service );
   
}


/**
  * Read method
  *   use "read" Unix system call (man 3 read)
  *
  * @param	void * text: buffer to store data read from socket
  * @param	int size: buffer capacity, read will stop if buffer is full
  *
 **/
size_t Socket::Read( void * text, size_t size ) {
   ssize_t bytesRead = read(this->idSocket, text, size);

   if ( -1 == bytesRead ) {
      throw std::runtime_error("read: " + std::string(strerror(errno)));
   }

   return static_cast<size_t>(bytesRead);

}


/**
  * Write method
  *   use "write" Unix system call (man 3 write)
  *
  * @param	void * buffer: buffer to store data write to socket
  * @param	size_t size: buffer capacity, number of bytes to write
  *
 **/
size_t Socket::Write( const void *text, size_t size ) {
	ssize_t bytesWritten = write(this->idSocket, text, size);

   if (bytesWritten == -1) {
		throw std::runtime_error("write: " + std::string(strerror(errno)));
	}
	return static_cast<size_t>(bytesWritten);
}


/**
  * Write method
  *
  * @param	char * text: string to store data write to socket
  *
  *  This method write a string to socket, use strlen to determine how many bytes
  *
 **/
size_t Socket::Write( const char *text ) {
	ssize_t bytesWritten = write( this->idSocket, text, strlen(text));
   
   if (bytesWritten == -1) {
		throw std::runtime_error("write: " + std::string(strerror(errno)));
	}
	return static_cast<size_t>(bytesWritten);

}


/**
  * Accept method
  *    use base class to accept connections
  *
  *  @returns   a new class instance
  *
  *  Waits for a new connection to service (TCP mode: stream)
  *
 **/
Socket * Socket::Accept(){
   int id;
   Socket * other;

   id = this->DoAccept();

   other = new Socket( id );

   return other;

}
