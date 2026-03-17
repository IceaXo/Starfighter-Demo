#include "Game.h"
#include "Config.h"
#include "GameManager.h"

Game::Game():myPlane(GameConfig::SCREEN_WIDTH/2.0f,GameConfig::SCREEN_HEIGHT-100),ps(500){
    currentState = GameState::PLAYING;
    boundary = {GameConfig::SCREEN_WIDTH/2.0f,GameConfig::SCREEN_HEIGHT/2.0f,GameConfig::SCREEN_WIDTH/2.0f,GameConfig::SCREEN_HEIGHT/2.0f};
    
    stars.resize(GameConfig::MAX_STARS);
    // 遍历每一个星星，赋予它们随机的属性
    for(auto &s:stars){
        s.x = (float)GetRandomValue(0,GameConfig::SCREEN_WIDTH);
        s.y = (float)GetRandomValue(0,GameConfig::SCREEN_HEIGHT);
        s.speed = (float)GetRandomValue(5,40)/10.0f;
        // 速度越快 (离得越近)，星星就越大
        s.radius = s.speed *0.4f;
        // 速度越快，星星越亮 (透明度 Alpha 越高)
        // Alpha 范围是 0 (全透明) 到 255 (不透明)
        unsigned char alpha = (unsigned char)(s.speed / 4.0f*255);
        // 组装颜色：白色，但带有不同的透明度
        s.color = {255,255,255,alpha};
    }

    enemies.reserve(GameConfig::MAX_ENEMIES);// 预分配内存，避免扩容
    for (int i = 0; i < GameConfig::MAX_ENEMIES; i++) {
        Enemy e(-100, -100); // 放在屏幕外
        e.active = false;    // [关键]: 标记为“死”，等待复活
        enemies.push_back(e);
    }

}
Game::~Game(){}

void Game::Update(float dt) {
    if (currentState != GameState::PLAYING) return;
    // --- 1. 逻辑更新 (Update) ---
    UpdateStars();
    myPlane.Update(enemies);
    spawner.Update(enemies);     
    ps.Update(dt);

    auto& GM = GameManager::GetInstance();
    if (myPlane.hp <=0) currentState = GameState::GAME_OVER;
    if (GM.score >=GameConfig::TARGET_SCORE) currentState = GameState::VICTORY;

    CheckCollisions();
}

void Game::Draw() {
        for(const auto&s:stars) DrawCircleV(Vector2{s.x,s.y},s.radius,s.color);
        if (currentState==GameState::PLAYING){
            myPlane.Draw();
            for (auto&e:enemies) if (e.active) e.Draw();
            ps.Draw();
        }
}
void Game::UpdateStars() {
    for (auto&s:stars){
        s.y += s.speed;
        // 如果星星掉出了屏幕底部
        if(s.y>GameConfig::SCREEN_HEIGHT){
        s.y = 0;
        s.x = (float)GetRandomValue(0,GameConfig::SCREEN_WIDTH);
        }
    }
}

void Game::CheckCollisions() {
    auto& GM = GameManager::GetInstance();
    QuadTree qt(boundary,4);
    for (auto &e : enemies){
        if (e.active){
            e.Update();
            qt.insert(&e);
        }
    }
    // 遍历玩家弹夹里的每一颗子弹
    for (auto &b : myPlane.bullets){
        // 如果这颗子弹也是活的
        if (!b.active) continue;

        Boundary searchRange = {b.x, b.y, 35.0f, 35.0f};
        std::vector<Enemy *> suspects = qt.query(searchRange);
        for (Enemy *e : suspects){
            // [核心法术]：检测 子弹的圆 和 敌人的圆 是否重叠
            // Vector2{x, y} 是把我们的坐标转换成 Raylib 需要的格式
            if (CheckCollisionCircles(Vector2{b.x, b.y}, b.radius, Vector2{e->x, e->y}, e->radius)){
                e->active = false;
                b.active = false;
                GM.score += GameConfig::SCORE_PER_KILL;
                myPlane.killStreak++;
                if (myPlane.killStreak >= 5){
                    myPlane.currentWeapon = BulletType::HOMING;
                    myPlane.speed += 0.3f;
                }
                ps.Emit(e->x, e->y, 30, ORANGE);
                // 顿帧
                GM.TriggerShake(5.0f,2);
                break;
            }
        }
    }
    Boundary playerRange = {myPlane.x, myPlane.y, 50.0f, 50.0f};
    std::vector<Enemy *> playersuspects = qt.query(playerRange);
    for (Enemy *e : playersuspects){
        if (e->active && CheckCollisionCircles(Vector2{myPlane.x, myPlane.y}, myPlane.radius, Vector2{e->x, e->y}, e->radius)){
            e->active = false;
            myPlane.TakeDamage(1);
            myPlane.killStreak = 0;
            myPlane.speed -= 0.8f;
            myPlane.currentWeapon = BulletType::NORMAL;

            // [新增法术：玉石俱焚的爆炸！]
            // 敌人炸出橙色火花，玩家受击炸出蓝色装甲碎片
            ps.Emit(e->x, e->y, 30, ORANGE);
            ps.Emit(myPlane.x, myPlane.y, 15, SKYBLUE);

            // 顿帧
            GM.TriggerShake(10.0,3);
        }
    }
}

void Game::DrawUI() {
    auto& GM = GameManager::GetInstance();
    if (currentState == GameState::PLAYING) {
        DrawText(TextFormat("SCORE: %04d", GM.score), 20, 20, 30, YELLOW);
        DrawText(TextFormat("HP: %d", myPlane.hp), 20, 60, 30, RED); // myPlane 是 Game 的私有财产，它自己可以随便用！
        DrawText("WASD: Move | SPACE: Shoot", 10, GameConfig::SCREEN_HEIGHT - 30, 20, DARKGRAY);
    } 
    else if (currentState == GameState::GAME_OVER) {
        const char *title = "GAME OVER";
        int titleWidth = MeasureText(title, 50);
        DrawText(title, GameConfig::SCREEN_WIDTH / 2 - titleWidth / 2, GameConfig::SCREEN_HEIGHT / 2 - 60, 50, RED);

        const char *scoreText = TextFormat("Final Score: %d", GM.score);
        int scoreWidth = MeasureText(scoreText, 20);
        DrawText(scoreText, GameConfig::SCREEN_WIDTH / 2 - scoreWidth / 2, GameConfig::SCREEN_HEIGHT / 2 + 20, 20, RAYWHITE);
    } 
    else if (currentState == GameState::VICTORY) {
        const char *title = "VICTORY!";
        int titleWidth = MeasureText(title, 50);
        DrawText(title, GameConfig::SCREEN_WIDTH / 2 - titleWidth / 2, GameConfig::SCREEN_HEIGHT / 2 - 60, 50, GREEN);

        const char *scoreText = TextFormat("Final Score: %d", GM.score);
        int scoreWidth = MeasureText(scoreText, 20);
        DrawText(scoreText, GameConfig::SCREEN_WIDTH / 2 - scoreWidth / 2, GameConfig::SCREEN_HEIGHT / 2 + 20, 20, RAYWHITE);
    }
}
