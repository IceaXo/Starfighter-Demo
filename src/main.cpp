#include "raylib.h"
#include "Entity.h"
#include "Bullet.h"
#include "Player.h"
#include "Enemy.h"
#include <vector>
#include <string> // [韩立批注]: 为了方便显示分数文字

// --- [配置空间] ---
namespace GameConfig {
    constexpr int SCREEN_WIDTH = 800;
    constexpr int SCREEN_HEIGHT = 600;
    constexpr int TARGET_FPS = 60;

    // --- 游戏性参数 ---
    constexpr int MAX_ENEMIES = 100;         // 对象池上限
    constexpr int BASE_SPAWN_RATE = 60;      // 基础刷怪间隔
    constexpr int MIN_SPAWN_RATE = 20;       // 最快间隔
    constexpr int SCORE_PER_KILL = 100;      // 分数
    constexpr int TARGET_SCORE = 2000;       // [新增] 通关分数 (达到这个分就赢)
    
    // --- 难度参数 ---
    constexpr int DIFFICULTY_STEP_RATE = 500;     
    constexpr int DIFFICULTY_STEP_SPEED = 200;
    constexpr float ENEMY_BASE_SPEED = 2.0f; 
    constexpr float ENEMY_SPEED_INCREMENT = 0.3f;
}
// [新增] 游戏状态枚举 (State Machine)
// 作用：区分游戏是在玩、还是输了、还是赢了
enum class GameState {
    PLAYING,
    GAME_OVER,
    VICTORY
};

int main(){
    InitWindow(GameConfig::SCREEN_WIDTH,GameConfig::SCREEN_HEIGHT,"Starfighter - Demo");
    
    SetTargetFPS(GameConfig::TARGET_FPS);
    // --- 2. 照着图纸造一架飞机 (Instantiation) ---
    // 创建一个叫 myPlane 的对象，出生在屏幕中心 (400, 500)
    Player myPlane(GameConfig::SCREEN_WIDTH/2.0f,GameConfig::SCREEN_HEIGHT-100);

    // --- 2. 敌军对象池初始化 (Object Pool Init) ---
    // [解释]: 以前是空的，现在我们一开始就造好 100 个“尸体”。
    std::vector<Enemy> enemies;
    enemies.reserve(GameConfig::MAX_ENEMIES);// 预分配内存，避免扩容
    for (int i = 0; i < GameConfig::MAX_ENEMIES; i++) {
        Enemy e(-100, -100); // 放在屏幕外
        e.active = false;    // [关键]: 标记为“死”，等待复活
        enemies.push_back(e);
    }
    // 此时 enemies.size() 已经是 100 了，之后永远不会变！

    // --- 3. 游戏状态 (功德与天劫) ---
    int score = 0;    
    int frameCounter = 0;    // 计时器 (用于控制刷怪频率)

    // [新增] 当前游戏状态，默认为“正在玩”
    GameState currentState = GameState::PLAYING;

    while (!WindowShouldClose()){
        if (currentState == GameState::PLAYING){
            // --- 1. 逻辑更新 (Update) ---
        myPlane.Update();

        // [新增] 胜负判定
        if (myPlane.hp <=0) currentState = GameState::GAME_OVER;
        if (score >=GameConfig::TARGET_SCORE) currentState = GameState::VICTORY;
        
        // --- B. 敌军生成 (Spawner) ---
        frameCounter++;
        
        // --- 步骤 1: 计算当前的天道压力 (动态生成间隔) ---
        // 基础是 60 帧(1秒)生成一次。
        // 逻辑：基础间隔 - (当前分数 / 难度阶梯) * 5
        int difficultyLevel = score/GameConfig::DIFFICULTY_STEP_RATE;
        int currentSpawnRate = GameConfig::BASE_SPAWN_RATE - (difficultyLevel*5);
        
        // [韩立]: 这是一个“保底阵法”。
        // 防止分数太高导致间隔变成 0 或负数。
        // 限制最快只能 20 帧(约0.3秒)生成一个敌人，否则游戏会崩溃或无法通关。
        if (currentSpawnRate<GameConfig::MIN_SPAWN_RATE) currentSpawnRate = GameConfig::MIN_SPAWN_RATE;
        
        // --- 步骤 2: 判断是否到了生成敌人的时刻 ---
        // 只有当计数器(frameCounter) 超过了 计算出的间隔(currentSpawnRate) 时，才动手。
        if (frameCounter>=currentSpawnRate){
            // [对象池复活逻辑]: 替代之前的 push_back
                // 遍历池子，找一个“死”的敌人把它变“活”
                for (auto& e:enemies){
                    if(!e.active){
                        e.active = true;
                        e.x = (float)GetRandomValue(20,GameConfig::SCREEN_WIDTH-20);
                        e.y = -50;
                        int speedLevel = score / GameConfig::DIFFICULTY_STEP_SPEED;
                        e.speed = GameConfig::ENEMY_BASE_SPEED+speedLevel*GameConfig::ENEMY_SPEED_INCREMENT;
                        break;
                    }
                }
            frameCounter = 0;
        }
        for (auto &e:enemies)
        {
            if(e.active){
                e.Update();
            // 遍历玩家弹夹里的每一颗子弹
            // auto& b : myPlane.bullets 意思是：拿出每一个子弹的真身(引用)
            for (auto& b : myPlane.bullets){
                // 如果这颗子弹也是活的
                if(b.active){
                    // [核心法术]：检测 子弹的圆 和 敌人的圆 是否重叠
                    // Vector2{x, y} 是把我们的坐标转换成 Raylib 需要的格式
                    if(CheckCollisionCircles(Vector2{b.x,b.y},b.radius,Vector2{e.x,e.y},e.radius))
                    {
                    e.active=false;
                    b.active=false;
                    score +=GameConfig::SCORE_PER_KILL;
                    break;
                }
                }
                
            }
            // 2. [新增] 敌人撞玩家 (同归于尽)
            // CheckCollisionCircles: Raylib 自带的圆形碰撞检测
            if(e.active){
                if(CheckCollisionCircles(Vector2{myPlane.x,myPlane.y},myPlane.radius,Vector2{e.x,e.y},e.radius)){
                e.active = false;
                myPlane.TakeDamage(1);
            }
            }
        }
        }
        
        }
        // 如果状态是 GAME_OVER 或 VICTORY，这里什么都不做，逻辑冻结。
        // --- 2. 画面渲染 (Draw) ---
        BeginDrawing();
        // 用黑色清空背景
        ClearBackground(BLACK);
        if (currentState==GameState::PLAYING){
            // 画出飞机
        myPlane.Draw();
        for (auto&e:enemies){
            if (e.active) e.Draw();
        }

        // UI
        DrawText(TextFormat("SCORE: %04d", score), 20, 20, 30, YELLOW);
        DrawText(TextFormat("HP: %d", myPlane.hp), 20, 60, 30, RED);
        DrawText("WASD: Move | SPACE: Shoot", 10, 560, 20, DARKGRAY);
        }

        else if (currentState==GameState::GAME_OVER){
            DrawText("GAME OVER", 250, 250, 50, RED);
            DrawText(TextFormat("Final Score: %d", score), 280, 320, 20, RAYWHITE);
        }
        
        else if (currentState == GameState::VICTORY){
            DrawText("VICTORY!", 280, 250, 50, GREEN);
            DrawText(TextFormat("Final Score: %d", score), 280, 320, 20, RAYWHITE);
        }
        
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
