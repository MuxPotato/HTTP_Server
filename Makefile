CXX=g++

all: httpserver

httpserver: http_server.cpp
	$(CXX) -o httpserver http_server.cpp

clean: 
	rm httpserver 