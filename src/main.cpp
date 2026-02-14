#include "raylib.h"

int main(){
    InitWindow(800,600,"Starfighter - Demo");
    
    SetTargetFPS(60);

    while (!WindowShouldClose()){
        // --- 1. 逻辑更新 (Update) ---
        // (稍后这里写玩家移动代码)

        // --- 2. 画面渲染 (Draw) ---
        BeginDrawing();
        
        // 用黑色清空背景
        ClearBackground(BLACK);
        
        // 在屏幕上写一行白字
        DrawText("System Online: Waiting for Code...", 200, 300, 20, WHITE);
        
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
