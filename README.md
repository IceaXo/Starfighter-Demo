# Starfighter: The Void Vanguard (星海先锋)

## 项目简介
这是一个基于 C++ 和 Raylib 图形库开发的高性能 2D 弹幕射击游戏 (Space Shooter)。
本项目旨在展示 C++ 面向对象编程、内存管理 (对象池) 以及基础的图形学应用。

## 核心技术
- **语言**: C++ 14
- **引擎**: Raylib
- **架构**: 实体组件系统 (Entity System), 对象池 (Object Pool)

## 开发进度
- [ ] 基础环境搭建
- [ ] 玩家移动控制
- [ ] 敌人生成与 AI
- [ ] 碰撞检测与计分

## 📅 [2026-2-16] 开发日志：架构重构与碰撞检测

### ✅ 今日完成 (Completed)
1. **架构重构 (Refactoring)**：
   - 创建基类 `Entity`，实现属性复用。
   - 将 `Player`、`Bullet`、`Enemy` 拆分到独立文件 (.h / .cpp)。
   - 实现了 `vector` 内存管理，防止内存泄漏。
2. **核心机制 (Mechanics)**：
   - 实现了 `WASD` 移动与 `Space` 射击。
   - 实现了基于 Raylib `CheckCollisionCircles` 的碰撞检测。
   - 实现了敌人死亡后的无限重生逻辑。

### 🚧 待办事项 (To-Do)
1. 实现计分系统 (Score UI)。
2. 实现敌军方阵 (Vector of Enemies)。
3. 增加难度曲线 (随着分数增加，敌人速度变快)。
