# Starfighter: The Void Vanguard (星海先锋)

> 一个基于 C++11 和 Raylib 构建的 2D 弹幕射击游戏与底层架构演练 Demo。
> 本项目专注于从零实践 **现代 C++ 内存管理 (RAII)**、**对象池算法** 、**物理与渲染解耦** 以及 **空间划分碰撞算法** ，探索游戏引擎底层的核心工程问题。

## 🎮 核心架构与技术壁垒

### 1. 零运行时分配与双重对象池 (Zero Runtime Allocation)
* **架构痛点**：在弹幕游戏的高频实体交互中，传统的 `new/delete` 动态内存分配会引发严重的内存碎片化与系统调用开销。
* **底层实现**：
    * **内存锁死**：引擎初始化阶段利用 `std::vector::reserve` 硬性锁死 `Enemy` (容量 100) 和 `Bullet` (容量 30) 的内存上限，杜绝运行时扩容（Reallocation）开销。
    * **ABA 问题防御**：针对对象池特有的“僵尸对象复用引发错误追踪”问题，摒弃了危险的裸指针引用。通过在 `Enemy` 基类植入 `static next_id` 发号器，结合子弹侧的 `unique_id` 引入**代型索引 (Generational Index)** 进行双重校验，从物理层面根除了内存生命周期错乱。

### 2. 空间划分与碰撞降维 (QuadTree Spatial Partitioning)
* **架构痛点**：同屏海量实体的 $O(N \times M)$ 双重循环碰撞检测是制约主线程帧率的绝对瓶颈。
* **底层实现**：
    * **四叉树降维**：实装动态四叉树数据结构，结合 AABB (轴对齐包围盒) 逆向排除法进行区域剪枝，将全局碰撞时间复杂度从指数级降维至近似 $O(N \log M)$。
    * **RAII 内存防线**：严格遵循 C++ 资源获取即初始化 (RAII) 规范，在 `~QuadTree` 析构函数中级联释放子节点，配合 `= delete` 斩断拷贝与赋值操作，杜绝 Double Free（双重释放）与内存泄漏。

### 3. GPU 离屏渲染与后处理 (Off-screen Rendering & GLSL)
* **架构痛点**：剥离纯 CPU 光栅化画图的视觉局限，接轨现代游戏引擎的 GPU 渲染流。
* **底层实现**：
    * **离屏渲染**：截获默认 Framebuffer，将主渲染流投射至 FBO 隐形画布 (`RenderTexture2D`)。
    * **光污染着色器**：编写纯 GLSL 片段着色器 (`bloom.fs`)，运用**卷积采样算法 (Convolution Blur)** 结合引擎层的加法混合 (`BLEND_ADDITIVE`) 状态，在零外部美术资源的前提下实现全屏高保真泛光 (Bloom) 特效。

### 4. 工业级引擎模块化解耦
* **外观模式 (Facade)**：构建 `Game` 类作为游戏世界的统一流转入口，彻底封装四叉树刷新、粒子分发与实体碰撞，实现平台调度层 (`main.cpp`) 与核心业务逻辑的物理隔离。
* **时空解耦 (Time Decoupling)**：严格剥离逻辑帧 (`FixedUpdate`, $1/60s$) 与渲染帧 (`DeltaTime`)，引入累加器 (Accumulator) 机制，保障各刷新率显示器下的物理推演绝对一致性。
* **单例与工厂模式**：基于 Meyers Singleton 范式构建 `GameManager`，解决全局状态越权篡改问题；通过 `EnemySpawner` 封装动态难度推演算法 (DDA)，极大提升主循环内聚度。

## 🛠️ 技术栈
* **核心语言**: C++ 11/14 (GCC MinGW)
* **图形与数学库**: Raylib 4.5
* **架构范式**: Object Pool, QuadTree, Facade, Singleton, Factory, FSM, RAII

---

## 📅 开发日志 (Dev Log & Issue Tracking)

### [2026-03-17] 架构解耦与设计模式落地 (Facade / Singleton)
* **Feature**: 全面引入外观模式 (Facade)，重构臃肿的 `main.cpp` 上帝类，将物理管线、特效分发全面内聚至 `Game` 类；利用 `GameState` 状态机无缝实现 Title 界面与 `Reset()` 重启闭环。
* **Feature**: 引入 Meyers Singleton 模式构建 `GameManager` 统管全局分数与摄像机震动；引入工厂模式 `EnemySpawner` 剥离刷怪与难度算法。
* **BugFix (嵌套惩罚修复)**：深度 Code Review 发现“玩家受击判定”被错误嵌套在“遍历子弹”的循环内部，导致单帧承受 $30$ 倍碰撞探测开销。抽离至同级管线后，恢复 $O(1)$ 玩家碰撞开销。
* **BugFix (变量遮蔽隐患)**：排查并修复 `Game.cpp` 中因作用域引发的变量遮蔽问题（局部 `boundary` 覆盖成员变量），拯救了因 AABB 范围失效导致的物理瘫痪。

