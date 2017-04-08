./obj/ant.o: ./src/ant.c
	gcc -o ./obj/ant.o ./src/ant.c -c -g
./lib/libant.a: ./obj/ant.o
	ar rcs ./lib/libant.a ./obj/ant.o
antbot: ./src/main.c ./lib/libant.a
	gcc -o ./build/antbot -static ./src/main.c -L./lib -I./include -lant -lm -g

.PHONY: clean
clean:
	rm -f ./includes/*~ ./src/*~ ./build/* ./lib/* ./obj/*
