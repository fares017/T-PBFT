CXX = g++
CXXFLAGS = -I. -Wall# -std=c++20

# LIBS =-lsalticidae -lssl -lcrypto -luv
LIBS =-lsalticidae -lssl -lcrypto -luv -lpthread
all: executable

debug: CXXFLAGS += -g -DDEBUG
debug: executable

executable: main.o handler.o messages.o node.o
	$(CXX) $(CXXFLAGS) -o main main.o config.h messages.o handler.o node.o $(LIBS)

main.o: main.cpp config.h handler.h
	$(CXX) $(CXXFLAGS) -c main.cpp

handler.o: handler.cpp handler.h config.h messages.h
	$(CXX) $(CXXFLAGS) -c handler.cpp

node.o: node.cpp node.h config.h
	$(CXX) $(CXXFLAGS) -c node.cpp

messages.o: messages.cpp messages.h config.h
	$(CXX) $(CXXFLAGS) -c messages.cpp

.PHONY: clean
clean:
	rm *.o main

.PHONY: run
run:
	clear
	./main
