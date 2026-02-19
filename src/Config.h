// [src/Config.h]
#pragma once

namespace GameConfig {
    constexpr int SCREEN_WIDTH = 1280;
    constexpr int SCREEN_HEIGHT = 720;
    constexpr int TARGET_FPS = 60;

    // --- 游戏性参数 ---
    constexpr int MAX_ENEMIES = 100;         
    constexpr int BASE_SPAWN_RATE = 60;      
    constexpr int MIN_SPAWN_RATE = 20;       
    constexpr int SCORE_PER_KILL = 100;      
    constexpr int TARGET_SCORE = 2000;       
    
    // --- 难度参数 ---
    constexpr int DIFFICULTY_STEP_RATE = 500;     
    constexpr int DIFFICULTY_STEP_SPEED = 200;
    constexpr float ENEMY_BASE_SPEED = 2.0f; 
    constexpr float ENEMY_SPEED_INCREMENT = 0.3f;

    // --- 星空配置 ---
    constexpr int MAX_STARS = 300;
}