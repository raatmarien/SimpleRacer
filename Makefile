CC=g++

physics: main.o groundTileMap.o car.o
	$(CC) main.o groundTileMap.o car.o -o simpleracer -lsfml-graphics -lsfml-window -lsfml-system -l Box2D
