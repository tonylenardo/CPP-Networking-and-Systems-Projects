# Linux Makefile

CC  := gcc
CXX := g++
CFLAGS := -g -c
CXXFLAGS := -g -c 
LDFLAGS := 
DEPS := Socket.h Container.h  
TARGETS := BookServer.out Client.out Holder.out

ALL := $(TARGETS)
$(info $$TARGETS = $(TARGETS))

%.o: %.cc $(DEPS)
	$(CXX) $(CXXFLAGS) $<

%.out: %.o VSocket.o Socket.o Container.o  
	$(CXX) $^ $(LDFLAGS) -o $@

BookServer.out: BookServer.o VSocket.o Socket.o Container.o
	$(CXX) $^ $(LDFLAGS) -o $@ -lpthread

Holder.out: Holder.o  VSocket.o Socket.o
	$(CXX) $^ $(LDFLAGS) -o $@ -lpthread

Client.out: Client.o VSocket.o Socket.o Container.o
	$(CXX) $^ $(LDFLAGS) -o $@

Container.o: Container.cc Container.h
	$(CXX) $(CXXFLAGS) Container.cc


clean:
	rm -f *.o *.out