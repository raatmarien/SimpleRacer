#include "groundTileMap.h"
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <fstream>
#include <vector>
#include <iostream>

using namespace sf;

void groundTileMap::genGroundTileMap (const char* filename, Texture nTexture
                                      , int tilesW, int tilesH
                                      , int textureTileGridWidth
                                      , b2World *world, int nScale)
{
    texture = nTexture;
    tilesWidth = tilesW;
    tilesHeight = tilesH;
    SCALE = nScale;
    std::ifstream bitmap(filename);
    char buf[10];
    bitmap >> buf;
    bitmap >> width >> height;
    bitmap >> buf; // The range of the grayscale
                   // Not needed for reading for us
    vertices.setPrimitiveType(Quads);
    vertices.resize(4 * width * height);
    for (int i = 0; i < width; i++) {
        std::cout << i;
        if (i < 10) 
            std::cout << ' ';
    }
    std::cout << '\n';
    for (int y = 0; y < height; y++) {
            std::cout << y;
        for (int x = 0; x < width; x++) {
            int indexInVertexArray = 4 * (width * y + x), tileNum;
            bitmap >> tileNum; // The grayscale value
                               // of the corresponding pixel
            
            // Rotation
            int rotation = tileNum % 4;
            tileNum /= 4;

            if (tileNum == 7)
                wayPoints.push_back(Vector2f((x + 0.5) * tilesWidth
                                             , (y + 0.5) * tilesWidth));

            std::cout << getDisplayChar(tileNum) << " ";
            
            int cornerTextureX = (tileNum % textureTileGridWidth)
                * tilesWidth
                , cornerTextureY = (tileNum / textureTileGridWidth)
                * tilesHeight;
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

void groundTileMap::draw(RenderTarget& target, RenderStates states) const
{
    states.transform *= getTransform();
    states.texture = &texture;
    target.draw(vertices, states);
}

std::vector<Vector2f> groundTileMap::getWayPoints()
{
    return wayPoints;
}

bool isSolid(int tileNum)
{
    return tileNum == 9;
}

char getDisplayChar(int tileNum)
{
    switch (tileNum) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 7:
    case 6:
    case 5:
    case 0:
        return 'X';
        break;
    case 9:
        return 'O';
        break;
    default:
        return ' ';
    }
}
    
