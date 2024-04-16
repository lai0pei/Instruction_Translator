file = ./Parser/Helper.c ./Parser/Parser.c ./CodeWriter/CodeWrite.c main.c
syntax : 
		gcc --syntax-only $(file)
build : 
		gcc -g -o vm $(file)
gdb : 
	gdb --args ./vm ./test.vm
test : 
	gcc -g -o vm $(file)
	./vm ../projects/8/FunctionCalls/FibonacciElement/
run : 
	./vm ../projects/8/FunctionCalls/FibonacciElement/
optimize : 
	gcc -O -o vm $(file)
check: 
	valgrind  --show-leak-kinds=all --track-origins=yes --leak-check=full --log-file="log.txt"  ./vm ./t.c
massif : 
	valgrind --tool=massif --massif-out-file=massif.txt  --time-unit=B ./vm ../projects/8/FunctionCalls/FibonacciElement/
