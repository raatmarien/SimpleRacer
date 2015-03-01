#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

class Wheel
{
public:
    Wheel();
    void initialize(float startRotation, float maxRotation
                    , float startSpeed, float frameFriction
                    , float frameSteerFriction);
    void update();
    float getSpeed();
    void setSpeed(float newSpeed);
    void turn(float rotation);
    b2Vec2 getForce(float rotation);

private:
    float relRotation, maxRelRotation, speed,  friction
        , steerFriction;
    bool turned;
};

class Car : public sf::Drawable, public sf::Transformable
{
public:
    Car();
    void initialize(float nSpeedAdd, float nFriction, float nRotationSpeed
                    , float  nMaxRotation, float nRotationReturn
                    , sf::Texture nTexture, b2Vec2 nSize, int nScale, b2World *nWorld
                    , b2Vec2 nPosition);
    void update();
    void move(bool forward);
    void turn(bool left);

private: 
    float speedAdd, friction, rotationSpeed, maxRotation
        , rotationReturn;
    int scale;
    b2Vec2 size;
    Wheel leftFront, rightFront, leftBack, rightBack;
    sf::Texture texture;
    b2Body *carBody;
    sf::Sprite car;
    b2World *world;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    float toDegrees(float radians);
    int toPixels(float position);
    b2Vec2 rotateVec(b2Vec2 vector, float degrees);
};
