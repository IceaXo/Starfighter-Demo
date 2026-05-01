#include "Player.h"
#include "Config.h"
#include "Effects.h"

// --- 构造函数实现 ---
Player::Player(float startX, float startY) : Entity(startX, startY) {
    speed = 8.0f;
    radius = 40.0f;
    color = SKYBLUE;

    hp = 5;
    lastDamageTime = -2.0f;

    killStreak = 0;
    historyIndex = 0;
    muzzleFlashTime = -99.0f;

    currentWeapon = BulletType::NORMAL;
    bullets.reserve(MAX_BULLETS);
    for (int i = 0; i < MAX_BULLETS; i++) {
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
        // 遍历弹夹，找一颗”死”的子弹
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
                muzzleFlashTime = (float)GetTime();
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
    float time = (float)GetTime();
    bool invincible = (time - lastDamageTime < 0.5f);
    bool flashOn = invincible ? ((int)(time * 10) % 2 == 0) : true;
    bool hasAura = (killStreak >= 5);

    Color bodyC   = { 26,  48,  80, 255};  // 深蓝灰机身
    Color wingC   = { 37,  72, 112, 255};  // 略浅机翼
    Color accentC = {  0, 200, 255, 255};  // 青色饰线
    Color flameC  = {  0, 180, 240, 255};  // 引擎焰色

    // ---- 1. 引擎火焰 (ADDITIVE, 在机身之后绘制所以先画) ----
    BeginBlendMode(BLEND_ADDITIVE);
    FX::DrawEngineFlame(x - 6.5f, y + 22.0f, 10.0f, time,       flameC);
    FX::DrawEngineFlame(x + 6.5f, y + 22.0f, 10.0f, time + 0.3f, flameC);
    EndBlendMode();

    // ---- 2. 残影 (ADDITIVE) ----
    BeginBlendMode(BLEND_ADDITIVE);
    for (int i = 0; i < 10; i++) {
        Vector2 pastPos = history[i];
        if (pastPos.x == 0 && pastPos.y == 0) continue;
        float alpha = (float)(10 - i) / 10.0f;
        FX::DrawStarFighterGhost(pastPos.x, pastPos.y, alpha, accentC);
    }
    EndBlendMode();

    // ---- 3. 机身主体 (ALPHA — 实体几何) ----
    if (flashOn) {
        BeginBlendMode(BLEND_ALPHA);
        FX::DrawStarFighterBody(x, y, bodyC, wingC, accentC);
        EndBlendMode();

        // ---- 4. 座舱 + 翼尖灯 + 喷口高亮 (ADDITIVE) ----
        BeginBlendMode(BLEND_ADDITIVE);
        {
            // 座舱
            DrawCircleGradient((int)x, (int)(y - 10.0f), 6.0f,
                {220, 240, 255, 200}, {0, 80, 160, 0});
            DrawCircle((int)x, (int)(y - 10.0f), 2.5f,
                {240, 248, 255, 240});

            // 翼尖导航灯: 左红 / 右绿
            DrawCircle((int)(x - 27.0f), (int)(y + 11.0f), 2.5f,
                {255, 60, 40, 200});
            DrawCircle((int)(x + 27.0f), (int)(y + 11.0f), 2.5f,
                {40, 255, 80, 200});

            // 发动机喷口内壁
            DrawRectangle((int)(x - 7.5f), (int)(y + 13.0f), 3, 4,
                {180, 230, 255, 180});
            DrawRectangle((int)(x + 4.5f), (int)(y + 13.0f), 3, 4,
                {180, 230, 255, 180});
        }
        EndBlendMode();
    }

    // ---- 5. 武器升级光环 (ADDITIVE) ----
    if (hasAura) {
        BeginBlendMode(BLEND_ADDITIVE);
        FX::DrawWeaponAura(x, y, time);
        EndBlendMode();
    }

    // ---- 6. 枪口闪光 (ADDITIVE) ----
    float sinceFlash = time - muzzleFlashTime;
    if (sinceFlash < 0.08f) {
        BeginBlendMode(BLEND_ADDITIVE);
        FX::DrawMuzzleFlash(x, y - 30.0f, sinceFlash);
        EndBlendMode();
    }

    // ---- 7. 子弹 ----
    for (auto& b : bullets) {
        if (b.active) b.Draw();
    }
}