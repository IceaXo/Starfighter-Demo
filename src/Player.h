#pragma once

#include "raylib.h"
#include "Entity.h"
#include "Bullet.h" // 玩家需要用到子弹
#include "Enemy.h"
#include <vector>


class Player : public Entity {
public:
    using Entity::Update;
    // 弹夹
    std::vector<Bullet> bullets;

    // [新增] 子弹池容量常量
    // 屏幕上同时存在的子弹很难超过 30 发，给 30 足够了
    static constexpr int MAX_BULLETS = 30;
    
    // --- [新增] 生存属性 ---
    int hp;                 // 当前血量
    float lastDamageTime;   // 上次受伤的时间点 (用于计算无敌帧)

    // 记录飞机过去 10 帧的坐标
    Vector2 history[10]; 
    // 记录写到了哪一页
    int historyIndex;
    
    // 【新增】杀戮状态机
    int killStreak;
    BulletType currentWeapon;

    // 构造函数声明
    Player(float startX, float startY);

    // 方法声明 (注意后面是分号 ;)
    void Update(std::vector<Enemy>& enemyPool);
    void Draw() override;
    // --- [新增] 受伤方法 ---
    // 传入伤害值，通常是 1
    void TakeDamage(int damage);
};
