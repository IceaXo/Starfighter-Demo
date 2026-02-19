#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "Entity.h"
#include "Config.h"

class Enemy : public Entity {
public:
    // 构造函数
    Enemy(float startX, float startY) : Entity(startX, startY) {
        speed = 2.0f;   
        radius = 20.0f; // 敌人的碰撞半径
        color = RED;    // 敌人是红色的
    }

    // 每一帧往下飞
    void Update() override {
        y += speed; 
        if (y > GameConfig::SCREEN_HEIGHT+2*radius) active = false; // 飞出底部就标记死亡
    }
};

#endif