file = ./Parser/Parser.c ./CodeWriter/CodeWrite.c main.c
syntax : 
		gcc --syntax-only $(file)
build : 
		gcc -g -o vm $(file)
gdb : 
	gdb --args ./vm ./test.vm
test : 
	gcc -g -o vm $(file)
	./vm ./test.vm