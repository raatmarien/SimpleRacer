CC=g++

physics: main.o groundTileMap.o car.o carTimer.o carAI.o
	$(CC) $(CFLAGS) main.o groundTileMap.o car.o carTimer.o carAI.o -o simpleracer -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -l Box2D
