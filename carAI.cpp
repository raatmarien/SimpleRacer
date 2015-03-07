#include "carAI.h"
#include "car.h"
#include "groundTileMap.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <math.h>
#include <iostream>

using namespace sf;

carAI::carAI() { }

void carAI::initialize(Car* nCar, groundTileMap* groundTileMap, int nTolerance
                       , float nAngleTolerance, Vector2f pos)
{
    car = nCar;
    tolerance = nTolerance;
    angleTolerance = nAngleTolerance;
    currentIndex = -1;
    gettingCloser = false;
    wayPoints = groundTileMap->getWayPoints();
    wayPointsVisited.resize(wayPoints.size(), false);
    Vector2f currentPos = pos;
    float minDistance = -1;
std::cout << currentPos.x << " " << currentPos.y << "\n";
    for (int i = 0; i < wayPoints.size(); i++) {
        std::cout << wayPoints[i].x << " " << wayPoints[i].y << "\n";
        float distance = getVectorLength(wayPoints[i] - currentPos);
        if ((distance < minDistance || minDistance < 0)
            && wayPoints[i].y < currentPos.y
            && wayPoints[i].x - 400 < currentPos.x) {
            currentIndex = i;
            minDistance = distance;
        }
    }
    currentWayPoint = wayPoints[currentIndex];
    wayPointsVisited[currentIndex] = true;
std::cout << currentIndex << '\n';
    std::cout << currentWayPoint.x << " " << currentWayPoint.y << "\n";
    prevDist = -1;
    car->update();
}

void carAI::update()
{
    Vector2f currentPos = car->getPosition();
    float distance = getVectorLength(currentPos - wayPoints[currentIndex]);
    if (distance > prevDist && prevDist > 0) {
        if (gettingCloser) {
            float minDistance = -1;
            bool stillLeft = false;
            while (!stillLeft) {
                for (int i = 0; i < wayPoints.size(); i++) {
                    if (!wayPointsVisited[i]) {
                        stillLeft = true;
                        float distance = getVectorLength(wayPoints[i] - currentPos);
                        if (distance < minDistance || minDistance < 0) {
                            currentIndex = i;
                            minDistance = distance;
                        }
                    }
                }
                currentWayPoint = wayPoints[currentIndex];
                wayPointsVisited[currentIndex] = true;
                if (!stillLeft) {
                    for (int i = 0; i < wayPointsVisited.size(); i++)
                        wayPointsVisited[i] = false;
                }
            }
            prevDist = 0;
            std::cout << currentWayPoint.x << " " << currentWayPoint.y << "\n";
        } else {
            prevDist = distance;
        }
        gettingCloser = false;
    } else {
         prevDist = distance + 2;
         gettingCloser = true;
    }
    float currentAngle = car->getRotation();
    Vector2f dif = currentWayPoint - currentPos;
    float angleNeeded = -atan(dif.x / dif.y);
    if (currentAngle > angleNeeded + angleTolerance
        || currentAngle + angleTolerance < angleNeeded) {
        car->turn(currentAngle > angleNeeded);
    // std::cout << currentPos.x << " " << currentPos.y << " ";
    // std::cout << currentWayPoint.x << " " << currentWayPoint.y << "\n";
    }
    std::cout << distance << "\t" << (currentAngle > angleNeeded) << "\n";
    car->move(true);
    car->update();
}

void carAI::draw(RenderTarget &target, RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(*car, states);
}

float getVectorLength(Vector2f v) {
    return sqrt((v.x * v.x) + (v.y * v.y));
}
