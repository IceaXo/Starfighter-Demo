#pragma once
#include "raylib.h"
#include <cmath>

namespace FX {

// 绘制多层光晕 (ADDITIVE 模式下调用)
// 从内到外，颜色从 inner 过渡到 outer(通常是透明)
inline void DrawGlow(float x, float y, float r, Color inner, Color outer) {
    DrawCircleGradient((int)x, (int)y, r, inner, outer);
}

// 引擎火焰 — 三层：外焰→内焰→白热核心，半径随时间脉动
inline void DrawEngineFlame(float x, float y, float size, float time, Color flame) {
    float pulse = 0.7f + 0.3f * sinf(time * 15.0f);
    float r1 = size * pulse;
    float r2 = r1 * 0.55f;
    float r3 = r1 * 0.22f;

    Color outer = flame; outer.a = 100;
    Color mid   = {flame.r, flame.g, flame.b, 180};
    Color core  = {255, 255, 255, 240};
    Color zero   = {flame.r, flame.g, flame.b, 0};

    DrawCircleGradient((int)x, (int)y, r1, outer, zero);
    DrawCircleGradient((int)x, (int)y, r2, mid,   zero);
    DrawCircle((int)x, (int)y, r3, core);
}

// 绘制菱形 — 四个三角形组成
// halfW = 半宽，halfH = 半高
inline void DrawDiamond(float cx, float cy, float halfW, float halfH, Color c) {
    Vector2 top    = {cx,        cy - halfH};
    Vector2 bottom = {cx,        cy + halfH};
    Vector2 left   = {cx - halfW, cy};
    Vector2 right  = {cx + halfW, cy};
    Vector2 center = {cx, cy};

    DrawTriangle(top,   left,  center, c);  // 左上
    DrawTriangle(top,   center, right,  c);  // 右上
    DrawTriangle(left,  bottom, center, c);  // 左下
    DrawTriangle(bottom, right, center, c);  // 右下
}

// 绘制菱形线框
inline void DrawDiamondLines(float cx, float cy, float halfW, float halfH, Color c) {
    Vector2 top    = {cx,        cy - halfH};
    Vector2 bottom = {cx,        cy + halfH};
    Vector2 left   = {cx - halfW, cy};
    Vector2 right  = {cx + halfW, cy};

    DrawLineEx(top, left, 2.0f, c);
    DrawLineEx(top, right, 2.0f, c);
    DrawLineEx(bottom, left, 2.0f, c);
    DrawLineEx(bottom, right, 2.0f, c);
}

// 几何星舰机身 — 多层三角构成箭头形战斗机
// 坐标系：(cx, cy) 为机身中心，机头朝上 (y-)
inline void DrawStarFighterBody(float cx, float cy, Color body, Color wing, Color accent) {
    // ---- 机头 ----
    DrawTriangle(
        {cx,       cy - 32.0f},  // 鼻尖
        {cx - 9.0f, cy - 6.0f},  // 左下
        {cx + 9.0f, cy - 6.0f},  // 右下
        body);

    // ---- 机身左半 ----
    DrawTriangle(
        {cx - 9.0f, cy - 6.0f},
        {cx,        cy + 16.0f},
        {cx,        cy - 6.0f},
        body);
    // ---- 机身右半 ----
    DrawTriangle(
        {cx + 9.0f, cy - 6.0f},
        {cx,        cy + 16.0f},
        {cx,        cy - 6.0f},
        body);

    // ---- 左翼 ----
    DrawTriangle(
        {cx - 9.0f,  cy - 4.0f},
        {cx - 28.0f, cy + 12.0f},
        {cx - 9.0f,  cy + 12.0f},
        wing);
    // ---- 右翼 ----
    DrawTriangle(
        {cx + 9.0f,  cy - 4.0f},
        {cx + 28.0f, cy + 12.0f},
        {cx + 9.0f,  cy + 12.0f},
        wing);

    // ---- 发动机舱 (两条竖条) ----
    DrawRectangle((int)(cx - 9.0f),  (int)(cy + 12.0f), 5, 10, body);
    DrawRectangle((int)(cx + 4.0f), (int)(cy + 12.0f), 5, 10, body);

    // ---- 机身边线 (accent 色) ----
    DrawLineEx({cx,       cy - 32.0f}, {cx - 9.0f, cy - 6.0f}, 1.5f, accent);
    DrawLineEx({cx,       cy - 32.0f}, {cx + 9.0f, cy - 6.0f}, 1.5f, accent);
    DrawLineEx({cx - 9.0f, cy - 6.0f}, {cx - 28.0f, cy + 12.0f}, 1.5f, accent);
    DrawLineEx({cx + 9.0f, cy - 6.0f}, {cx + 28.0f, cy + 12.0f}, 1.5f, accent);

    // ---- 机腹中线 ----
    DrawLineEx({cx, cy + 16.0f}, {cx, cy - 6.0f}, 1.0f, accent);
}

// 绘制玩家星舰残影 (用于 ghost trail)
inline void DrawStarFighterGhost(float cx, float cy, float alpha, Color accent) {
    Color ghost = accent; ghost.a = (unsigned char)(alpha * 0.25f);
    DrawTriangleLines({cx,       cy - 32.0f},
                      {cx - 8.0f, cy + 14.0f},
                      {cx + 8.0f, cy + 14.0f}, ghost);
    DrawTriangleLines({cx,       cy - 28.0f},
                      {cx - 6.0f, cy + 10.0f},
                      {cx + 6.0f, cy + 10.0f}, ghost);
}

// 武器升级光环 (HOMING 武器激活时)
inline void DrawWeaponAura(float cx, float cy, float time) {
    float pulse = 0.85f + 0.15f * sinf(time * 6.0f);
    float r = 45.0f * pulse;
    Color inner = {0, 200, 255, 60};
    Color outer = {0, 200, 255, 0};
    DrawCircleGradient((int)cx, (int)cy, r, inner, outer);

    // 第二圈 —— 更内圈但更亮
    float r2 = r * 0.7f;
    Color i2 = {0, 220, 255, 90};
    Color o2 = {0, 200, 255, 0};
    DrawCircleGradient((int)cx, (int)cy, r2, i2, o2);
}

// 枪口闪光
inline void DrawMuzzleFlash(float x, float y, float age) {
    if (age > 0.08f) return;
    float ratio   = 1.0f - age / 0.08f;                     // 1→0
    float r       = 8.0f * (1.0f - ratio * 0.6f);           // 8→3
    Color core    = {255, 255, 255, (unsigned char)(255 * ratio)};
    Color glow    = {255, 220, 100, (unsigned char)(180 * ratio)};
    Color zero    = {255, 200, 50, 0};

    DrawCircleGradient((int)x, (int)y, r,       glow, zero);
    DrawCircle((int)x, (int)y, r * 0.35f, core);
}

} // namespace FX
