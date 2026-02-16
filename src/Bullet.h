#ifndef BULLET_H
#define BULLET_H

#include "raylib.h"
#include "Entity.h"

class Bullet:public Entity{
public:
    
    Bullet(float startX,float startY):Entity(startX,startY)
    {
        speed = 10.0f; // 子弹比飞机快
        color = YELLOW;
    }
    void Update() override{
        y -= speed; // 往上飞
        if (y < 0) active = false; // 飞出屏幕上方，标记为死亡
    }
    void Draw() override{
        DrawRectangle((int)x - 2, (int)y, 4, 10, color); // 画个黄色长条
    }
};

#endif