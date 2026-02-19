#pragma once
#include "raylib.h"
#include <vector>

// 粒子的纯数据肉身
struct Particle {
    // 1. 空间属性
    float x, y;       // 当前位置
    float vx, vy;     // [新概念] 速度向量 (Velocity X, Velocity Y)
    
    // 2. 视觉属性
    Color color;      // 粒子的颜色
    float radius=5.0f;     // 粒子的大小
    float baseRadius = 5.0f;

    // 3. 时间属性 (决定生死)
    float life;       // 粒子还能活多久 (比如 1.0 秒)
    float maxLife;    // 粒子的寿命上限 (用于计算渐变比例)
    
    bool active;      // 对象池标记：是否活着

};

// 粒子阵法控制枢纽
class ParticleSystem {
private:
    std::vector<Particle> pool;
    int poolSize;

public:
    ParticleSystem(int size);

    // [核心法术 1]：在指定位置，引爆指定数量的粒子
    void Emit(float startX,float startY,int count, Color baseColor);

    // [核心法术 2]：时间的流逝与状态更新
    void Update();

    // [核心法术 3]：渲染
    void Draw();
};