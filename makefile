file = ./Parser/Helper.c ./Parser/Parser.c ./CodeWriter/CodeWrite.c main.c
syntax : 
		gcc --syntax-only $(file)
build : 
		gcc -g -o vm -DDEBUG_HEADER="\"NonDebug.h\"" $(file)
debug : 
		gcc -g -o vm -DDEBUG_HEADER="\"Debug.h\"" $(file)
gdb : 
	gdb --args ./vm ./test.vm
test : 
	gcc -g -o vm $(file)
	./vm ./test.vm
run : 
	./vm ./test.vm
optimize : 
	gcc -O -o vm $(file)
check: 
	valgrind  --show-leak-kinds=all --track-origins=yes --leak-check=full --log-file="log.txt"  ./vm ./test.vm
massif : 
	valgrind --tool=massif --massif-out-file=massif.txt  --time-unit=B ./vm ./test.vm
