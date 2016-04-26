all: lab1 lab2 lab3

lab1: lab1-build
	@echo "--> Running lab1 in release mode"
	./lab1-release
	@echo "--> Running lab1 in debug mode"
	./lab1-debug

lab1-build:
	gcc -m32 -g -o lab1-debug lab1.cpp
	gcc -m32 -o lab1-release lab1.cpp


lab2: lab2-build
	@echo "--> Running lab2"
	./lab2-release

lab2-build:
	gcc -m32 -o lab2-release lab2.cpp



lab3: lab3-build
	@echo "--> Running lab3"
	./lab3-release

lab3-build:
	gcc -m32 -o lab3-release lab3.cpp -lpthread


clean:
	rm -r lab1-debug.dSYM
	rm *-debug
	rm *-release
	rm test_*.raw
