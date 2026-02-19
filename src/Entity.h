#ifndef ENTITY_H  // 1. 如果没定义过 ENTITY_H
#define ENTITY_H  // 2. 那就定义它 (防止被重复包含)

#include "Config.h"
#include "raylib.h"


class Entity {
public:
    // 大家都拥有的属性
    float x, y;
    float speed;
    float radius;
    bool active; // 是否存活
    Color color;

    // 构造函数
    Entity(float startX, float startY) {
        x = startX;
        y = startY;
        active = true;
        radius = 10.0f; // 默认半径
        speed = 0.0f;   // 默认速度
        color = WHITE;  // 默认颜色
    }
    // 虚析构函数 (必写！)：
    // 确保销毁子类对象时，内存能清理干净，否则会有内存泄漏
    virtual ~Entity() {}
    // --- 虚函数 (关键！) ---
    // virtual 的意思是：允许子类 (Player/Enemy) 推翻父类的逻辑，写自己的版本。
    // 如果子类不写，就用父类这个空的。
    virtual void Update() {}
    //默认画圆
    virtual void Draw() {
        if (active) {
            DrawCircle((int)x, (int)y, radius, color);
        }
    }
};
#endif