#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
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

float toDegrees(float radians);
int toPixels(float position);
b2Vec2 rotate(b2Vec2 vector, float degrees);

class Wheels
{
public:
    Wheels(float startRotation, float maxRotation
           , float startSpeed, float frameFriction
           , float frameSteerFriction)
        {
            relRotation = startRotation;
            maxRelRotation = maxRotation;
            speed = startSpeed;
            friction = frameFriction;
            steerFriction = frameSteerFriction;
            turned = false;
        }

    void update()
        {
            speed *= friction;
            if (!turned)
                relRotation *= steerFriction;
            turned = false;
        }

    void addSpeed(float extra)
        {
            speed += extra;
        }

    void setSpeed(float newSpeed)
        {
            speed = newSpeed;
        }

    void turn(float rotation)
        {
            relRotation += rotation;
            relRotation = relRotation > maxRelRotation ?
                maxRelRotation : relRotation;
            relRotation = relRotation < (-1 * maxRelRotation) ?
                (-1 * maxRelRotation) : relRotation;
            turned = true;
        }

    b2Vec2 getForce(float rotation)
        {
            return b2Vec2(speed * sin(relRotation + rotation)
                          , -1 * speed * cos(relRotation + rotation));
        }

private:
    float relRotation, maxRelRotation, speed,  friction
        , steerFriction;
    bool turned;
};

Texture playerCarTex;
Sprite playerCar;
Texture tileMapTex;
groundTileMap tileMap;

View playerView(FloatRect(0, 0, 1920, 1080));

b2Vec2 gravity(0.0f, 0.0f);
b2World world(gravity);
b2Body *playerBody;

float box2DTimeStep = 1.0f / 60.0f;
int velocityIterations = 8;
int positionIterations = 3;

RectangleShape testBackgroundSquare(Vector2f(1920, 1080));

Wheels playercarWheels(0, (PI / 4), 0, 0.96, 0.95);

int main()
{
    b2Vec2 testVec = rotate(b2Vec2(0,1), PI / 6);
    std::cout << testVec.x << " " << testVec.y << "\n";
    RenderWindow window(VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Test");
    window.setVerticalSyncEnabled(true);
    window.setView(playerView);

    // Load textures
    if (!playerCarTex.loadFromFile("sprites/car_3.png")) return 0;
    if (!tileMapTex.loadFromFile("sprites/tiles_3.png")) return 0;
    
    // Set up car
    playerCar.setTexture(playerCarTex);
    playerCar.setOrigin(PLAYER_WIDTH / 2,PLAYER_HEIGHT / 2);

    // Set up groundTileMap
    tileMap.genGroundTileMap("sprites/tilesmap_5.pgm", tileMapTex, 100
                             , 100, 5, &world);

    // Box2D setup code
    b2BodyDef playerBodyDef;
    playerBodyDef.type = b2_dynamicBody;
    playerBodyDef.position.Set(2.0f, 2.0f);
    playerBodyDef.linearDamping = 2.4f;
    playerBodyDef.angularDamping = 5.0f;
    playerBody = world.CreateBody(&playerBodyDef);

    b2PolygonShape playerBox;
    playerBox.SetAsBox((float) (PLAYER_WIDTH) / ((float) (SCALE) * 2)
                       , (float) (PLAYER_HEIGHT) / ((float) (SCALE) * 2));

    b2FixtureDef playerFixtureDef;
    playerFixtureDef.shape = &playerBox;
    playerFixtureDef.density = 1.6f;
    playerFixtureDef.friction = 0.3f;

    playerBody->CreateFixture(&playerFixtureDef);

    testBackgroundSquare.setFillColor(Color::Green);

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
        // playerBody->ApplyForce(b2Vec2( -10 * cos(playerBody->GetAngle()
        //                                          + PI / 2 + PI * 2
        //                                    )
        //                              , -10 * sin(playerBody->GetAngle()
        //                                          + PI / 2 + PI * 2
        //                                  ))
        //                              , playerBody->GetWorldCenter(), true);
        playercarWheels.addSpeed(0.5);
    }
    if(Keyboard::isKeyPressed(Keyboard::S)
       || Keyboard::isKeyPressed(Keyboard::Down)) {
        // playerBody->ApplyForce(b2Vec2( 5 * cos(playerBody->GetAngle()
        //                                         + PI / 2 + PI * 2
        //                                    )
        //                              , 5 * sin(playerBody->GetAngle()
        //                                         + PI / 2 + PI * 2
        //                                  ))
        //                              , playerBody->GetWorldCenter(), true);
        playercarWheels.addSpeed(-0.5);
    }
    if(Keyboard::isKeyPressed(Keyboard::A)
       || Keyboard::isKeyPressed(Keyboard::Left)) {
        // playerBody->ApplyTorque(-5, true);
        playercarWheels.turn(-0.1);
    }
    if(Keyboard::isKeyPressed(Keyboard::D)
       || Keyboard::isKeyPressed(Keyboard::Right)) {
        // playerBody->ApplyTorque(5, true);
        playercarWheels.turn(0.1);
    }
}

void update(RenderWindow *window)
{
    playerView.setCenter(playerCar.getPosition());
    playercarWheels.update();
    // b2Vec2 debug = rotate(playercarWheels.getRelativeForce()
    //                       , 1 * playerBody->GetAngle() + PI / 2);
    // std::cout << (int) (debug.x) << " " << (int) (debug.y) << "\n"; 
    playerBody->ApplyForce(playercarWheels.getForce(playerBody->GetAngle())
                           , playerBody->GetWorldCenter()
                           + rotate(b2Vec2(0, -0.3),playerBody->GetAngle()), true);
}

void simulatePhysics(RenderWindow *window)
{
    world.Step(box2DTimeStep, velocityIterations, positionIterations);
    world.ClearForces();
    b2Vec2 playerPosition = playerBody->GetPosition();
    float playerAngle = playerBody->GetAngle();
    playerCar.setPosition(toPixels(playerPosition.x)
                       ,toPixels(playerPosition.y));
    playerCar.setRotation(toDegrees(playerAngle));
}

void draw(RenderWindow *window)
{
    window->clear();
    window->draw(testBackgroundSquare);
    window->draw(tileMap);
    window->draw(playerCar);
    window->display();
}

float toDegrees(float radians)
{
    return (radians / PI) * 180;
}

int toPixels(float position)
{
    return position * SCALE;
}

b2Vec2 rotate(b2Vec2 vector, float degrees)
{
    float x = vector.x * cos(degrees) - vector.y * sin(degrees);
    float y = vector.x * sin(degrees) + vector.y * cos(degrees);
    return b2Vec2(x, y);
}