### [2026-03-01] 空间划分与物理管线降维 (QuadTree)
* **Feature**: 接入 QuadTree 空间划分结构，利用极小范围的 `searchRange` 替代全屏搜索，完成物理碰撞管线的 $O(N \log M)$ 降维打击。
* **Feature**: 遵循 RAII 机制重写析构函数，利用 `std::vector::insert` 与迭代器进行底层内存批量拷贝；坚持 `-Wsign-compare` 零警告的严苛编译标准。

### [2026-02-25] 离屏渲染与后处理着色器 (FBO & GLSL)
* **Feature**: 引入 FBO 离屏渲染架构，开辟显存构建隐形画布；自主编写 `bloom.fs` 片段着色器实现工业级泛光特效。
* **BugFix (V-Sync 死锁掉帧)**：勘破双重缓冲底层天机，修复因重复调用 `SwapBuffers` 导致错过垂直同步信号，引发帧率腰斩至 30FPS 的严重卡顿 Bug。
* **BugFix (坐标系翻转)**：解决了 OpenGL 底层纹理坐标系与屏幕坐标系的 Y 轴翻转 (Y-Flip) 冲突。

### [2026-02-23] 时空法则解耦与视觉升维
* **Feature**: 彻底解耦 `FixedUpdate` (物理) 与 `DeltaTime` (渲染)，解决高刷显示器导致的逻辑加速问题。
* **Feature**: 废弃基础圆形，实装三角形与倒三角形实体；基于环形缓冲区 (Ring Buffer) 算法实装战机前世残影；引入基于速度向量的动态几何拉伸。
* **Feature**: 剥离渲染与物理循环，实装顿帧 (Hit Stop) 机制；引入摄像机矩阵偏移，实现受击物理震动。

### [2026-02-22] 制导武器系统与对象池 ABA 防御
* **Feature**: 基于向量归一化数学模型 ($Dx, Dy / L$) 实现追踪导弹推演。
* **BugFix (除零崩溃防御)**：实装底层距离阈值校验（`L > 0.0001f`），杜绝坐标极度重合时引发的 Divide by Zero 崩溃。
* **BugFix (ABA 僵尸夺舍)**：彻底摒弃对象池裸指针寻址。引入 C++ 静态成员 `next_id` 发号器与 `unique_id` 双重信物校验，完美解决子弹追踪错乱的内存生命周期危机。
* **Feature**: 封装连杀状态机，实现击杀 5 敌武器升级及受击重置反馈。

### [2026-02-19] 纯代码粒子渲染与工程化解耦
* **Feature**: 摒弃虚函数表，基于连续内存数组构建 500 容量轻量级粒子对象池。利用极坐标转笛卡尔坐标 ($cos, sin$) 实现全向爆破，结合归一化时间插值 ($Lerp$) 实现平滑褪色。
* **Feature**: 实装基于纯数据结构的多层视差滚动星空 (Parallax Starfield)。
* **Refactor**: 抽离硬编码参数至 `Config.h`，确立配置驱动开发范式；引入 `MeasureText` 适配多分辨率 UI 动态居中。

### [2026-02-18] 零分配对象池与生存机制闭环
* **Feature**: 彻底废弃 `push_back` 带来的运行时开销，实装 `Bullet` 与 `Enemy` 的双重对象池。
* **Feature**: 基于 `enum class GameState` 构建游戏状态流程；实装血量系统 (HP) 与基于 `GetTime()` 算法的无敌帧 (I-Frames) 闪烁反馈。
* **BugFix**: 修复“击杀同帧内敌人仍能造成物理伤害”的逻辑漏洞。

### [2026-02-17] 数据驱动与动态难度 (DDA)
* **Feature**: 构建 `GameConfig` 命名空间，将所有魔法数字（Magic Numbers）转化为 `constexpr` 编译期常量。
* **Feature**: 实装线性反馈的动态难度算法，随分数动态计算生成频率 (60 帧压缩至 20 帧) 与敌机基础初速度增量。

### [2026-02-16] 基础物理架构重构
* **Feature**: 建立基类 `Entity` 规范化通用属性，拆分独立文件体系。
* **Feature**: 基于基础圆心距离测算 (`CheckCollisionCircles`) 构建物理碰撞原型，完成 WASD 操控流与基础坐标更新管线。