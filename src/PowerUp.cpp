#include "PowerUp.h"

void PowerUp::Render(PicsContainer& pics, unsigned pic)
{
    pics.draw(pic, pos.v[0], pos.v[1], type, true);
}
