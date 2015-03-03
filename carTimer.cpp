#include "carTimer.h"
#include "car.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

using namespace sf;


void carTimer::initialize(int nScale, Car *nCar
                          , std::vector<sf::Vector2f> nWaypoints
                          , int y, int x1, int x2
                          , int nTolerance, sf::Text nText)
{
    scale = nScale;
    car = nCar;
    waypoints = nWaypoints;
    lineY = y;
    lineX1 = x1;
    lineX2 = x2;
    tolerance = nTolerance;
    waypointsVisited.clear();
    waypointsVisited.resize(waypoints.size(), false);
    lapTime = seconds(0);
    text = nText;
}

void carTimer::setCar(Car* nCar)
{
    car = nCar;
}

void carTimer::setWayPoints(std::vector<sf::Vector2f> nWaypoints)
{
    waypoints = nWaypoints;
    waypointsVisited.clear();
    waypointsVisited.resize(waypoints.size(), false);
}

void carTimer::setStartLine(int y, int x1, int x2)
{
    lineY = y;
    lineX1 = x1;
    lineX2 = x2;
}

void carTimer::setTolerance(int nTolerance)
{
    tolerance = nTolerance;
}

void carTimer::start()
{
    clock.restart();
}

Time carTimer::getTime()
{
    return clock.getElapsedTime();
}

Time carTimer::getLapTime()
{
    return lapTime;
}

void carTimer::update()
{
    Vector2f carPos = car->getPosition();
    for (int i = 0; i < waypoints.size(); i++) {
        if (!waypointsVisited[i]) {
            float distance = sqrt(pow(carPos.x - waypoints[i].x, 2)
                                  + pow(carPos.y - waypoints[i].y, 2));
            if (distance < tolerance) {
                waypointsVisited[i] = true;
            }
            break;
        }
    }
    bool allVisited = true;
    for (int i = 0; i < waypointsVisited.size(); i++) {
        if (!waypointsVisited[i]) {
            allVisited = false;
            break;
        }
    }
    if (allVisited) {
        if (carPos.y < lineY && carPos.x > lineX1 && carPos.x < lineX2) {
            lapTime = clock.restart();
            if (lapTime < bestLap || bestLap.asSeconds() < 0.1f)
                bestLap = lapTime;
            for (int i = 0; i < waypoints.size(); i++) waypointsVisited[i]
                                                           = false;
        }
    }
    Time currentTime = clock.getElapsedTime();
    std::string currentLapTime = timeToString(currentTime.asSeconds());
    std::string prevLap = timeToString(lapTime.asSeconds());
    std::string bestLapStr = timeToString(bestLap.asSeconds());
    std::string displayStr = "Current:  " + currentLapTime + "\nPrevious: "
        + prevLap + "\nBest:     " + bestLapStr;
    text.setString(displayStr);
}

void carTimer::draw(RenderTarget& target, RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(text, states);
}

std::string carTimer::timeToString(float seconds)
{
    float decaSeconds = 10 * seconds;
    int decaSecondsRound = (int) decaSeconds;
    int secondsI = decaSecondsRound / 10;
    int decimalPoint = decaSecondsRound % 10;
    std::stringstream ss;
    ss << secondsI;
    std::string secondsS;
    ss >> secondsS;
    ss << "";
    ss.clear();
    ss << decimalPoint;
    std::string decimalPointS;
    ss >> decimalPointS;
    std::string timeStr = "";
    timeStr = secondsS + "." + decimalPointS;
    // std::cout << timeStr << '\n';
    return timeStr;
}
