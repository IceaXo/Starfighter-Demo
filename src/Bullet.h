#pragma once

#include "raylib.h"
#include "Entity.h"
#include "Enemy.h"
#include <vector>
#include <math.h>

enum class BulletType{
    NORMAL, // 普通直线子弹
    HOMING  // 追踪导弹
};

class Bullet:public Entity{
public:
    using Entity::Update;

    BulletType type;

    int target_index;
    unsigned int target_id;

    float vx;
    float vy;
    
    Bullet(float startX,float startY):Entity(startX,startY)
    {
        speed = 10.0f; // 子弹比飞机快
        color = YELLOW;
        type = BulletType::NORMAL;

        target_index = -1;
        target_id = 0;

        vx = 0.0f;
        vy = -speed;
    }
    void Update(std::vector<Enemy>& enemyPool) {
        if(type == BulletType::HOMING){
            if(target_index>=0&&enemyPool[target_index].active&&target_id == enemyPool[target_index].unique_id){
                float Dx = enemyPool[target_index].x-this->x,Dy = enemyPool[target_index].y - this->y;
                float L = sqrt(Dx*Dx+Dy*Dy);
                if(L>0.0001f){
                    vx = (Dx/L)*speed;
                    vy = (Dy/L)*speed;
                }
            }
        }

        x+=vx;
        y+=vy;

        if (y < 0||x<0||x>GameConfig::SCREEN_WIDTH||y>GameConfig::SCREEN_HEIGHT) active = false; // 飞出屏幕外，标记为死亡
    }
    void Draw() override{
        DrawRectangle((int)x - 2, (int)y, 4, 10, color); // 画个黄色长条
    }
};

