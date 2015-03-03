#include "car.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Box2D/Box2D.h>
#include <math.h>
#include <iostream>

#ifndef PI
 #define PI 3.14159265358979323846
#endif

using namespace sf;

Car::Car() { }

void Car::initialize(float nSpeedAdd, float nFriction, float nRotationSpeed
                     , float  nMaxRotation, float nRotationReturn 
                     , sf::Texture nTexture, b2Vec2 nSize, int nScale
                     , b2World *nWorld, b2Vec2 nPostion, bool nBox
                     , bool nSound)
{
    speedAdd = nSpeedAdd;
    friction = nFriction;
    rotationSpeed = nRotationSpeed;
    maxRotation = nMaxRotation;
    rotationReturn = nRotationReturn;
    texture = nTexture;
    size = nSize;
    scale = nScale;
    world = nWorld;
    box = nBox;
    sound = nSound;

    // Creating the Box2D body
    b2BodyDef carBodyDef;
    carBodyDef.type = b2_dynamicBody;
    carBodyDef.position.Set(nPostion.x / scale
                            , nPostion.y / scale);
    carBodyDef.linearDamping = 2.4f;
    carBodyDef.angularDamping = 3.0f;
    carBody = world->CreateBody(&carBodyDef);

    b2PolygonShape carBox;
    if (!box) {
        float proportionx = (float) (size.x) / (float) (scale)
            , proportiony = (float) (size.y) / (float) (scale);

        b2Vec2 vertices[8];
        vertices[0].Set(0.5 * proportionx, 0.0 * proportiony);
        vertices[1].Set(0.92045 * proportionx, 0.26667 * proportiony);
        vertices[2].Set(0.90909 * proportionx, 0.92045 * proportiony);
        vertices[3].Set(0.78409 * proportionx, 0.98 * proportiony);
        vertices[4].Set(0.5 * proportionx, 0.0 * proportiony);
        vertices[5].Set(0.21591 * proportionx, 0.98 * proportiony);
        vertices[6].Set(0.068182 * proportionx, 0.89333 * proportiony);
        vertices[7].Set(0.079545 * proportionx, 0.26667 * proportiony);

        carBox.Set(vertices, 8);
    } else { 
        carBox.SetAsBox((float) (size.x) / (float) (scale * 2)
                        , (float) (size.y) / (float) (scale * 2));
    }
    b2FixtureDef carFixtureDef;
    carFixtureDef.shape = &carBox;
    carFixtureDef.density = box ? 1.6 : 2.6f;
    carFixtureDef.friction = 1.0f;

    carBody->CreateFixture(&carFixtureDef);

    // Setting up the wheels
    leftFront.initialize(0, maxRotation, 0, friction, rotationReturn);
    leftBack.initialize(0, maxRotation, 0, friction, rotationReturn);
    rightFront.initialize(0, maxRotation, 0, friction, rotationReturn);
    rightBack.initialize(0, maxRotation, 0, friction, rotationReturn);

    // Setting up the sprite
    car.setTexture(texture);
    car.setOrigin(size.x / 2, size.y / 2);

    if (sound) {
        // Loading sounds
        bool error = 0;
        if (!engineBuf[0].loadFromFile("sounds/engine_1.wav")) error = 1;
        if (!engineBuf[1].loadFromFile("sounds/engine_2.wav")) error = 1;
        if (!engineBuf[2].loadFromFile("sounds/engine_3.wav")) error = 1;
        if (!engineBuf[3].loadFromFile("sounds/engine_4.wav")) error = 1;
        if (!engineBuf[4].loadFromFile("sounds/engine_5.wav")) error = 1;
        if (error) std::cout << "Error while loading engine sounds\n";
        engineSound.setLoop(true);
        soundLevel = 0;
    }
}

