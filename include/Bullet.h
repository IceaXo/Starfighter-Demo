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
        BeginBlendMode(BLEND_ADDITIVE);

        if (type == BulletType::NORMAL) {
            // 能量弹: 拉长的发光椭圆
            Color glow = YELLOW; glow.a = 80;
            Color zero = YELLOW; zero.a = 0;
            DrawCircleGradient((int)x, (int)y, 10.0f, glow, zero);
            DrawEllipse((int)x, (int)y, 3.5f, 10.0f, YELLOW);
            // 头部高亮
            DrawEllipse((int)x, (int)(y - 5.0f), 2.0f, 4.0f, WHITE);
        }
        else {
            // 追踪弹: 紫色旋转菱形 + 脉冲光环
            float time     = (float)GetTime();
            float rotSpeed = time * 20.0f + (float)target_index * 0.7f;
            float pulse    = 0.8f + 0.2f * sinf(time * 10.0f);

            // 外围脉冲
            Color aura = {255, 60, 255, 50};
            Color z    = {255, 60, 255, 0};
            DrawCircleGradient((int)x, (int)y, 14.0f * pulse, aura, z);

            // 拖尾 (3个缩小的历史位置)
            float trailVx = (type == BulletType::HOMING) ? vx : 0.0f;
            float trailVy = vy;
            float len = sqrtf(trailVx * trailVx + trailVy * trailVy) + 0.001f;
            float dx = trailVx / len * 4.0f;
            float dy = trailVy / len * 4.0f;
            for (int i = 1; i <= 3; i++) {
                Color tc = {255, 60, 255, (unsigned char)(150 / i)};
                float tx = x - dx * i;
                float ty = y - dy * i;
                float s = 1.0f - i * 0.25f;
                DrawRectanglePro(
                    {tx, ty, 8.0f * s, 8.0f * s},
                    {4.0f * s, 4.0f * s}, rotSpeed, tc);
            }

            // 主体: 旋转方形
            Color mainC = {255, 80, 255, 255};
            DrawRectanglePro(
                {x, y, 10.0f, 10.0f},
                {5.0f, 5.0f}, rotSpeed, mainC);

            // 内核
            DrawCircle((int)x, (int)y, 3.0f, WHITE);
        }

        EndBlendMode();
    }
};

