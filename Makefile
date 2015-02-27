CC=g++

physics: main.o groundTileMap.o
	$(CC) main.o groundTileMap.o -o physics -lsfml-graphics -lsfml-window -lsfml-system -l Box2D
