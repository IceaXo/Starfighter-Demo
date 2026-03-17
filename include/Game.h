#pragma once
#include <vector>
#include "raylib.h"
#include "Player.h"
#include "Enemy.h"
#include "ParticleSystem.h"
#include "EnemySpawner.h"
#include "QuadTree.h"

struct Star{
    float x,y;
    float speed;
    float radius;
    Color color;
};

enum class GameState {
    TITLE,
    PLAYING,
    GAME_OVER,
    VICTORY
};

class Game {
public:
    GameState currentState;
    Game();
    ~Game();
    void Update(float dt);
    void Draw();
    void DrawUI();
    void Reset();

private:
    Player myPlane;
    std::vector<Enemy> enemies;
    ParticleSystem ps;
    EnemySpawner spawner;
    std::vector<Star> stars;
    Boundary boundary;

    void UpdateStars();
    void CheckCollisions();
};