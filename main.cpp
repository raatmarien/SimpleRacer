#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "car.h"
#include "groundTileMap.h"
#include "carTimer.h"
#include <math.h>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define SCALE 50
#define PI 3.14159265358979323846
#define PLAYER_WIDTH 44 
#define PLAYER_HEIGHT 75

using namespace sf;

void handleEvents(RenderWindow *window);
void keyboardInput(RenderWindow *window);
void twoPlayerKeyboardInput(RenderWindow *window);
void update(RenderWindow *window);
void simulatePhysics(RenderWindow *window);
void draw(RenderWindow *window);

const bool boxCars = false
    , twoPlayers = 0
    , sound = 0;

Texture playerCarTex;
Car playerCar, playerTwoCar;
Texture tileMapTex;
groundTileMap tileMap;

carTimer playerCarTimer, playerTwoCarTimer;

View playerView(FloatRect(0, 0, 1920, 1080))
    , playerTwoView(FloatRect(0,0,960,540));

RectangleShape border(Vector2f(5,playerView.getSize().y));

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
    if (twoPlayers) {
        playerView.setSize(960,540);
        playerView.setViewport(FloatRect(0.0f,0.0f,0.5f,1.0f));
        playerTwoView.setViewport(FloatRect(0.5f,0.0f,0.5f,1.0f));
        window.setView(playerTwoView);
    }

    // Load textures and fonts
    if (!playerCarTex.loadFromFile("sprites/smaller_car_1.png")) return 0;
    if (!tileMapTex.loadFromFile("sprites/smaller_tiles_1.png")) return 0;
    Font timerFont;
    if (!timerFont.loadFromFile("sprites/UbuntuMono-B.ttf")) return 0;
    
    // Set up car(s)
    playerCar.initialize(0.3f, 0.987f, 0.03f, (PI / 4), 0.9f, playerCarTex
                         , b2Vec2(PLAYER_WIDTH, PLAYER_HEIGHT), SCALE
                         , &world, b2Vec2(350,4000), boxCars, sound);
    if (twoPlayers) {
        playerTwoCar.initialize(0.3f, 0.987f, 0.03f, (PI / 4), 0.9f, playerCarTex
                             , b2Vec2(PLAYER_WIDTH, PLAYER_HEIGHT), SCALE
                                , &world, b2Vec2(250,4000), boxCars, sound);
    }

    // Set up carTimer
    int tS = 50; // Tilesize
    // Set up waypoints
    std::vector<Vector2f> wayPoints;
    wayPoints.push_back(Vector2f(5 * tS, 42 * tS));
    wayPoints.push_back(Vector2f(21 * tS, 5 * tS));
    wayPoints.push_back(Vector2f(27 * tS, 55 * tS));
    wayPoints.push_back(Vector2f(26 * tS, 79 * tS));
    wayPoints.push_back(Vector2f(26 * tS, 100 * tS));
    // Set up Text object
    Text timerText;
    timerText.setFont(timerFont);
    timerText.setCharacterSize(24);
    timerText.setColor(Color::White);
    playerCarTimer.initialize(SCALE, &playerCar, wayPoints, 4000, 0, 1000
                              , 700, timerText);
    if (twoPlayers) {
        playerTwoCarTimer.initialize(SCALE, &playerTwoCar, wayPoints, 4000, 0, 1000
                                     , 700, timerText);
    }

    // Set up groundTileMap
    tileMap.genGroundTileMap("sprites/big_tilesmap_3.pgm", tileMapTex, 50
                             , 50, 5, &world, SCALE);

    playerCarTimer.start();
    while(window.isOpen()) {
        if (twoPlayers) {
            twoPlayerKeyboardInput(&window);
        } else {
            keyboardInput(&window);
        }
        handleEvents(&window);
        simulatePhysics(&window);
        update(&window);
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
            if (twoPlayers) {
                playerView.setSize(event.size.width / 2, event.size.height);
                playerTwoView.setSize(event.size.width / 2, event.size.height);
            } else {
                playerView.setSize(event.size.width, event.size.height);
            }
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

void twoPlayerKeyboardInput(RenderWindow *window)
{
    if(Keyboard::isKeyPressed(Keyboard::W)) {
        playerCar.move(true);
    }
    if(Keyboard::isKeyPressed(Keyboard::S)) {
        playerCar.move(false);
    }
    if(Keyboard::isKeyPressed(Keyboard::A)) {
        playerCar.turn(true);
    }
    if(Keyboard::isKeyPressed(Keyboard::D)) {
        playerCar.turn(false);
    }
    if(Keyboard::isKeyPressed(Keyboard::Up)) {
        playerTwoCar.move(true);
    }
    if(Keyboard::isKeyPressed(Keyboard::Down)) {
        playerTwoCar.move(false);
    }
    if(Keyboard::isKeyPressed(Keyboard::Left)) {
        playerTwoCar.turn(true);
    }
    if(Keyboard::isKeyPressed(Keyboard::Right)) {
        playerTwoCar.turn(false);
    }
}

void update(RenderWindow *window)
{
    playerCar.update();
    playerCarTimer.update();
    playerView.setCenter(playerCar.getPosition());
    playerCarTimer.setPosition(playerCar.getPosition()
                               - (Vector2f(playerView.getSize().x / 2
                                           , playerView.getSize().y / 2))
                               + Vector2f(20,20));
    if (twoPlayers) {
        playerTwoView.setCenter(playerTwoCar.getPosition());
        playerTwoCarTimer.setPosition(playerTwoCar.getPosition()
                                      - (Vector2f(playerTwoView.getSize().x / 2
                                                  , playerTwoView.getSize().y / 2))
                                      + Vector2f(20,20));
        playerTwoCar.update();
        playerTwoCarTimer.update();
    }
}

void simulatePhysics(RenderWindow *window)
{
    world.Step(box2DTimeStep, velocityIterations, positionIterations);
    world.ClearForces();
}

void draw(RenderWindow *window)
{
    window->setView(playerView);
    window->clear(sf::Color(0,150,60,255));
    window->draw(tileMap);
    window->draw(playerCar);
    window->draw(playerCarTimer);
    if (twoPlayers) {
        window->draw(playerTwoCar);
        border.setSize(Vector2f(3, playerView.getSize().y));
        border.setFillColor(Color::Black);
        border.setOrigin(0,0);
        border.setPosition(playerView.getCenter().x
                           + (playerView.getSize().x / 2) - 3
                           , playerView.getCenter().y
                           - (playerView.getSize().y / 2));
        window->draw(border);

        window->setView(playerTwoView);
        window->draw(tileMap);
        window->draw(playerCar);
        window->draw(playerTwoCar);
        window->draw(playerTwoCarTimer);
        border.setPosition(playerTwoView.getCenter().x
                           - (playerTwoView.getSize().x / 2)
                           , playerTwoView.getCenter().y
                           - (playerTwoView.getSize().y / 2));
        window->draw(border);
    }
    window->display();
}
