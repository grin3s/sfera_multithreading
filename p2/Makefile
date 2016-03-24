SRC = chatsrv.cpp chat_server.cpp client_buffer.cpp
HDR = chat_server.h client_buffer.h

all: $(SRC) $(HDR)
	g++ -O2 -std=c++11 -o chatsrv $(SRC)
	
clean:
	rm -f chatsrv
