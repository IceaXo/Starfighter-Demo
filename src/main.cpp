#include "Game.h"
#include "GameManager.h"

#include <vector>
#include <string> // [韩立批注]: 为了方便显示分数文字

unsigned int Enemy::next_id = 1;

int main()
{
    auto &GM = GameManager::GetInstance();

    InitWindow(GameConfig::SCREEN_WIDTH, GameConfig::SCREEN_HEIGHT, "Starfighter - Demo");
    // 加载泛光着色器 (0表示默认顶点着色器，"bloom.fs" 是刚才写的片段着色器)
    Shader bloomShader = LoadShader(0, "bloom.fs");
    SetTargetFPS(GameConfig::TARGET_FPS);

    RenderTexture2D target = LoadRenderTexture(GameConfig::SCREEN_WIDTH, GameConfig::SCREEN_HEIGHT);

    Camera2D camera = {0};
    camera.target = Vector2{0.0f, 0.0f};
    camera.offset = Vector2{0.0f, 0.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // 节拍器：严格规定物理逻辑每 1/60 秒（约 0.0166秒）走一步
    const float FIXED_STEP = 1.0f / 60.0f;
    // 时空沙漏：用来收集现实世界流逝的时间
    float accumulator = 0.0f;

    Game game;

    while (!WindowShouldClose()){
        // 获取这一帧现实过了多少秒，倒进沙漏
        float frameTime = GetFrameTime();
        // 【核心法术：封死上限】如果你卡了很久，我最多只认 0.25 秒！
        if (frameTime > 0.25f) frameTime = 0.25f;
        accumulator += frameTime;

        while (accumulator >= FIXED_STEP){
            if (GM.shakeIntensity > 0.0f){
                // 产生 -10 到 +10 的随机偏移
                camera.offset.x = (float)GetRandomValue(-GM.shakeIntensity, GM.shakeIntensity);
                camera.offset.y = (float)GetRandomValue(-GM.shakeIntensity, GM.shakeIntensity);
                // 震动极速衰减
                GM.shakeIntensity -= 0.5f;
                if (GM.shakeIntensity < 0.0f){
                    GM.shakeIntensity = 0.0f;
                    camera.offset = Vector2{0.0f, 0.0f};
                }
            }
            if (GM.freezeFrames > 0) GM.freezeFrames--;
            else game.Update(FIXED_STEP);
            accumulator -= FIXED_STEP;
        }
        BeginTextureMode(target);
        // 用黑色清空背景
        ClearBackground(BLACK);
        // 【开启摄像机模式】
        BeginMode2D(camera);
        game.Draw();
        EndMode2D(); // 【关闭摄像机模式】
        // UI
        game.DrawUI();
        EndTextureMode();

        Rectangle sourseRec = {0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height};
        Vector2 destPos = {0.0f, 0.0f};

        BeginDrawing();
        ClearBackground(BLACK);

        // 【开启魔法滤镜】
        BeginShaderMode(bloomShader);

        // 画出被滤镜处理过的隐形画布
        DrawTextureRec(target.texture, sourseRec, destPos, WHITE);
        EndShaderMode();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
