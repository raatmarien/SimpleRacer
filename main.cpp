#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
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
bool isSolid(int tileNum);

class groundTileMap : public sf::Drawable, public sf::Transformable
{
public:
    groundTileMap() { }
    void genGroundTileMap (const char* filename, Texture nTexture, int tilesW, int tilesH
                           , int textureTileGridWidth, b2World *world)
        {
            texture = nTexture;
            tilesWidth = tilesW;
            tilesHeight = tilesH;
            std::ifstream bitmap(filename);
            char buf[10];
            bitmap >> buf;
            bitmap >> width >> height;
            bitmap >> buf; // The range of the grayscale
                           // Not needed for reading for us
            vertices.setPrimitiveType(Quads);
            vertices.resize(4 * width * height);
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    int indexInVertexArray = 4 * (width * y + x), tileNum;
                    bitmap >> tileNum; // The grayscale value
                                       // of the corresponding pixel
                    
                    // Rotation
                    int rotation = tileNum % 4;
                    tileNum /= 4;

                    std::cout << tileNum << " ";
                    
                    int cornerTextureX = (tileNum % textureTileGridWidth) * tilesWidth
                        , cornerTextureY = (tileNum / textureTileGridWidth) * tilesHeight;
                    // Top left corner
                    vertices[indexInVertexArray+0].position
                        = Vector2f(x * tilesWidth
                                   , y * tilesHeight);
                    // Top right corner
                    vertices[indexInVertexArray+1].position
                        = Vector2f((x + 1) * tilesWidth
                                   , y * tilesHeight);
                    // Bottom right corner
                    vertices[indexInVertexArray+2].position
                        = Vector2f((x + 1) * tilesWidth
                                   , (y + 1) * tilesHeight);
                    // Bottom left corner
                    vertices[indexInVertexArray+3].position
                        = Vector2f(x * tilesWidth
                                   , (y + 1) * tilesHeight);

                    int turnArray[4] = {0,1,2,3};
                    if (rotation == 1) {
                        turnArray[0] = 1;
                        turnArray[1] = 2;
                        turnArray[2] = 3;
                        turnArray[3] = 0;
                    } else if (rotation == 2) {
                        turnArray[0] = 2;
                        turnArray[1] = 3;
                        turnArray[2] = 0;
                        turnArray[3] = 1;
                    } else if (rotation == 3) {
                        turnArray[0] = 3;
                        turnArray[1] = 0;
                        turnArray[2] = 1;
                        turnArray[3] = 2;
                    }
                    // Texture Coords
                    // Top left corner
                    vertices[indexInVertexArray+turnArray[0]].texCoords
                        = Vector2f(cornerTextureX
                                   , cornerTextureY);
                    // Top right corner
                    vertices[indexInVertexArray+turnArray[1]].texCoords
                        = Vector2f(cornerTextureX + tilesWidth
                                   , cornerTextureY);
                    // Bottom right corner
                    vertices[indexInVertexArray+turnArray[2]].texCoords
                        = Vector2f(cornerTextureX + tilesWidth
                                   , cornerTextureY + tilesHeight);
                    // Bottom left corner
                    vertices[indexInVertexArray+turnArray[3]].texCoords
                        = Vector2f(cornerTextureX
                                   , cornerTextureY + tilesHeight);

                    
                    // Adding a physical box if necessary
                    if (isSolid(tileNum)) {
                        b2BodyDef boxBodyDef;
                        boxBodyDef.position.Set((float)((float)(x + 0.5) * tilesWidth)
                                             / (float) (SCALE)
                                             , (float)((float)(y + 0.5) * tilesWidth)
                                             / (float) (SCALE));
                        b2Body *boxBody = world->CreateBody(&boxBodyDef);

                        b2PolygonShape boxBodyShape;
                        boxBodyShape.SetAsBox((float)(tilesWidth) / (2 * SCALE)
                                              , (float)(tilesHeight) / (2 * SCALE));
                        boxBody->CreateFixture(&boxBodyShape, 0.0f);
                    }
                }
                std::cout << "\n"; // debug
            }
        }

private:
    virtual void draw (RenderTarget& target, RenderStates states) const
        {
            states.transform *= getTransform();
            states.texture = &texture;
            target.draw(vertices, states);
        }
    int width, height;
    VertexArray vertices;
    Texture texture;
    int tilesWidth, tilesHeight;
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

int main()
{
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
    playerBodyDef.linearDamping = 0.4f;
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
        playerBody->ApplyForce(b2Vec2( -10 * cos(playerBody->GetAngle()
                                                 + PI / 2 + PI * 2
                                           )
                                     , -10 * sin(playerBody->GetAngle()
                                                 + PI / 2 + PI * 2
                                         ))
                                     , playerBody->GetWorldCenter(), true);
    }
    if(Keyboard::isKeyPressed(Keyboard::S)
       || Keyboard::isKeyPressed(Keyboard::Down)) {
        playerBody->ApplyForce(b2Vec2( 5 * cos(playerBody->GetAngle()
                                                + PI / 2 + PI * 2
                                           )
                                     , 5 * sin(playerBody->GetAngle()
                                                + PI / 2 + PI * 2
                                         ))
                                     , playerBody->GetWorldCenter(), true);
    }
    if(Keyboard::isKeyPressed(Keyboard::A)
       || Keyboard::isKeyPressed(Keyboard::Left)) {
        playerBody->ApplyTorque(-5, true);
        // playerBody->ApplyForce(b2Vec2(-10,0) player
    }
    if(Keyboard::isKeyPressed(Keyboard::D)
       || Keyboard::isKeyPressed(Keyboard::Right)) {
        playerBody->ApplyTorque(5, true);
    }
}

void update(RenderWindow *window)
{
    playerView.setCenter(playerCar.getPosition());
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

bool isSolid(int tileNum) {
    return tileNum == 9;
}
