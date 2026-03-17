#pragma once

class GameManager{
public:
    static GameManager& GetInstance(){
        static GameManager instance;
        return instance;
    }

    int score;
    float shakeIntensity;
    int freezeFrames;

    void AddScore(int val) {score+= val;}
    void TriggerShake (float intensity,int freeze){
        shakeIntensity = intensity;
        freezeFrames = freeze;
    }

    void Reset(){
        score = 0;
        shakeIntensity = 0.0f;
        freezeFrames = 0;
    }

private:
    GameManager() {
        score = 0;
        shakeIntensity = 0.0f;
        freezeFrames = 0;
    }

    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;
};