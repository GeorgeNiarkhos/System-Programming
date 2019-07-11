OBJ = main.o functions.o structs.o

default: $(OBJ)  
	@g++ $(OBJ) -o mirror_client

main.o: main.cpp
	@g++ -c main.cpp

gdb: main.cpp
	@g++ -g main.cpp -o mirror_client

functions.o: functions.cpp ./functions.hpp
	@g++ -c functions.cpp

structs.o: structs.cpp ./structs.hpp
	@g++ -c structs.cpp

run: main.o
	@./mirror_client -n 1 -c ./common -i ./1_input -m ./1_mirror -b 100 -l log_file1

run2: main.o
	@./mirror_client -n 2 -c ./common -i ./2_input -m ./2_mirror -b 200 -l log_file2

run3: main.o
	@./mirror_client -n 3 -c ./common -i ./3_input -m ./3_mirror -b 300 -l log_file3

run4: main.o
	@./mirror_client -n 4 -c ./common -i ./4_input -m ./4_mirror -b 300 -l log_file4

val: main.o
	@valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes --track-origins=yes ./mirror_client -n 1 -c ./common -i ./1_input -m ./1_mirror -b 100 -l log_file1

val2: main.o
	@valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes --track-origins=yes ./mirror_client -n 2 -c ./common -i ./2_input -m ./2_mirror -b 200 -l log_file2

clean:
	@rm -f *.o ./mirror_client ./log_file* ./test1.txt
	@rm -rf ./inp/*
	@rm -rf ./1_mirror ./common ./2_mirror ./3_mirror ./4_mirror
	

