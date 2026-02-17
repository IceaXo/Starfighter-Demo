#include "raylib.h"
#include "Entity.h"
#include "Bullet.h"
#include "Player.h"
#include "Enemy.h"
#include <vector>
#include <string> // [韩立批注]: 为了方便显示分数文字
// --- [配置空间] ---
// 使用 constexpr (C++11) 替代 #define，类型更安全，面试官更喜欢。

int main(){
    InitWindow(800,600,"Starfighter - Demo");
    
    SetTargetFPS(60);
    // --- 2. 照着图纸造一架飞机 (Instantiation) ---
    // 创建一个叫 myPlane 的对象，出生在屏幕中心 (400, 500)
    Player myPlane(400,500);

    std::vector<Enemy> enemies;
    // --- 3. 游戏状态 (功德与天劫) ---
    int score = 0;    
    int frameCounter = 0;    // 计时器 (用于控制刷怪频率)

    while (!WindowShouldClose()){
        // --- 1. 逻辑更新 (Update) ---
        myPlane.Update();
        frameCounter++;
        // --- 步骤 1: 计算当前的天道压力 (动态生成间隔) ---
        // 基础是 60 帧(1秒)生成一次。
        // 这里的逻辑是：(score / 500) 算出玩家得了几个 500 分。
        // 每得 500 分，生成间隔就减少 5 帧。
        int currentSpawnRate = 60 - (score/500)*5;
        // [韩立]: 这是一个“保底阵法”。
        // 防止分数太高导致间隔变成 0 或负数。
        // 限制最快只能 20 帧(约0.3秒)生成一个敌人，否则游戏会崩溃或无法通关。
        if (currentSpawnRate<20) currentSpawnRate = 20;
        // --- 步骤 2: 判断是否到了生成敌人的时刻 ---
        // 只有当计数器(frameCounter) 超过了 计算出的间隔(currentSpawnRate) 时，才动手。
        if (frameCounter>=currentSpawnRate){
            float randomX = (float)GetRandomValue(20,780);

            Enemy newEnemy(randomX,-50);
            newEnemy.speed = 2.0f+(score/100)*0.5f;

            enemies.push_back(newEnemy);

            frameCounter = 0;
        }
        for (auto it = enemies.begin();it!= enemies.end();)
        {
            it->Update();

            if(it->active){
            // 遍历玩家弹夹里的每一颗子弹
            // auto& b : myPlane.bullets 意思是：拿出每一个子弹的真身(引用)
            for (auto& b : myPlane.bullets){
                // 如果这颗子弹也是活的
                if(b.active){
                    // [核心法术]：检测 子弹的圆 和 敌人的圆 是否重叠
                    // Vector2{x, y} 是把我们的坐标转换成 Raylib 需要的格式
                    if(CheckCollisionCircles(Vector2{b.x,b.y},b.radius,Vector2{it->x,it->y},it->radius))
                    {
                    it->active=false;
                    b.active=false;
                    score +=100;
                    break;
                }
                }
                
            }
        }
        if (!it->active){
            it = enemies.erase(it);
        }
        else {
            ++it;
        }
        }
        
        // --- 2. 画面渲染 (Draw) ---
        BeginDrawing();
        // 用黑色清空背景
        ClearBackground(BLACK);
        // 画出飞机
        myPlane.Draw();
        for (auto&e:enemies){
            e.Draw();
        }
        // 在屏幕上写一行白字
        DrawText(TextFormat("SCORE: %04d", score), 20, 20, 30, YELLOW);
        DrawText("WASD: Move | SPACE: Shoot", 10, 560, 20, DARKGRAY);
        
        
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
