g++ src/main.cpp src/Player.cpp src/ParticleSystem.cpp src/Game.cpp -o Starfighter.exe -O1 -Wall -std=c++11 -I include -I C:/raylib/include -L C:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm

./Starfighter.exe

得分越高玩家速度与子弹飞行速度越快，调低敌人速度变化率。

git add .
git commit -m "feat: Implement Object Pool and Survival Mechanics"
git push

更新计划：
1.选择buff：杀5个开启追踪导弹；杀5个回一血；杀5个移速加快
2.

