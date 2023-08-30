CXX = g++
CXXFLAGS = -I. -Wall

LIBS =-lsalticidae -lssl -lcrypto -luv 
# -std=c++11 -pthread

all: executable

debug: CXXFLAGS += -DDEBUG -g
debug: executable

executable: main.o messages.o
	$(CXX) $(CXXFLAGS) -o main main.o ../server/config.h messages.o $(LIBS)

main.o: main.cpp ../server/config.h ../server/messages.h
	$(CXX) $(CXXFLAGS) -c main.cpp

messages.o: ../server/messages.cpp ../server/config.h
	$(CXX) $(CXXFLAGS) -c ../server/messages.cpp messages.o

.PHONY: clean
clean:
	rm *.o main
