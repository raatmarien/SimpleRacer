#include "car.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>

class carTimer : public sf::Drawable, public sf::Transformable
{
public:
    void initialize(int nScale, Car *nCar
                    , std::vector<sf::Vector2f> nWaypoints
                    , int y, int x1, int x2
                    , int nTolerance, sf::Text nText);
    void setCar(Car *nCar);
void setWayPoints(std::vector <sf::Vector2f> nWaypoints);
    void setStartLine(int y, int x1, int x2);
    void setTolerance(int nTolerance);
    void start();
    void update();
    sf::Time getTime();
    sf::Time getLapTime();
private:
    Car *car;
    sf::Clock clock;
    sf::Time lapTime, bestLap;
    int lineY, lineX1, lineX2, tolerance;
std::vector<sf::Vector2f> waypoints;
std::vector<bool> waypointsVisited;
    int scale;
    sf::Text text;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    std::string timeToString(float seconds);
};
