CXX = g++
CXXFLAGS = -I. -Wall# -std=c++20

# LIBS =-lsalticidae -lssl -lcrypto -luv
LIBS =-lsalticidae -lssl -lcrypto -luv -lpthread -lcrypto++
all: executable

debug: CXXFLAGS += -g -DDEBUG
debug: executable



executable: main.o handler.o messages.o node.o TrustManager.o rsakey.o checkpoint.o
	$(CXX) $(CXXFLAGS) -o main main.o config.h messages.o handler.o node.o TrustManager.o rsakey.o checkpoint.o $(LIBS)

main.o: main.cpp config.h handler.h
	$(CXX) $(CXXFLAGS) -c main.cpp

handler.o: handler.cpp handler.h config.h messages.h rsakey.h
	$(CXX) $(CXXFLAGS) -c handler.cpp

node.o: node.cpp node.h config.h rsakey.h
	$(CXX) $(CXXFLAGS) -c node.cpp

messages.o: messages.cpp messages.h config.h
	$(CXX) $(CXXFLAGS) -c messages.cpp

trust_manager.o: TrustManager.cpp TrustManager.h 
	$(CXX) $(CXXFLAGS) -c TrustManager.cpp

rsakey.o: rsakey.cpp rsakey.h
	$(CXX) $(CXXFLAGS) -c rsakey.cpp

checkpoint.o: checkpoint.cpp checkpoint.h
	$(CXX) $(CXXFLAGS) -c checkpoint.cpp 



.PHONY: clean
clean:
	rm *.o main

.PHONY: run
run:
	clear
	./main
