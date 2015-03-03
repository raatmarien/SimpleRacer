CC=g++

physics: main.o groundTileMap.o car.o carTimer.o
	$(CC) $(CFLAGS) main.o groundTileMap.o car.o carTimer.o -o simpleracer -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -l Box2D
