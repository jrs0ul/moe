
#include <cmath>
#include <wchar.h>
#include "LevelMap.h"

void LevelMap::destroy(){
    for (unsigned i = 0; i < height; i++)
        delete []map[i];
    delete []map;
    
    if (bmap){
        for (unsigned i = 0; i < height; i++)
            free(bmap[i]);
        free(bmap);
        bmap = 0;
    }
    
    map = 0;
    width = 0;
    height = 0;
}
//------------------
void LevelMap::generate(unsigned _width, unsigned _height){
    if ((!_width) && (!_height)){
        width = rand() % 7 + 9;
        if (!(width % 2))
            width++;
        height = width;
    }
    else{
        width = _width;
        if (!width) width = height;
        height = _height;
        if (!height) height = width;
    }

    map = new Tile*[height];
    for (unsigned i = 0; i < height; i++){
        map[i] = new Tile[width];
        for (unsigned a = 0; a < width; a++){
            unsigned pic = (rand() % 4);
            map[i][a].picture = (pic > 2) ? pic + 3 : pic;
            map[i][a].animated = (pic == 2) ? true : false;
            map[i][a].c = COLOR(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }
    
}
//-------------------
bool ** LevelMap::boolean(){
    
    if (bmap){
        for (unsigned i = 0; i < height; i++)
            free(bmap[i]);
        free(bmap);
        bmap = 0;
    }
    bmap = (bool **)malloc( height * sizeof(bool*));
    for (unsigned i = 0; i < height; i++){
        bmap[i] = (bool*)malloc(width * sizeof(bool));
        for (unsigned a = 0; a < width; a++){
            bmap[i][a] = (map[i][a].picture == 0) ? false : true;
        }
    }
            
    return bmap;
}
//-------------------------
void LevelMap::boolean(bool *** _map){
    *_map = (bool **)malloc( height * sizeof(bool*));
    for (unsigned i = 0; i < height; i++){
        (*_map)[i] = (bool*)malloc(width * sizeof(bool));
        for (unsigned a = 0; a < width; a++){
            (*_map)[i][a] = (map[i][a].picture == 0) ? false : true;
        }
    }
}

//-------------------
void LevelMap::loadFromXMLNode(XmlNode * node){
    char buf[100];
    wcstombs(buf, node->getAttribute(0)->getValue(), 100);//width
    width = atoi(buf);
    wcstombs(buf, node->getAttribute(1)->getValue(), 100);//height
    height = atoi(buf);
    XmlAttribute * picIndex = 0;
    picIndex = node->getAttribute(2);
    if (picIndex){
        wcstombs(buf, picIndex->getValue(), 100);
        backgroundPicIndex = (unsigned)atoi(buf);
    }
    
    XmlAttribute * songPath = 0;
    songPath = node->getAttribute(3);
    if (songPath){
        wcstombs(buf, songPath->getValue(), 100);
        strncpy(musicPath, buf, 100);
    }
    
    
    map = new Tile*[height];
    
    for (unsigned i = 0; i < height; i++){
        XmlNode * row = 0;
        row = node->getNode(i);
        map[i] = new Tile[width];
        for (unsigned long a = 0; a < row->childrenCount(); a++){
            XmlNode * tile = 0;
            tile = row->getNode(a);
            
            wcstombs(buf, tile->getAttribute(0)->getValue(), 100);
            map[i][a].picture = atoi(buf);
            
            wcstombs(buf, tile->getAttribute(1)->getValue(), 100);
            map[i][a].c.c[0] = atof(buf);
           
            wcstombs(buf, tile->getAttribute(2)->getValue(), 100);
            map[i][a].c.c[1] = atof(buf);
            
            wcstombs(buf, tile->getAttribute(3)->getValue(), 100);
            map[i][a].c.c[2] = atof(buf);
            map[i][a].c.c[3] = 1.0f;
        }
    }
    
}
//--------------------
void LevelMap::putToXMLNode(XmlNode * node){
    
    node->setName(L"Tiles");
    wchar_t buf[100];
    swprintf(buf,
    #ifndef __MINGW32__ 
            100,
    #endif
    L"%u", width);
    node->addAtribute(L"width", buf);
    swprintf(buf,
    #ifndef __MINGW32__ 
            100,
    #endif
            L"%u", height);
    node->addAtribute(L"height", buf);
    swprintf(buf,
    #ifndef __MINGW32__ 
            100,
    #endif
            L"%u", backgroundPicIndex);
    node->addAtribute(L"bgPicIndex", buf);
    
    swprintf(buf,
#ifndef __MINGW32__ 
             100,
#endif
             L"%s", musicPath);
    node->addAtribute(L"music", buf);
    
    
    for (unsigned i = 0; i < height; i++){
        XmlNode row;
        row.setName(L"r");
        for (unsigned a = 0; a < width; a++) {
            XmlNode tile;
            tile.setName(L"t");
            swprintf(buf,
            #ifndef __MINGW32__ 
                    100,
            #endif
                    L"%u", map[i][a].picture);
            tile.addAtribute(L"i", buf);
            swprintf(buf,
            #ifndef __MINGW32__ 
                    100,
            #endif
                    L"%.3f", map[i][a].c.c[0]);
            tile.addAtribute(L"r", buf);
            swprintf(buf,
            #ifndef __MINGW32__ 
                    100,
            #endif
                    L"%.3f", map[i][a].c.c[1]);
            tile.addAtribute(L"g", buf);
            swprintf(buf,
            #ifndef __MINGW32__ 
                    100,
            #endif
                    L"%.3f", map[i][a].c.c[2]);
            tile.addAtribute(L"b", buf);
            
            tile.setValue(L"");
            
            row.addChild(tile);
        }
        node->addChild(row);
    }
    
}
//--------------------
void LevelMap::draw(PicsContainer& pics, unsigned index, int x, int y,
                    float c, int screenWidth, int screenHeight){
    for (unsigned i = 0; i < height; i++)
        for (unsigned a = 0; a < width; a++){
                COLOR tcolor = map[i][a].c;
                float _x = x + a * 32.0f;
                float _y = y + i * 32.0f;
                
                if ((_x > -32) && (_x < screenWidth)
                    && (_y > -32) && (_y < screenHeight) && (c > 0.0f))
                    pics.draw(index, _x, _y ,
                              map[i][a].picture + map[i][a].frame, false, 1.0f, 1.0f, 0.0f,
                              COLOR(tcolor.c[0] * c, tcolor.c[1] * c, tcolor.c[2] * c), 
                              COLOR(tcolor.c[0] * c, tcolor.c[1] * c, tcolor.c[2] * c));
            }
}
//-----------------------
void LevelMap::setTile(unsigned x, unsigned y, Tile & t){

    if ((x < width) && (y < height)){
        map[y][x].c = t.c;
        map[y][x].picture = t.picture;
    }

}
//-----------------------------
void LevelMap::animateTiles(){
    
    for (unsigned i = 0; i < height; i++){
        for (unsigned a = 0; a < width; a++){
            if (map[i][a].animated){
                map[i][a].animtics++;
                if (map[i][a].animtics > 15){
                    map[i][a].animtics = 0;
                    map[i][a].frame++;
                    if (map[i][a].frame > 3)
                        map[i][a].frame = 0;
                }
            }
        }
    }

}
//---------------------------
void LevelMap::affectTile(unsigned x, unsigned y){

    if ((x < width)&&(y < height)){
        int TerrainType = map[y][x].picture;
       // printf("pic %d ", TerrainType);
        if (map[y][x].picture > 2){
            if (map[y][x].picture == 6)
                TerrainType = 3;
            if (map[y][x].picture > 6)
                TerrainType = 4;

            if (map[y][x].picture < 6)
                TerrainType = 2;
        }

        switch(TerrainType){
            case 0: {
                map[y][x].picture = 7;
                map[y][x].animated = true;
                map[y][x].frame = 0;
                //puts("lava");
            }break;
            case 1: {
                map[y][x].picture = 7;
                map[y][x].animated = true;
                map[y][x].frame = 0;
                //puts("lava");
            }break;
            case 2: {
                map[y][x].picture = 0;
                map[y][x].animated = false;
                map[y][x].frame = 0;
                //puts("ground");
            }break;
            case 3: {
                map[y][x].picture = 2;
                map[y][x].animated = true;
                map[y][x].frame = 0;
                map[y][x].frame = 0;
                //puts("water");
            }break;
            case 4: {
                map[y][x].picture = 7;
                map[y][x].animated = true;
                map[y][x].frame = 0;
                //puts("fire");
            }break;
        }

        
        map[y][x].affected = true;
    }

}
