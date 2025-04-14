CC = g++
CXXFLAGS = -std=c++11 -pthread
LDFLAGS = -lrt -pthread

all: server cliente

server: src/server.cpp
	$(CC) $(CXXFLAGS) src/server.cpp -o server $(LDFLAGS)

cliente: src/cliente.cpp
	$(CC) $(CXXFLAGS) src/cliente.cpp -o cliente $(LDFLAGS)

clean:
	rm -f server cliente