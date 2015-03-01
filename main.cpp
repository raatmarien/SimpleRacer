#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "car.h"
#include "groundTileMap.h"
#include <math.h>
#include <string>
#include <fstream>
#include <iostream>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define SCALE 100
#define PI 3.14159265358979323846
#define PLAYER_WIDTH 88
#define PLAYER_HEIGHT 150

using namespace sf;

void handleEvents(RenderWindow *window);
void keyboardInput(RenderWindow *window);
void update(RenderWindow *window);
void simulatePhysics(RenderWindow *window);
void draw(RenderWindow *window);

Texture playerCarTex;
Car playerCar;
Texture tileMapTex;
groundTileMap tileMap;

View playerView(FloatRect(0, 0, 1920, 1080));

b2Vec2 gravity(0.0f, 0.0f);
b2World world(gravity);
b2Body *playerBody;

float box2DTimeStep = 1.0f / 60.0f;
int velocityIterations = 8;
int positionIterations = 3;

int main()
{
    RenderWindow window(VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Test");
    window.setVerticalSyncEnabled(true);
    window.setView(playerView);

    // Load textures
    if (!playerCarTex.loadFromFile("sprites/car_3.png")) return 0;
    if (!tileMapTex.loadFromFile("sprites/tiles_3.png")) return 0;
    
    // Set up car
    playerCar.initialize(0.5f, 0.982f, 0.1f, (PI / 8), 0.85f, playerCarTex
                         , b2Vec2(PLAYER_WIDTH, PLAYER_HEIGHT), SCALE
                         , &world, b2Vec2(800,800));

    // Set up groundTileMap
    tileMap.genGroundTileMap("sprites/big_tilesmap_2.pgm", tileMapTex, 100
                             , 100, 5, &world);

    while(window.isOpen()) {
        keyboardInput(&window);
        handleEvents(&window);
        simulatePhysics(&window);
        update(&window);
        window.setView(playerView);
        draw(&window);
    }
    return 0;
}

void handleEvents(RenderWindow *window)
{
    Event event;
    while(window->pollEvent(event)) {
        if (event.type == Event::Closed) {
            window->close();
        }
        if (event.type == Event::Resized) {
            playerView.setSize(event.size.width, event.size.height);
        }
    }
}

void keyboardInput(RenderWindow *window)
{
    if(Keyboard::isKeyPressed(Keyboard::W)
       || Keyboard::isKeyPressed(Keyboard::Up)) {
        playerCar.move(true);
    }
    if(Keyboard::isKeyPressed(Keyboard::S)
       || Keyboard::isKeyPressed(Keyboard::Down)) {
        playerCar.move(false);
    }
    if(Keyboard::isKeyPressed(Keyboard::A)
       || Keyboard::isKeyPressed(Keyboard::Left)) {
        playerCar.turn(true);
    }
    if(Keyboard::isKeyPressed(Keyboard::D)
       || Keyboard::isKeyPressed(Keyboard::Right)) {
        playerCar.turn(false);
    }
}

void update(RenderWindow *window)
{
    playerCar.update();
    playerView.setCenter(playerCar.getPosition());
}

void simulatePhysics(RenderWindow *window)
{
    world.Step(box2DTimeStep, velocityIterations, positionIterations);
    world.ClearForces();
}

void draw(RenderWindow *window)
{
    window->clear(sf::Color(0,150,60,255));
    window->draw(tileMap);
    window->draw(playerCar);
    window->display();
}
