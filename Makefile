CC=g++

physics: main.cpp
	$(CC) -c main.cpp
	$(CC) main.o -o physics -lsfml-graphics -lsfml-window -lsfml-system -l Box2D
	rm main.o
