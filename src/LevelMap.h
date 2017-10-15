#ifndef _LEVEL_MAP_H
#define _LEVEL_MAP_H

#include "TerrainTypes.h"
#include "TextureLoader.h"
#include "Xml.h"

struct Tile{
    COLOR c;
    unsigned picture;

    bool affected;

    bool animated;
    unsigned frame;
    unsigned animtics;

    Tile(){picture = 0;
           frame = 0;
           affected = false;
           animtics = 0;
           animated = false;}
};

struct LevelMap{
    
    Tile ** map;
    bool ** bmap;
    unsigned width;
    unsigned height;
    unsigned backgroundPicIndex;
    char musicPath[255];
    
    LevelMap(){
        map = 0;
        width = 0;
        height = 0;
        backgroundPicIndex = 13;
    }
    void destroy();
    void generate(unsigned _width = 0, unsigned _height = 0);
    bool ** boolean();//updates bmap and returns pointer
    void boolean(bool *** _map); //creates external boolean map
    void loadFromXMLNode(XmlNode * node);
    void putToXMLNode(XmlNode * node);
    void draw(PicsContainer& pics, unsigned index, int x, int y, 
              float c = 1.0f, int screenWidth = 480,
              int screenHeight = 320);
    int  getTerrainType(unsigned x, unsigned y) const;
    void setTerrainType(unsigned x, unsigned y, TerrainTypes t);
    void setTile(unsigned x, unsigned y, Tile & t);
    void animateTiles();
    void affectTile(unsigned x, unsigned y);
};

#endif //_LEVEL_MAP_H

