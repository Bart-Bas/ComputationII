.PHONY: clean ./obj ./lib ./build

./obj/ant.o: ./src/ant.c | ./obj
	gcc -o ./obj/ant.o ./src/ant.c -c -g
./lib/libant.a: ./obj/ant.o | ./lib
	ar rcs ./lib/libant.a ./obj/ant.o
antbot: ./src/main.c ./lib/libant.a | ./build
	gcc -o ./build/antbot -static ./src/main.c -L./lib -I./include -lant -lm -g

clean:
	rm -f ./includes/*~ ./src/*~ ./build/* ./lib/* ./obj/*

./obj:
	mkdir -p ./obj

./lib:
	mkdir -p ./lib

./build:
	mkdir -p ./build
