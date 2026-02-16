#include "raylib.h"
#include "Entity.h"
#include "Bullet.h"
#include "Player.h"
#include "Enemy.h"
#include <vector>


int main(){
    InitWindow(800,600,"Starfighter - Demo");
    
    SetTargetFPS(60);
    // --- 2. 照着图纸造一架飞机 (Instantiation) ---
    // 创建一个叫 myPlane 的对象，出生在屏幕中心 (400, 300)
    Player myPlane(400,500);
    std::vector<Enemy> enemies;
    Enemy target(400, -50);
    while (!WindowShouldClose()){
        // --- 1. 逻辑更新 (Update) ---
        // (稍后这里写玩家移动代码)
        myPlane.Update();
        target.Update();
        
        // --- [新增] 碰撞检测逻辑 ---
        // 只有当敌人活着的时候，才检测碰撞
        if(target.active){
            // 遍历玩家弹夹里的每一颗子弹
            // auto& b : myPlane.bullets 意思是：拿出每一个子弹的真身(引用)
            for (auto& b : myPlane.bullets){
                // 如果这颗子弹也是活的
                if(b.active){
                    // [核心法术]：检测 子弹的圆 和 敌人的圆 是否重叠
                    // Vector2{x, y} 是把我们的坐标转换成 Raylib 需要的格式
                    if(CheckCollisionCircles(Vector2{b.x,b.y},b.radius,Vector2{target.x,target.y},target.radius))
                    {
                    target.active=false;
                    b.active=false;
                    // 这里以后可以加分：score += 100;
                }
                }
                
            }
        }
        // --- [新增] 敌人重生逻辑 ---
        // 如果敌人死了 (被子弹打死，或者飞出屏幕)，我们就让它重生
        if(!target.active){
            // 1. 把它复活
            target.active = true;
            // 2. 把它的 Y 坐标拉回屏幕上方
            target.y=-50;
            // 3. [随机] 让它的 X 坐标在 50 到 750 之间随机选一个
            target.x = GetRandomValue(50,750);
        }
        // --- 2. 画面渲染 (Draw) ---
        BeginDrawing();
        // 用黑色清空背景
        ClearBackground(BLACK);
        // 画出飞机
        myPlane.Draw();
        target.Draw();
        // 在屏幕上写一行白字
        DrawText("WASD: Move | SPACE: Shoot", 10, 10, 20, WHITE);
        
        
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
