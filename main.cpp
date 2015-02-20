#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <math.h>
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

RectangleShape square(Vector2f(150,100));
RectangleShape testGround(Vector2f(100,600));
Texture playerCarTex;
Sprite playerCar;

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

	// Load texture
	if (!playerCarTex.loadFromFile("sprites/car_3.png")) return 0;
	
	playerCar.setTexture(playerCarTex);
	playerCar.setOrigin(PLAYER_WIDTH / 2,PLAYER_HEIGHT / 2);

	// Box2D setup code
	b2BodyDef playerBodyDef;
	playerBodyDef.type = b2_dynamicBody;
	playerBodyDef.position.Set(2.0f, 2.0f);
	playerBodyDef.linearDamping = 1.0f;
	playerBodyDef.angularDamping = 3.0f;
	playerBody = world.CreateBody(&playerBodyDef);

	b2PolygonShape playerBox;
	playerBox.SetAsBox((float) (PLAYER_WIDTH) / ((float) (SCALE) * 2)
					   , (float) (PLAYER_HEIGHT) / ((float) (SCALE) * 2));

	b2FixtureDef playerFixtureDef;
	playerFixtureDef.shape = &playerBox;
	playerFixtureDef.density = 1.6f;
	playerFixtureDef.friction = 0.3f;

	playerBody->CreateFixture(&playerFixtureDef);

	// Test groundBox
	b2BodyDef testGroundBodyDef;
	testGroundBodyDef.position.Set(5.0f, 5.0f);
	b2Body *testGroundBody = world.CreateBody(&testGroundBodyDef);

	b2PolygonShape testGroundBox;
	testGroundBox.SetAsBox(0.5f, 3.0f);
	testGroundBody->CreateFixture(&testGroundBox, 0.0f);

	testGround.setPosition(500,500);
	testGround.setOrigin(50,300);

	while(window.isOpen()) {
		keyboardInput(&window);
		handleEvents(&window);
		update(&window);
		simulatePhysics(&window);
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
			FloatRect visible(0,0, event.size.width, event.size.height);
			window->setView(View(visible));
		}
	}
}

void keyboardInput(RenderWindow *window)
{
	if(Keyboard::isKeyPressed(Keyboard::W)
	   || Keyboard::isKeyPressed(Keyboard::Up)) {
		playerBody->ApplyForce(b2Vec2( -20 * cos(playerBody->GetAngle()
												 + PI / 2 + PI * 2
										   )
								     , -20 * sin(playerBody->GetAngle()
												 + PI / 2 + PI * 2
										 ))
							         , playerBody->GetWorldCenter(), true);
	}
	if(Keyboard::isKeyPressed(Keyboard::S)
	   || Keyboard::isKeyPressed(Keyboard::Down)) {
		playerBody->ApplyForce(b2Vec2( 10 * cos(playerBody->GetAngle()
												+ PI / 2 + PI * 2
										   )
								     , 10 * sin(playerBody->GetAngle()
												+ PI / 2 + PI * 2
										 ))
							         , playerBody->GetWorldCenter(), true);
	}
	if(Keyboard::isKeyPressed(Keyboard::A)
	   || Keyboard::isKeyPressed(Keyboard::Left)) {
		playerBody->ApplyTorque(-10, true);
	}
	if(Keyboard::isKeyPressed(Keyboard::D)
	   || Keyboard::isKeyPressed(Keyboard::Right)) {
		playerBody->ApplyTorque(10, true);
	}
}

void update(RenderWindow *window)
{
	// std::cout << playerBody->GetPosition().x << " "
	// 		  << playerBody->GetPosition().y << "\n";
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
	window->draw(playerCar);
	window->draw(testGround);
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
