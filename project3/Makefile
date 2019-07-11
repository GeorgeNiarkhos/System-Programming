OBJ1 = dropbox_server.o client_list.o
OBJ2 = dropbox_client.o client_list.o threads.o functions.o

deafault: $(OBJ1) $(OBJ2)
	@g++ $(OBJ1) -o dropbox_server
	@g++ $(OBJ2) -o dropbox_client -lpthread

server: $(OBJ1)
	@g++ $(OBJ1) -o dropbox_server

client: $(OBJ2)
	@g++ $(OBJ2) -o dropbox_client -lpthread -g

client_list.o: client_list.cpp client_list.hpp
	@g++ -c client_list.cpp -lpthread

threads.o: threads.cpp threads.hpp
	@g++ -c threads.cpp -pthread -lpthread

functions.o: functions.cpp functions.hpp
	@g++ -c functions.cpp -lpthread

dropbox_server.o: dropbox_server.cpp
	@g++ -c dropbox_server.cpp

dropbox_client.o: dropbox_client.cpp
	@g++ -c dropbox_client.cpp -lpthread

runs: dropbox_server.o
	@./dropbox_server -p 9204

runc: dropbox_client.o
	@./dropbox_client -d ./data -p 9203 -w 3 -b 10 -sp 9204 -sip linux27.di.uoa.gr	 

vals: $(OBJ1)
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./dropbox_server -p 9204

valc: $(OBJ2)
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./dropbox_client -d ./data -p 9203 -w 3 -b 10 -sp 9204 -sip linux27.di.uoa.gr

clean:
	@rm -f *.o ./dropbox_server ./dropbox_client
	

