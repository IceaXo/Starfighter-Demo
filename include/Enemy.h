#pragma once

#include "raylib.h"
#include "Entity.h"
#include "Config.h"

class Enemy : public Entity {
public:
    using Entity::Update;
    // 1. 敌人的个人身份证（每个敌人对象各自拥有一份）
    unsigned unique_id;

    // 2. 宗门执事手里的发号器！（static 关键字的无上伟力）
    // static 意味着这个变量不属于某一个具体的敌人，而是属于整个 Enemy 家族共享的！
    static unsigned int next_id;
    // 构造函数
    Enemy(float startX, float startY) : Entity(startX, startY) {
        speed = 3.0f;   
        radius = 30.0f; // 敌人的碰撞半径
        color = RED;    // 敌人是红色的
        unique_id = 0;
    }

    // 每一帧往下飞
    void Update() override {
        y += speed; 
        if (y > GameConfig::SCREEN_HEIGHT+2*radius) active = false; // 飞出底部就标记死亡
    }
    void Draw() override{
        BeginBlendMode(BLEND_ADDITIVE);
        DrawTriangleLines({x,y+radius},{x-radius,y-radius},{x+radius,y-radius},color);
        EndBlendMode();
    }
};
