#include "Enemy.h"
#include <vector>

struct Boundary{
    float x;
    float y;
    float halfWidth;
    float halfHeight;

    bool contains(float px,float py)const{
        return (px>=x-halfWidth&&px<=x+halfWidth&&py>=y-halfHeight&&py<=y+halfHeight);
    }

    bool intersects(Boundary range) const{
        return !(range.x-range.halfWidth>x+halfWidth||
                 range.x+range.halfWidth<x-halfWidth||
                 range.y+range.halfHeight<y-halfHeight||
                 range.y-range.halfHeight>y+halfHeight);
    }
};

class QuadTree {
private:
    Boundary boundary;
    unsigned int capacity;

    std::vector<Enemy*> enemies;

    bool divided;

    QuadTree* northwest;
    QuadTree* northeast;
    QuadTree* southwest;
    QuadTree* southeast;

    void subdivide(){
        float x = boundary.x;
        float y = boundary.y;
        float w = boundary.halfWidth;
        float h = boundary.halfHeight;

        Boundary nw = {x-w/2,y-h/2,w/2,h/2};
        Boundary ne = {x+w/2,y-h/2,w/2,h/2};
        Boundary sw = {x-w/2,y+h/2,w/2,h/2};
        Boundary se = {x+w/2,y+h/2,w/2,h/2};

        northwest = new QuadTree(nw, capacity);
        northeast = new QuadTree(ne,capacity);
        southeast = new QuadTree(se,capacity);
        southwest = new QuadTree(sw,capacity);

        divided = true;
    }
    

public:
    QuadTree(Boundary b,int cap){
        boundary = b;
        capacity = cap;
        divided = false;

        northwest = nullptr;
        northeast = nullptr;
        southeast = nullptr;
        southwest = nullptr;
    }

    bool insert(Enemy* e){
        // 1. 不在我的辖区？滚蛋！
        if (!boundary.contains(e->x,e->y)) return false;

        // 2. 还有空位？关进我的电脑里！
        if(enemies.size()<capacity){
            enemies.push_back(e);
            return true;
        }

        // 3. 没空位了！而且还没裂变过？立刻裂变！
        if (!divided){
            subdivide();
        }

        // 4. 踢皮球：让四个小派出所去试着收押这个小偷，谁收进去了算谁的！
        if (northwest->insert(e)) return true;
        if (northeast->insert(e)) return true;
        if (southwest->insert(e)) return true;
        if (southeast->insert(e)) return true;

        return false;

    }

    ~QuadTree() {
        // 如果我曾经裂变过，必须把底下四个小派出所彻底拆掉！
        if (divided) {
            delete northwest;
            delete northeast;
            delete southwest;
            delete southeast;
        }
    }

    std::vector<Enemy*> query(Boundary range){
        std::vector<Enemy*> found;

        // 1. 如果警察的网跟我这片辖区压根没重叠，直接交白卷！(剪枝)
        if(!boundary.intersects(range)){
            return found;
        }

        for (auto e:enemies){
            if(range.contains(e->x,e->y)){
                found.push_back(e);
            }
        }

        if(divided){
            std::vector<Enemy*> nwFound = northwest->query(range);
            //内存的批量拷贝(直接将名单加在原名单末尾)
            found.insert(found.end(),nwFound.begin(),nwFound.end());

            std::vector<Enemy*> neFound = northeast->query(range);
            found.insert(found.end(),neFound.begin(),neFound.end());
            
            std::vector<Enemy*> swFound = southwest->query(range);
            found.insert(found.end(),swFound.begin(),swFound.end());

            std::vector<Enemy*> seFound = southeast->query(range);
            found.insert(found.end(),seFound.begin(),seFound.end());
        }

        return found;
    }
};
