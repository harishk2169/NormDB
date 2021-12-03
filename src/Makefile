client: client.cpp
	g++ client.cpp -o client

server: server.cpp server.h parser.cpp disk.cpp disk.h parser.h
	g++ -Werror -Wno-pointer-arith -o server server.cpp parser.cpp disk.cpp

