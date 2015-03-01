#include "car.h"
#include <SFML/Graphics.hpp>
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
                     , b2World *nWorld, b2Vec2 nPostion)
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

    // Creating the Box2D body
    b2BodyDef carBodyDef;
    carBodyDef.type = b2_dynamicBody;
    carBodyDef.position.Set(nPostion.x / scale
                            , nPostion.y / scale);
    carBodyDef.linearDamping = 2.4f;
    carBodyDef.angularDamping = 8.0f;
    carBody = world->CreateBody(&carBodyDef);

    b2PolygonShape carBox;
    carBox.SetAsBox((float) (size.x) / (float) (scale * 2)
                    , (float) (size.y) / (float) (scale * 2));

    b2FixtureDef carFixtureDef;
    carFixtureDef.shape = &carBox;
    carFixtureDef.density = 1.6f;
    carFixtureDef.friction = 0.3f;

    carBody->CreateFixture(&carFixtureDef);

    // Setting up the wheels
    leftFront.initialize(0, maxRotation, 0, friction, rotationReturn);
    leftBack.initialize(0, maxRotation, 0, friction, rotationReturn);
    rightFront.initialize(0, maxRotation, 0, friction, rotationReturn);
    rightBack.initialize(0, maxRotation, 0, friction, rotationReturn);

    // Setting up the sprite
    car.setTexture(texture);
    car.setOrigin(size.x / 2, size.y / 2);
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
    setPosition(toPixels(newPosition.x)
                    , toPixels(newPosition.y));
    setRotation(toDegrees(carBody->GetAngle()));
}

void Car::move(bool forward)
{
    float newSpeed = leftFront.getSpeed();
    newSpeed += forward ? speedAdd : -0.5 * speedAdd;
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
    if (!turned)
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
