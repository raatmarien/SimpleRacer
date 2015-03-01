#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
bool isSolid(int tileNum);
char getDisplayChar(int tileNum);

class groundTileMap : public sf::Drawable, public sf::Transformable
{
public:
    void genGroundTileMap (const char* filename, sf::Texture nTexture
                           , int tilesW, int tilesH
                           , int textureTileGridWidth, b2World *world);

private:
    virtual void draw (sf::RenderTarget& target, sf::RenderStates states) const;
    int width, height;
    sf::VertexArray vertices;
    sf::Texture texture;
    int tilesWidth, tilesHeight;
};
