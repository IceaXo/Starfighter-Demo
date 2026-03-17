#pragma once

#include "Enemy.h"
#include "Config.h"
#include "GameManager.h"
#include "raylib.h"
#include <vector>

class EnemySpawner {
private:
    int frameCounter;

public:
    EnemySpawner(){
        frameCounter = 0;
    }

    // --- B. 敌军生成 (Spawner) ---
   void Update(std::vector<Enemy>& enemies) {
        frameCounter++;
        
        // 唤醒大管家，打听现在的分数
        auto& GM = GameManager::GetInstance();
        
        // --- 步骤 1: 计算当前的天道压力 (动态生成间隔) ---
        int difficultyLevel = GM.score / GameConfig::DIFFICULTY_STEP_RATE;
        int currentSpawnRate = GameConfig::BASE_SPAWN_RATE - (difficultyLevel * 5);
        
        // [保底阵法]
        if (currentSpawnRate < GameConfig::MIN_SPAWN_RATE) {
            currentSpawnRate = GameConfig::MIN_SPAWN_RATE;
        }
        
        // --- 步骤 2: 到了时辰，抓壮丁复活 ---
        if (frameCounter >= currentSpawnRate) {
            // 遍历池子，找一个“死”的敌人把它变“活”
            for (auto& e : enemies) {
                if (!e.active) {
                    e.active = true;
                    e.unique_id = Enemy::next_id++;
                    e.x = (float)GetRandomValue(20, GameConfig::SCREEN_WIDTH - 20);
                    e.y = -50; // 从屏幕外飞进场
                    int speedLevel = GM.score / GameConfig::DIFFICULTY_STEP_SPEED;
                    e.speed = GameConfig::ENEMY_BASE_SPEED + speedLevel * GameConfig::ENEMY_SPEED_INCREMENT;
                    break;
                }
            }
            frameCounter = 0; // 干完活，计时器清零
        }
    }
};