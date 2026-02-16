#include "Player.h"

// --- 构造函数实现 ---
Player::Player(float startX, float startY) : Entity(startX, startY) {
    speed = 5.0f;
    radius = 25.0f;
    color = SKYBLUE;
}

void Player::Update()
{
    if (IsKeyDown(KEY_A)) x -= speed;
    if (IsKeyDown(KEY_D)) x += speed;
    if (IsKeyDown(KEY_W)) y -= speed;
    if (IsKeyDown(KEY_S)) y += speed;


    if (IsKeyPressed(KEY_SPACE)) {
        bullets.push_back(Bullet(x, y - 30));
    }
    
    for (int i = 0; i < bullets.size(); i++) {
        bullets[i].Update();
    }
    for (auto it = bullets.begin(); it != bullets.end(); ) {
        
        // it->active：查看 it 指向的那颗子弹是不是活的。
        // -> 箭头：因为 it 是指针，访问属性要用箭头，不能用点。
        if (!it->active) {
            // [erase]：把这颗死子弹踢出去。
            // 关键：踢出去后，后面的子弹会自动补位。
            // erase 会返回【补位上来的那颗新子弹】的位置，赋值给 it。
            it = bullets.erase(it); 
        } else {
            // 如果子弹活着，it 就往后挪一位，检查下一颗。
            ++it;
        }
    }
}

void Player::Draw(){
    Entity::Draw();
    // [画子弹循环] - 写法二：高效引用法 (C++11)
    // auto& b：b 是弹夹里子弹的【真身引用】。
    // : bullets：遍历 bullets 容器。
    // 如果不加 & (auto b)，b 就是个【复制品】，画它是浪费时间。
    // 加了 &，b 就是子弹本身，速度最快。
    for (auto& b : bullets) {
        b.Draw();
    }
}