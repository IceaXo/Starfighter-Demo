#include "Player.h"
#include "Config.h"

// --- 构造函数实现 ---
Player::Player(float startX, float startY) : Entity(startX, startY) {
    speed = 5.0f;
    radius = 25.0f;
    color = SKYBLUE;

    // [新增] 初始化状态
    hp = 5;
    lastDamageTime = -2.0f;    

    killStreak = 0;
    
    historyIndex = 0;

    currentWeapon = BulletType::NORMAL;
    // --- [新增] 子弹对象池初始化 ---
    bullets.reserve(MAX_BULLETS);
    for(int i = 0; i < MAX_BULLETS; i++) {
        // 创建死子弹填满弹夹
        Bullet b(-100, -100); 
        b.active = false;
        bullets.push_back(b);
    }
}

void Player::Update(std::vector<Enemy>& enemyPool)
{
    if (IsKeyDown(KEY_A)) x -= speed;
    if (IsKeyDown(KEY_D)) x += speed;
    if (IsKeyDown(KEY_W)) y -= speed;
    if (IsKeyDown(KEY_S)) y += speed;

    
    if (IsKeyPressed(KEY_SPACE)) {
        // 遍历弹夹，找一颗“死”的子弹
        for (auto&b:bullets){
            if(!b.active){
                b.active = true;
                b.x = x;
                b.y = y-30;
                b.vx = 0.0f;
                b.vy = -b.speed;

                // 【核心 1】子弹继承玩家当前的武器状态
                b.type = this->currentWeapon;

                if(b.type == BulletType::HOMING){
                    int best_index = -1;
                    float min_dist_sq = 9999999.0f;

                    for(size_t i=0;i<enemyPool.size();i++){
                        if(enemyPool[i].active){
                            float dx = enemyPool[i].x-this->x;
                            float dy = enemyPool[i].y-this->y;
                            float dist_dq = dx*dx+dy*dy;

                            if (dist_dq<min_dist_sq) {
                                min_dist_sq = dist_dq;
                                best_index = i;
                            }
                        }
                    }

                    if(best_index!=-1){
                        b.target_index = best_index;
                        b.target_id = enemyPool[best_index].unique_id;
                    }else{
                        b.target_id = -1;
                    }
                }
                break;
            }
        }
    }
    
    for(auto& b :bullets){
        if(b.active) b.Update(enemyPool);
    }

    // 动态边界限制
    if(x<radius) x=radius;
    if (x > GameConfig::SCREEN_WIDTH - radius) x = GameConfig::SCREEN_WIDTH - radius;
    if (y < radius) y = radius;
    if (y > GameConfig::SCREEN_HEIGHT - radius) y = GameConfig::SCREEN_HEIGHT - radius;

    // 存入当前坐标
    history[historyIndex%10] = {x,y};
    historyIndex++;
}

void Player::TakeDamage(int damage)
{
    double currentTime = GetTime();

    // 只有当“当前时间”距离“上次受伤”超过 0.5 秒时，才扣血
    if (currentTime - lastDamageTime>0.5){
        hp-=damage;
        lastDamageTime = currentTime;
        // (可选) 以后这里可以在控制台打印一句 Log
        // TraceLog(LOG_INFO, "Player hit! HP: %d", hp);
    }
    
}
void Player::Draw() {
    BeginBlendMode(BLEND_ADDITIVE);

    if(GetTime()-lastDamageTime<0.5){
        // 让飞机闪烁：每隔 0.1 秒画一次，不画一次
        if((int)(GetTime()*10)%2==0) DrawTriangleLines({x,y-radius},{x-radius,y+radius},{x+radius,y+radius},color);
    }
    else {
        for (int i=0;i<10;i++){
            Vector2 pastPos = history[i];
            if (pastPos.x ==0&&pastPos.y ==0) continue;

            // 算出一个 0.1 到 1.0 的透明度和大小比例
            Color ghostColor = {SKYBLUE.r,SKYBLUE.g,SKYBLUE.b,100};
            float ghostRadius = radius *0.8f;

            DrawTriangleLines({pastPos.x,pastPos.y-ghostRadius},{pastPos.x-ghostRadius,pastPos.y+ghostRadius},{pastPos.x+ghostRadius,pastPos.y+ghostRadius},ghostColor);
        }
        DrawTriangleLines({x,y-radius},{x-radius,y+radius},{x+radius,y+radius},color);
    }
    // [画子弹循环] - 写法二：高效引用法 (C++11)
    // auto& b：b 是弹夹里子弹的【真身引用】。
    // : bullets：遍历 bullets 容器。
    // 如果不加 & (auto b)，b 就是个【复制品】，画它是浪费时间。
    // 加了 &，b 就是子弹本身，速度最快。
    for (auto& b : bullets) {
        if (b.active) b.Draw();
    }
    EndBlendMode();
}