void Car::update()
{
    // Update the wheels
    leftFront.update();
    leftBack.update();
    rightFront.update();
    rightBack.update();

    // Applying wheel forces
    carBody->ApplyForce(leftFront.getForce(carBody->GetAngle())
                           , carBody->GetWorldCenter()
                           + rotateVec(b2Vec2(-0.2, -0.4)
                                       , carBody->GetAngle())
                           , true);
    carBody->ApplyForce(leftBack.getForce(carBody->GetAngle())
                           , carBody->GetWorldCenter()
                           + rotateVec(b2Vec2(-0.2, 0.4)
                                       , carBody->GetAngle())
                           , true);
    carBody->ApplyForce(rightFront.getForce(carBody->GetAngle())
                           , carBody->GetWorldCenter()
                           + rotateVec(b2Vec2(0.2, -0.4)
                                       , carBody->GetAngle())
                           , true);
    carBody->ApplyForce(rightBack.getForce(carBody->GetAngle())
                           , carBody->GetWorldCenter()
                           + rotateVec(b2Vec2(0.2, 0.4)
                                       , carBody->GetAngle())
                           , true);

    // Update sprite position and rotation
    b2Vec2 newPosition = carBody->GetPosition();
    float angle = carBody->GetAngle();
    b2Vec2 placeVec;
    if (!box) {
        placeVec = rotateVec(b2Vec2(size.x / 2, size.y / 2), angle);
    } else {
        placeVec = b2Vec2(0,0);
    }
    setPosition(toPixels(newPosition.x) + placeVec.x
                , toPixels(newPosition.y) + placeVec.y);
    setRotation(toDegrees(angle));

    if (sound) {
        // Update sounds
        int newLevel;
        float maxSpeed = (friction * speedAdd)
            / (1 - friction);
        float speed = leftFront.getSpeed()
            , fifth = maxSpeed / 5;
        if (speed < 0.1f) newLevel = 0;
        else if (speed < maxSpeed / 3) newLevel = 1;
        else if (speed < maxSpeed / 2) newLevel = 2;
        else if (speed < maxSpeed * 3 / 4) newLevel = 3;
        else if (speed < maxSpeed * 16 / 18) newLevel = 4;
        else if (speed < maxSpeed) newLevel = 5;

        if (newLevel != soundLevel) {
            std::cout << newLevel << "\n";
            engineSound.stop();
            if (newLevel != 0) {
                engineSound.setBuffer(engineBuf[newLevel-1]);
                engineSound.play();
            }
            soundLevel = newLevel;
        }
    }
}

void Car::move(bool forward)
{
    float newSpeed = leftFront.getSpeed();
    newSpeed += forward ? speedAdd : newSpeed > 0 ?
        -3 * speedAdd : -0.5 * speedAdd;
    leftFront.setSpeed(newSpeed);
    leftBack.setSpeed(newSpeed);
    rightFront.setSpeed(newSpeed);
    rightBack.setSpeed(newSpeed);
}

void Car::turn(bool left)
{
    float rotation = left ? -rotationSpeed : rotationSpeed;
    leftFront.turn(rotation);
    rightFront.turn(rotation);
}

void Car::draw(RenderTarget &target, RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(car, states);
}

float Car::toDegrees(float radians)
{
    return (radians / PI) * 180;
}

int Car::toPixels(float position)
{
    return position * scale;
}

b2Vec2 Car::rotateVec(b2Vec2 vector, float degrees)
{
    float x = vector.x * cos(degrees) - vector.y * sin(degrees);
    float y = vector.x * sin(degrees) + vector.y * cos(degrees);
    return b2Vec2(x, y);
}

Wheel::Wheel() { }

void Wheel::initialize(float startRotation, float maxRotation
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

void Wheel::update()
{
    speed *= friction;
    // if (!turned)
        relRotation *= steerFriction;
    turned = false;
}

float Wheel::getSpeed()
{
    return speed;
}

void Wheel::setSpeed(float newSpeed)
{
    speed = newSpeed;
}

void Wheel::turn(float rotation)
{
    relRotation += rotation;
    relRotation = relRotation > maxRelRotation ?
        maxRelRotation : relRotation;
    relRotation = relRotation < (-1 * maxRelRotation) ?
        (-1 * maxRelRotation) : relRotation;
    turned = true;
}

b2Vec2 Wheel::getForce(float rotation)
{
    return b2Vec2(speed * sin(relRotation + rotation)
                  , -1 * speed * cos(relRotation + rotation));
}
