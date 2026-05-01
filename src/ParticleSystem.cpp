#include "ParticleSystem.h"
#include <cmath>

ParticleSystem::ParticleSystem(int size){
    poolSize = size;
    pool.reserve(poolSize);
    for (int i = 0; i < poolSize; i++) {
        Particle p;
        p.active = false;
        pool.push_back(p);
    }
}

void ParticleSystem::Emit(float startX, float startY, int count, Color baseColor){
    int emitted = 0;

    for (auto& p : pool){
        if (!p.active){
            p.active = true;
            p.x = startX;
            p.y = startY;

            // 随机选择粒子形状: 60% LINE / 25% CIRCLE / 10% SQUARE / 5% SPARK
            int rnd = GetRandomValue(0, 99);
            if      (rnd < 60) p.shape = ParticleShape::LINE;
            else if (rnd < 85) p.shape = ParticleShape::CIRCLE;
            else if (rnd < 95) p.shape = ParticleShape::SQUARE;
            else               p.shape = ParticleShape::SPARK;

            // 根据形状微调参数
            float speedScale = 1.0f;
            float lifeScale  = 1.0f;
            switch (p.shape) {
                case ParticleShape::LINE:   speedScale = 1.0f;  lifeScale = 1.0f;  break;
                case ParticleShape::CIRCLE: speedScale = 0.6f;  lifeScale = 1.3f;  break;
                case ParticleShape::SQUARE: speedScale = 0.35f; lifeScale = 1.6f;  break;
                case ParticleShape::SPARK:  speedScale = 1.4f;  lifeScale = 0.6f;  break;
            }

            p.maxLife = (float)GetRandomValue(5, 15) / 10.0f * lifeScale;
            p.life = p.maxLife;

            p.baseRadius = (float)GetRandomValue(1, 4);
            p.radius = p.baseRadius;

            // 颜色微调: 在基础色上做 ±15 的随机偏移
            Color varied = baseColor;
            varied.r = (unsigned char)(baseColor.r + GetRandomValue(-15, 15));
            varied.g = (unsigned char)(baseColor.g + GetRandomValue(-15, 15));
            varied.b = (unsigned char)(baseColor.b + GetRandomValue(-15, 15));
            p.color = varied;

            float angle = (float)GetRandomValue(0, 360) * (PI / 180.0f);
            float speed = (float)GetRandomValue(200, 800) * speedScale;
            p.vx = cos(angle) * speed;
            p.vy = sin(angle) * speed;

            emitted++;
            if (emitted >= count) break;
        }
    }
}

// 轻量级发射: 用于尾迹等持续少量粒子
void ParticleSystem::EmitTrail(float startX, float startY, int count, Color baseColor, float maxSpeed){
    int emitted = 0;
    for (auto& p : pool){
        if (!p.active){
            p.active = true;
            p.x = startX;
            p.y = startY;

            p.shape = ParticleShape::CIRCLE;
            p.maxLife = (float)GetRandomValue(2, 5) / 10.0f;  // 短命: 0.2~0.5s
            p.life = p.maxLife;
            p.baseRadius = (float)GetRandomValue(1, 2);
            p.radius = p.baseRadius;

            Color varied = baseColor;
            varied.a = (unsigned char)(GetRandomValue(60, 140));
            p.color = varied;

            float angle = (float)GetRandomValue(140, 220) * (PI / 180.0f);  // 大体向下
            float speed = (float)GetRandomValue(30, (int)maxSpeed);
            p.vx = cos(angle) * speed;
            p.vy = sin(angle) * speed;

            emitted++;
            if (emitted >= count) break;
        }
    }
}

void ParticleSystem::Update(float dt){
    for (auto& p : pool){
        if (p.active){
            p.x += p.vx * dt;
            p.y += p.vy * dt;
            p.life -= dt;

            if (p.life <= 0) p.active = false;
            else {
                float ratio = p.life / p.maxLife;
                p.color.a = (unsigned char)(255 * ratio);
                p.radius = p.baseRadius * ratio;
                p.vx *= 0.95f;
                p.vy *= 0.95f;
            }
        }
    }
}

void ParticleSystem::Draw(){
    BeginBlendMode(BLEND_ADDITIVE);

    for (const auto& p : pool) {
        if (!p.active) continue;

        switch (p.shape) {
            case ParticleShape::LINE: {
                Vector2 startPos = {p.x, p.y};
                Vector2 endPos   = {p.x - p.vx * 0.25f, p.y - p.vy * 0.25f};
                DrawLineEx(startPos, endPos, p.radius, p.color);
                break;
            }
            case ParticleShape::CIRCLE: {
                Color outer = p.color; outer.a = 0;
                DrawCircleGradient((int)p.x, (int)p.y, p.radius, p.color, outer);
                break;
            }
            case ParticleShape::SQUARE: {
                float s = p.radius;
                Rectangle rec = {p.x - s / 2.0f, p.y - s / 2.0f, s, s};
                DrawRectanglePro(rec, {s / 2.0f, s / 2.0f}, 0.0f, p.color);
                break;
            }
            case ParticleShape::SPARK: {
                // 微小闪烁十字
                DrawLine((int)(p.x - p.radius), (int)p.y,
                         (int)(p.x + p.radius), (int)p.y, p.color);
                DrawLine((int)p.x, (int)(p.y - p.radius),
                         (int)p.x, (int)(p.y + p.radius), p.color);
                break;
            }
        }
    }
    EndBlendMode();
}
