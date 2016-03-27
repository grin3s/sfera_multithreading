SRC = chatsrv.cpp chat_server.cpp client_buffer.cpp
HDR = chat_server.h client_buffer.h

client: chat_client.h chat_client.cpp chat_cl.cpp
	g++ -O2 -std=c++11 -o chat_client chat_cl.cpp chat_client.h chat_client.cpp chat_utils.h

all: $(SRC) $(HDR)
	g++ -O2 -std=c++11 -o chatsrv $(SRC) chat_utils.h
	make client
	
	
clean:
	rm -f chatsrv
	rm -f chat_client
