#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "Entity.h"
#include "Bullet.h" // 玩家需要用到子弹
#include <vector>

class Player : public Entity {
public:
    // 弹夹
    std::vector<Bullet> bullets;

    // 构造函数声明
    Player(float startX, float startY);

    // 方法声明 (注意后面是分号 ;)
    void Update() override;
    void Draw() override;
};

#endif