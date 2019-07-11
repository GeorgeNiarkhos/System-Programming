OBJ = main.o dataStructs.o hash.o functions.o

default: $(OBJ)
	@g++ $(OBJ) -o bitcoin

main.o: main.cpp
	@g++ -c main.cpp

gdb: main.cpp
	@g++ -g main.cpp -o bitcoin

dataStructs.o: dataStructs.cpp ./dataStructs.hpp
	@g++ -c dataStructs.cpp

hash.o: hash.cpp ./hash.hpp
	@g++ -c hash.cpp

functions.o: functions.cpp ./functions.hpp
	@g++ -c functions.cpp

run: main.o
	@./bitcoin -a bitCoinBalancesFile.txt -t transactionsFile.txt -v 50 -h1 8 -h2 8 -b 48

runBig: main.o
	@./bitcoin -a bitCoinBalaceFileBig2.txt -t transactionsFileIDs.txt -v 10000 -h1 17 -h2 19 -b 200

val: main.o
	@valgrind --leak-check=full --show-leak-kinds=all ./bitcoin -a bitCoinBalancesFile.txt -t transactionsFile.txt -v 10 -h1 8 -h2 8 -b 48

valBig: main.o
	@valgrind --leak-check=full --show-leak-kinds=all ./bitcoin -a bitCoinBalaceFileBig2.txt -t transactionsFileIDs.txt -v 10000 -h1 17 -h2 19 -b 200

clean:
	@rm -f *.o ./bitcoin
