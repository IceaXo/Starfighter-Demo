#pragma once

#include "raylib.h"
#include "Entity.h"
#include "Config.h"
#include "Effects.h"

class Enemy : public Entity {
public:
    using Entity::Update;
    unsigned int unique_id;
    static unsigned int next_id;

    Color hullColor;   // 随机色相变体
    Color glowColor;   // 光环颜色
    float spawnTime;   // 出生时刻 (用于脉动动画)

    Enemy(float startX, float startY) : Entity(startX, startY) {
        speed = 3.0f;
        radius = 30.0f;
        color = RED;
        unique_id = 0;
        hullColor = RED;
        glowColor = {255, 80, 40, 255};
        spawnTime = 0.0f;

        // 随机色相变体: 深红 / 暗橙 / 紫红
        int variant = GetRandomValue(0, 2);
        switch (variant) {
            case 0: hullColor = {58, 14, 14, 255};  glowColor = {255, 50, 20, 255}; break;  // 深红
            case 1: hullColor = {60, 30, 10, 255};  glowColor = {255, 100, 30, 255}; break;  // 暗橙
            case 2: hullColor = {45, 10, 40, 255};  glowColor = {220, 40, 180, 255}; break;  // 紫红
        }
    }

    void Update() override {
        y += speed;
        if (y > GameConfig::SCREEN_HEIGHT + 2 * radius) active = false;
    }

    void Draw() override {
        float time = (float)GetTime() - spawnTime;

        // ---- 第一层: 外围暗红光环 (ADDITIVE) ----
        BeginBlendMode(BLEND_ADDITIVE);
        {
            float r1 = 38.0f;
            Color outerAura = glowColor; outerAura.a = 40;
            Color transparent = {0, 0, 0, 0};
            DrawCircleGradient((int)x, (int)y, r1, outerAura, transparent);

            // 尖刺延伸 (四个方向)
            float spikeLen = 12.0f;
            Color spikeC = glowColor; spikeC.a = 120;
            DrawLineEx({x, y - 22.0f}, {x, y - 22.0f - spikeLen}, 2.0f, spikeC);
            DrawLineEx({x, y + 22.0f}, {x, y + 22.0f + spikeLen}, 2.0f, spikeC);
            DrawLineEx({x - 14.0f, y}, {x - 14.0f - spikeLen, y}, 2.0f, spikeC);
            DrawLineEx({x + 14.0f, y}, {x + 14.0f + spikeLen, y}, 2.0f, spikeC);
        }
        EndBlendMode();

        // ---- 第二层: 菱形主体 (ALPHA) ----
        BeginBlendMode(BLEND_ALPHA);
        {
            FX::DrawDiamond(x, y, 14.0f, 22.0f, hullColor);

            // 内部框线
            Color trim = glowColor; trim.a = 200;
            FX::DrawDiamondLines(x, y, 9.0f, 14.0f, trim);
        }
        EndBlendMode();

        // ---- 第三层: 脉动核心 (ADDITIVE) ----
        BeginBlendMode(BLEND_ADDITIVE);
        {
            float pulse  = 0.7f + 0.3f * sinf(time * 10.0f);
            float coreR  = 6.0f * pulse;
            Color coreIn = {255, 255, 255, 220};
            Color coreMid = glowColor; coreMid.a = 160;
            Color coreOut = glowColor; coreOut.a = 0;

            DrawCircleGradient((int)x, (int)y, coreR + 2.0f, coreMid, coreOut);
            DrawCircleGradient((int)x, (int)y, coreR,       coreIn,  coreOut);
            DrawCircle((int)x, (int)y, coreR * 0.4f, WHITE);
        }
        EndBlendMode();
    }
};
