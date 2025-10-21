/**
 *  Establece la definición de la clase Socket para efectuar la comunicación
 *  de procesos que no comparten memoria, utilizando un esquema de memoria
 *  distribuida.  El desarrollo de esta clase se hará en varias etapas, primero
 *  los métodos necesarios para los clientes, en la otras etapas los métodos para el servidor,
 *  manejo de IP-v6, conexiones seguras y otros
 *
 *  Universidad de Costa Rica
 *  ECCI
 *  CI0123 Proyecto integrador de redes y sistemas operativos
 *  2024-ii
 *  Grupos: 3, 5
 *
 **/

#include <cstddef>
#include <stdexcept>
#include <cstdio>
#include <cstring>			// memset

#include <sys/socket.h>
#include <arpa/inet.h>		// ntohs
#include <unistd.h>			// close
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>			// getaddrinfo, freeaddrinfo

#include "VSocket.h"


/**
  *  Class initializer
  *     use Unix socket system call
  *
  *  @param     char t: socket type to define
  *     's' for stream
  *     'd' for datagram
  *  @param     bool ipv6: if we need a IPv6 socket
  *
 **/
void VSocket::CreateVSocket( char t, bool IPv6 ) {

   this->idSocket = socket(IPv6 ? AF_INET6 : AF_INET, t == 's' ? SOCK_STREAM : SOCK_DGRAM, 0);
   this->IPv6 = IPv6;

   if (this->idSocket == -1) {
      throw( std::runtime_error( "VSocket::VSocket, createVSocket" ));
   }

   // Habilitar SO_REUSEADDR para permitir reutilizar el puerto y evitar errores de "address already in use"
   int opt = 1;
   if (setsockopt(this->idSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
      throw std::runtime_error("setsockopt: " + std::string(strerror(errno)));
   }

}


/**
  *  Class initializer
  *     use Unix socket system call
  *
  *  @param     int id: descriptor for an already opened socket (accept)
  *
 **/
void VSocket::CreateVSocket( int id ){

   this->idSocket = id;

   if (this->idSocket == -1) {
      throw std::runtime_error("socket: " + std::string(strerror(errno)));
   }

}


/**
  * Class destructor
  *
 **/
VSocket::~VSocket() {

   this->Close();

}


/**
  * Close method
  *    use Unix close system call (once opened a socket is managed like a file in Unix)
  *
 **/
void VSocket::Close() {

	if (this->idSocket != -1) {

		close(this->idSocket);
		this->idSocket = -1;
	}
   
}


/**
  * MakeConnection method
  *   use "connect" Unix system call
  *
  * @param      char * host: host address in dot notation, example "10.1.104.187"
  * @param      int port: process address, example 80
  *
 **/
int VSocket::MakeConnection( const char * hostip, int port ) {
   struct addrinfo hints, *res;
   int status;
   char port_str[6];
   sprintf(port_str, "%d", port);

   memset(&hints, 0, sizeof hints);
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;

   if ((status = getaddrinfo(hostip, port_str, &hints, &res)) != 0) {
      throw std::runtime_error("getaddrinfo: " + std::string(gai_strerror(status)));
   }

   if (connect(this->idSocket, res->ai_addr, res->ai_addrlen) == -1) {
      freeaddrinfo(res);
      throw std::runtime_error("connect: " + std::string(strerror(errno)));
   }

   freeaddrinfo(res);

   return status;
}


/**
  * MakeConnection method
  *   use "connect" Unix system call
  *
  * @param      char * host: host address in dns notation, example "os.ecci.ucr.ac.cr"
  * @param      char * service: process address, example "http"
  *
 **/
int VSocket::MakeConnection( const char *host, const char *service ) {

   struct addrinfo hints, *result, *rp;
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_family = AF_UNSPEC; // Allow IPv4 or IPv6
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = 0;
   hints.ai_protocol = 0;

   int st = getaddrinfo( host, service, &hints, &result );
   if ( st == -1 ) {
      throw( std::runtime_error( "VSocket::MakeConnection, getaddrinf" ));
   }
   for ( rp = result; rp; rp->ai_next ) {
      st = connect( this->idSocket, rp->ai_addr, rp->ai_addrlen );
      if (st == 0) {
         break;
      }
   }
   freeaddrinfo( result );
   return st;
}


/**
  * Listen method
  *
  * @param      int queue: max pending connections to enqueue (server mode)
  *
  *  This method define how many elements can wait in queue
  *
 **/
int VSocket::Listen( int queue ) {

   int st = listen(this->idSocket, queue);

   if ( -1 == st ) { throw std::runtime_error( "VSocket::Listen( int )" ); }

   return st;

}

/**
  * Bind method
  *    use "bind" Unix system call (man 3 bind) (server mode)
  *
  * @param      int port: bind a unamed socket to a port defined in sockaddr structure
  *
  *  Links the calling process to a service at port
  *
 **/
int VSocket::Bind( int port ) {

   struct sockaddr_in server_addr;

   server_addr.sin_family = AF_INET;  // Definimos la familia para IPv4
   server_addr.sin_addr.s_addr = htonl( INADDR_ANY );
   server_addr.sin_port = htons( port );

   memset( server_addr.sin_zero, '\0', sizeof (server_addr.sin_zero));

   int st = bind(this->idSocket, (sockaddr *) &server_addr, sizeof(server_addr));

   if ( st == -1 ) { throw( std::runtime_error( "VSocket::Bind, bind" )); }

   return st;
}


/**
  * DoAccept method
  *    use "accept" Unix system call (man 3 accept) (server mode)
  *
  *  @returns   a new class instance
  *
  *  Waits for a new connection to service (TCP mode: stream)
  *
 **/
int VSocket::DoAccept() {

   struct sockaddr_in server_addr;

   socklen_t server_addr_len = sizeof(server_addr);

   int st = accept(this->idSocket, (struct sockaddr *) &server_addr, (socklen_t *) &server_addr_len);
    
   if ( -1 == st ) { throw std::runtime_error( "VSocket::DoAccept()" ); }

   return st;

}


/**
  * Shutdown method
  *    use "shutdown" Unix system call (man 3 shutdown)
  *
  *  @param	int mode define how to cease socket operation
  *
  *  Partial close the connection (TCP mode)
  *
 **/
int VSocket::Shutdown( int mode ) {

   int st = shutdown(this->idSocket, mode);

   if ( -1 == st ) { throw std::runtime_error( "VSocket::Shutdown( int )" ); }

   return st;

}


/**
  *  sendTo method
  *
  *  @param	const void * buffer: data to send
  *  @param	size_t size data size to send
  *  @param	void * addr address to send data
  *
  *  Send data to another network point (addr) without connection (Datagram)
  *
 **/
size_t VSocket::sendTo( const void * buffer, size_t size, void * addr ) {

   sockaddr_in *dest = (sockaddr_in *) addr;
   
   ssize_t st = sendto(this->idSocket, buffer, size, 0, (struct sockaddr *)addr, sizeof(*dest));

   if ( st == -1 ) { throw( std::runtime_error( "VSocket::sendTo, send" )); }

   return static_cast<size_t>(st);
}


/**
  *  recvFrom method
  *
  *  @param	const void * buffer: data to send
  *  @param	size_t size data size to send
  *  @param	void * addr address to receive from data
  *
  *  @return	size_t bytes received
  *
  *  Receive data from another network point (addr) without connection (Datagram)
  *
 **/
size_t VSocket::recvFrom( void * buffer, size_t size, void * addr ) {

   sockaddr_in *src = (sockaddr_in *) addr;
   socklen_t addr_len = sizeof(*src);
   ssize_t st = recvfrom(this->idSocket, buffer, size, 0, (struct sockaddr *) src,(socklen_t *) &addr_len);

   if ( st == -1 ) { throw( std::runtime_error( "VSocket::recvFrom, recv" )); }

   return static_cast<size_t>(st);
}

