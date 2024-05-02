#include <allegro5/base.h>
#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <string>
#include <limits>
#include "LOG.hpp"
#include "AudioHelper.hpp"
#include "Collider.hpp"
#include "Army.hpp"
#include "Group.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "BombArmy.hpp"
#include "Defense.hpp"
#include <vector>



//
#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <random>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "AudioHelper.hpp"
#include "Bullet.hpp"
#include "DirtyEffect.hpp"
#include "Army.hpp"
#include "ArmyBullet.hpp"
#include "ExplosionEffect.hpp"
#include "GameEngine.hpp"
#include "Group.hpp"
#include "IScene.hpp"
#include "LOG.hpp"
#include "PlayScene.hpp"
#include "Defense.hpp"
#define TOP_LEFT 0
#define TOP_RIGHT 1
#define BOTTOM_LEFT 2
#define BOTTOM_RIGHT 3

#define LEFT 0
#define UP 1
#define RIGHT 2
#define DOWN 3

//Army(std::string img, float x, float y, float radius, float coolDown, float speed, float hp, int id, float shootRadius);
BombArmy::BombArmy(float x, float y) :
    Army("play/lyna.png", x, y, 20, 0, 80, 15, 1, 0) {
    // Move center downward, since we the army head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}
TankArmy::TankArmy(float x, float y) :
    Army("play/boy2.png", x, y, 20, 0, 80, 120, 2, 0) {
    // Move center downward, since we the army head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}
IceArmy::IceArmy(float x, float y) :
    //al_start_timer(timer);
    Army("play/ice2.png", x, y, 20, 0, 80, 120, 3, 0) {
    timer=al_create_timer(1.0f/60);
    // Move center downward, since we the army head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}
AkaArmy::AkaArmy(float x, float y) :
    //al_start_timer(timer);
    Army("play/aka2.png", x, y, 20, 1, 55, 25, 4, 1.5 * PlayScene::BlockSize) {
    timer = al_create_timer(1.0f / 60);
    // Move center downward, since we the army head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}
void BombArmy::Update(float deltaTime) {
    // PlayScene
    PlayScene* scene = getPlayScene();

    if (isPreview) return ;
    
    // position
    int x = static_cast<int>(floor(Position.x / PlayScene::BlockSize));
    int y = static_cast<int>(floor(Position.y / PlayScene::BlockSize));
    
    if (!Target) {
        // Lock closet target
        // Can be improved by Spatial Hash, Quad Tree, ...
        // However simply loop through all enemies is enough for this program.
        
        // TODO 2 (6/8): Lock the closet wall. If there's no wall on the map, it will lock the closet defense.
        // For the simplicity, we use manHattan distance to measure the distance bewteen objects. You can use the ManHattanDistance() function in Army class directly for calculation.
        int maxDis = INT_MAX;
        Defense* tgt = nullptr;
        for (auto& it : scene->WallGroup->GetObjects()) {
            int dis = ManHattanDistance(it->Position);
            if (dis < maxDis) {
                maxDis = dis;
                tgt = dynamic_cast<Defense*>(it);
            }
            movingToWall = true;
        }

        maxDis = INT_MAX;

        if (tgt == nullptr) {
            movingToWall = false;
            for (auto& it : scene->DefenseGroup->GetObjects()) {
                int dis = ManHattanDistance(it->Position);
                if (dis < maxDis) {
                    maxDis = dis;
                    tgt = dynamic_cast<Defense*>(it);
                }
            }
        }

        if (tgt) {
            Target = tgt;
            Target->lockedArmies.push_back(this);
            lockedArmyIterator = std::prev(Target->lockedArmies.end());
        }
        
        // TODO 2 (7/8): Store the closet target in Target, and update lockedArmyIterator. You can imitate the same part in Defense::Update().
        // Also, record the target is wall or a noraml defense.
        //movingToWall = true;


    }
    if (Target) {
        Rotation = UpdateRotation(deltaTime, Target->Position);
        // Bomb cannot shoot, so the reload time just set as coolDown.
        reload = coolDown;
        
        // TODO 2 (8/8): If bomb army is on the same block with target. Explode itself to deal damage to the target. Otherwise, move toward the target.
        if (/*(Position.x == Target->Position.x) && (Target->Position.y == Position.y)*/Engine::Collider::IsCircleOverlap(Position, CollisionRadius, Target->Position, Target->CollisionRadius) /* need to modify */) {
            // Notice that bomb army deals different damage to wall and normal target.
            //if(Target.)
            if (movingToWall) {
                Target->Hit(500); // Wall HP = 500
            }
            else {
                Target->Hit(10); // Tower HP  = 50
            }
            Hit(INFINITY);
        }
        else {
            int dx = Target->Position.x - Position.x;
            int dy = Target->Position.y - Position.y;
            double len = sqrt(pow(dx, 2) + pow(dy, 2));
            Velocity = speed * Engine::Point(dx / len, dy / len);
        }
        
    }

    Position.x += Velocity.x * deltaTime;
    Position.y += Velocity.y * deltaTime;
}

// Since the bomb army cannot shoot, the function doesn't need to do anything.
void BombArmy::CreateBullet(Engine::Point pt) {}

// TODO 2 (5/8): You can imitate the hit function in Army class. Notice that the bomb army won't have explosion effect.
void BombArmy::Hit(float damage) {

    HP -= damage;
    if (damage == 3)
    {
        speed = 30;
    }
    if (HP <= 0) {
        OnExplode();
        // Remove all Defense's reference to target.
        for (auto& it : lockedDefenses)
            it->Target = nullptr;
        getPlayScene()->ArmyGroup->RemoveObject(objectIterator);
        //AudioHelper::PlayAudio("explosion.wav");
    }
}
void TankArmy::Hit(float damage) {

    HP -= damage;
    if (HP <= 0) {
        OnExplode();
        // Remove all Defense's reference to target.
        for (auto& it : lockedDefenses)
            it->Target = nullptr;
        getPlayScene()->ArmyGroup->RemoveObject(objectIterator);
        //AudioHelper::PlayAudio("explosion.wav");
    }
}
void IceArmy::Hit(float damage) {

    HP -= damage;
    if (HP <= 0) {
        OnExplode();
        // Remove all Defense's reference to target.
        //for (auto& it : lockedDefenses)
            //it->Target = nullptr;
        getPlayScene()->IceGroup->RemoveObject(objectIterator);
        //getPlayScene()->ArmyGroup->RemoveObject(objectIterator);
        //AudioHelper::PlayAudio("explosion.wav");
    }
}
void AkaArmy::Hit(float damage) {
    PlayScene* scene = getPlayScene();
    HP -= damage;
    if (HP <= 0) {
        for (auto& it : scene->DefenseGroup->GetObjects())
        {
            Defense* tgt = dynamic_cast<Defense*>(it);
            tgt->cool(0.333);
        }
        OnExplode();
        // Remove all Defense's reference to target.
        for (auto& it : lockedDefenses)
            it->Target = nullptr;
        getPlayScene()->ArmyGroup->RemoveObject(objectIterator);
        //AudioHelper::PlayAudio("explosion.wav");
    }
}
void AkaArmy::Update(float deltaTime){
    if (isPreview) return;
    PlayScene* scene = getPlayScene();
    if (flag == 0)
    {
        flag = 1;
        //al_start_timer(timer);
        for (auto& it : scene->DefenseGroup->GetObjects())
        {
                Defense* tgt = dynamic_cast<Defense*>(it);
                tgt->cool(3);
        }
    }
    /*if (flag == 1)
    {
        if (al_get_timer_count(timer) / 60 >= 5)
        {
        al_stop_timer(timer);
        al_destroy_timer(timer);
        for (auto& it : scene->DefenseGroup->GetObjects())
        {
            Defense* tgt = dynamic_cast<Defense*>(it);
            tgt->cool(0.25);
        }
        //Hit(10000);
        flag = 2;
        }
        
    }*/

//PlayScene* scene = getPlayScene();

const int distThreshold = PlayScene::BlockSize / 32;

if (isPreview) return;

// position
int x = static_cast<int>(floor(Position.x / PlayScene::BlockSize));
int y = static_cast<int>(floor(Position.y / PlayScene::BlockSize));

if (region == 5) {
    if (!Target) {
        // Lock closet target
        // Can be improved by Spatial Hash, Quad Tree, ...
        // However simply loop through all enemies is enough for this program.
        int maxDis = INT_MAX;
        Defense* tgt = nullptr;
        for (auto& it : scene->DefenseGroup->GetObjects()) {
            int dis = ManHattanDistance(it->Position);
            if (dis < maxDis) {
                maxDis = dis;
                tgt = dynamic_cast<Defense*>(it);
            }
        }
        if (tgt) {
            Target = tgt;
            Target->lockedArmies.push_back(this);
            lockedArmyIterator = std::prev(Target->lockedArmies.end());
        }
    }
    if (Target) {
        Rotation = UpdateRotation(deltaTime, Target->Position);
        // Shoot reload.
        reload -= deltaTime;
        if (reload <= 0) {
            // shoot.
            reload = coolDown;
            if (InShootingRange(Target->Position))
                CreateBullet(Target->Position);
        }
        if (InShootingRange(Target->Position)) {
            Velocity = Engine::Point(0, 0);
        }
        else {
            float tx = Target->Position.x;
            float ty = Target->Position.y;

            if (dir == 1) {
                if (abs(tx - Position.x) > distThreshold) {
                    Velocity = (Position.x < tx) ? Engine::Point(1, 0) : Engine::Point(-1, 0);
                }
                else {
                    Position.x = tx;
                    Velocity = (Position.y < ty) ? Engine::Point(0, 1) : Engine::Point(0, -1);
                }
            }
            else {
                if (abs(ty - Position.y) > distThreshold) {
                    Velocity = (Position.y < ty) ? Engine::Point(0, 1) : Engine::Point(0, -1);
                }
                else {
                    Position.y = ty;
                    Velocity = (Position.x < tx) ? Engine::Point(1, 0) : Engine::Point(-1, 0);
                }
            }
        }
    }
}
else { // region != 5
    CalcRegion(x, y);
    if (!movingToWall) {
        // top
        if (region == 1 || region == 2 || region == 3) {
            if (!scene->brokenWall[UP].empty()) {
                movingToWall = true;
                int minDis = INT_MAX;
                for (auto wall : scene->brokenWall[UP]) {
                    int dis = ManHattanDistance(wall * PlayScene::BlockSize);
                    if (minDis > dis) {
                        minDis = dis;
                        wallPos = wall * PlayScene::BlockSize + Engine::Point(PlayScene::BlockSize / 2, PlayScene::BlockSize / 2);
                    }
                }
                dir = 0;
            }
        }
        // down
        if (region == 7 || region == 8 || region == 9) {
            if (!scene->brokenWall[DOWN].empty()) {
                movingToWall = true;
                int minDis = INT_MAX;
                for (auto wall : scene->brokenWall[DOWN]) {
                    int dis = ManHattanDistance(wall * PlayScene::BlockSize);
                    if (minDis > dis) {
                        minDis = dis;
                        wallPos = wall * PlayScene::BlockSize + Engine::Point(PlayScene::BlockSize / 2, PlayScene::BlockSize / 2);
                    }
                }
                dir = 0;
            }
        }
        // left
        if (region == 1 || region == 4 || region == 7) {
            if (!scene->brokenWall[LEFT].empty()) {
                movingToWall = true;
                int minDis = INT_MAX;
                for (auto wall : scene->brokenWall[LEFT]) {
                    int dis = ManHattanDistance(wall * PlayScene::BlockSize);
                    if (minDis > dis) {
                        minDis = dis;
                        wallPos = wall * PlayScene::BlockSize + Engine::Point(PlayScene::BlockSize / 2, PlayScene::BlockSize / 2);
                    }
                }
                dir = 1;
            }
        }
        // right
        if (region == 3 || region == 6 || region == 9) {
            if (!scene->brokenWall[RIGHT].empty()) {
                movingToWall = true;
                int minDis = INT_MAX;
                for (auto wall : scene->brokenWall[RIGHT]) {
                    int dis = ManHattanDistance(wall * PlayScene::BlockSize);
                    if (minDis > dis) {
                        minDis = dis;
                        wallPos = wall * PlayScene::BlockSize + Engine::Point(PlayScene::BlockSize / 2, PlayScene::BlockSize / 2);
                    }
                }
                dir = 1;
            }
        }
    }

    // there's no broken wall
    if (!movingToWall) {
        if (!Target) {
            // Lock closet wall
            // Can be improved by Spatial Hash, Quad Tree, ...
            // However simply loop is enough for this program.
            int maxDis = INT_MAX;
            Defense* tgt = nullptr;
            for (auto& it : scene->WallGroup->GetObjects()) {
                int dis = ManHattanDistance(it->Position);
                if (dis < maxDis) {
                    maxDis = dis;
                    tgt = dynamic_cast<Defense*>(it);
                }
            }
            if (tgt) {
                Target = tgt;
                Target->lockedArmies.push_back(this);
                lockedArmyIterator = std::prev(Target->lockedArmies.end());
            }
        }
        else {
            Rotation = UpdateRotation(deltaTime, Target->Position);

            // Shoot reload.
            reload -= deltaTime;
            if (reload <= 0) {
                // shoot.
                reload = coolDown;
                if (InShootingRange(Target->Position))
                    CreateBullet(Target->Position);
            }
            if (InShootingRange(Target->Position)) {
                Velocity = Engine::Point(0, 0);
            }
            else {
                float tx = Target->Position.x;
                float ty = Target->Position.y;

                if (dir == 0) {
                    if (abs(tx - Position.x) > distThreshold) {
                        Velocity = (Position.x < tx) ? Engine::Point(1, 0) : Engine::Point(-1, 0);
                    }
                    else {
                        Position.x = tx;
                        Velocity = (Position.y < ty) ? Engine::Point(0, 1) : Engine::Point(0, -1);
                    }
                }
                else {
                    if (abs(ty - Position.y) > distThreshold) {
                        Velocity = (Position.y < ty) ? Engine::Point(0, 1) : Engine::Point(0, -1);
                    }
                    else {
                        Position.y = ty;
                        Velocity = (Position.x < tx) ? Engine::Point(1, 0) : Engine::Point(-1, 0);
                    }
                }
            }
        }
    }
    else {
        if (Target) {
            Target = nullptr;
        }
        Rotation = UpdateRotation(deltaTime, wallPos);
        // Shoot reload.
        reload = coolDown;
        // position
        float wx = wallPos.x;
        float wy = wallPos.y;

        if (abs(wx - Position.x) < distThreshold && abs(wy - Position.y) < distThreshold) {
            Position = wallPos;
            movingToWall = false;
            region = 5;
            Velocity = Engine::Point(0, 0);
        }
        else {
            if (dir == 0) {
                if (abs(wx - Position.x) > distThreshold) {
                    Velocity = (Position.x < wx) ? Engine::Point(1, 0) : Engine::Point(-1, 0);
                }
                else {
                    Position.x = wx;
                    Velocity = (Position.y < wy) ? Engine::Point(0, 1) : Engine::Point(0, -1);
                }
            }
            else {
                if (abs(wy - Position.y) > distThreshold) {
                    Velocity = (Position.y < wy) ? Engine::Point(0, 1) : Engine::Point(0, -1);
                }
                else {
                    Position.y = wy;
                    Velocity = (Position.x < wx) ? Engine::Point(1, 0) : Engine::Point(-1, 0);
                }
            }
        }
    }
}
Velocity = Velocity * speed;
Position.x += Velocity.x * deltaTime;
Position.y += Velocity.y * deltaTime;
}