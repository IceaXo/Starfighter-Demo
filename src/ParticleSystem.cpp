#include "ParticleSystem.h"
#include <cmath> // 数学库，提供 sin 和 cos

// 初始化对象池
ParticleSystem::ParticleSystem(int size){
    poolSize = size;
    pool.reserve(poolSize);
    for (int i = 0; i < poolSize; i++) {
        Particle p;
        p.active = false;
        pool.push_back(p);
    }
}

// --- [数学核心：爆炸发射] ---
void ParticleSystem::Emit(float startX,float startY,int count,Color baseColor){
    int emitted = 0;
    
    // 遍历池子找死粒子来复活
    for (auto& p:pool){
        if(!p.active){
            p.active = true;
            p.x = startX;
            p.y = startY;
            p.color = baseColor;

            // 寿命随机：0.5秒 到 1.5秒之间
            p.maxLife = (float)GetRandomValue(5,15)/10.0f;
            p.life = p.maxLife;

            // 【新增】强制把线变细，展现锐利感
            p.baseRadius = (float)GetRandomValue(1, 3);

            // ==========================================
            // [数学魔法：极坐标转笛卡尔坐标]
            // 面试官问：怎么让粒子360度随机炸开？
            // 答：生成一个随机角度，然后用 cos 算 X 速度，sin 算 Y 速度。
            // ==========================================

            // 1. 随机一个角度 (0 到 360度)，并转成弧度 (乘以 PI / 180)
            float angle = (float)GetRandomValue(0,360)*(PI/180.0f);

            // 2. 随机一个爆炸力度 (速度大小)
            float speed = (float)GetRandomValue(200,800);

            // 3. 极坐标转换公式 (初中数学)
            // vx = 速度 * cos(角度)
            // vy = 速度 * sin(角度)
            p.vx = cos(angle)*speed;
            p.vy = sin(angle)*speed;

            emitted++;
            if(emitted>=count) break;
        }
    }
}

// --- [数学核心：时间与插值] ---
void ParticleSystem::Update(float dt){


    for (auto&p:pool){
        if(p.active){
            // 1. 移动位置 (速度 * 时间)
            p.x +=p.vx*dt;
            p.y +=p.vy*dt;

            // 2. 扣减寿命
            p.life -= dt;

            if (p.life<=0) p.active = false;
            else {
                // ==========================================
                // [数学魔法：归一化与线性插值 Lerp]
                // 随着时间流逝，让粒子渐渐变透明！
                // ==========================================
                
                // 算出剩余寿命的比例 (1.0 代表刚出生，0.0 代表快死了)
                float ratio = p.life/p.maxLife;
                // 透明度从 255 渐渐降到 0
                p.color.a = (unsigned char)(255*ratio);
                // 假设粒子初始大小是 5.0
                p.radius = p.baseRadius * ratio; // 寿命越少，ratio越小，半径就越小，直到缩成 0
                // 可选：让粒子越跑越慢 (摩擦力效果)
                p.vx*=0.95f;
                p.vy*=0.95f;
            }
        }
    }
}

void ParticleSystem::Draw(){
    // 开启加法混合模式 (光剑/发光效果的灵魂！)
    // 这会让重叠的粒子变得极其亮白，产生“高能量爆炸”的错觉
    BeginBlendMode(BLEND_ADDITIVE);

    for (const auto&p:pool) {
        if(p.active) {
            // DrawCircleV(Vector2{p.x, p.y}, p.radius, p.color);
            // 起点：粒子当前的位置
            Vector2 startPos = {p.x,p.y};

            // 终点：粒子【上一瞬间】的位置。
            // 用当前的坐标减去速度，乘以一个拉伸系数（比如 0.05f），速度越快，线越长！
            Vector2 endPos = {p.x-p.vx*0.15f,p.y-p.vy*0.15f};

            // 用 p.radius 作为线的粗细，画出这条残影
            DrawLineEx(startPos,endPos,p.radius,p.color);
        }
    }
    EndBlendMode();
}