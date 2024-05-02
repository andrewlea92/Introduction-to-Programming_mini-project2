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

Army::Army(std::string img, float x, float y, float radius, float coolDown, float speed, float hp, int id, float shootRadius) :
    Role(img, x, y), coolDown(coolDown), speed(speed), id(id), shootRadius(shootRadius) {
    CollisionRadius = radius;
    HP = hp;
    reachEndTime = 0;
    wallPos = Engine::Point(-1, -1);
    Velocity = Engine::Point(0, 0);
}

void Army::Hit(float damage) {
    HP -= damage;
    if (damage == 3)
    {
        speed = 30;
    }
    if (HP <= 0) {
        OnExplode();
        // Remove all Defense's reference to target.
        for (auto& it: lockedDefenses)
            it->Target = nullptr;
        getPlayScene()->ArmyGroup->RemoveObject(objectIterator);
        AudioHelper::PlayAudio("explosion.wav");
    }
}
void Army::Update(float deltaTime) {
    // PlayScene
    PlayScene* scene = getPlayScene();

    const int distThreshold = PlayScene::BlockSize / 32;
    
    if (isPreview) return ;
    
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
void Army::Draw() const {
    Sprite::Draw();
    if (PlayScene::DebugMode) {
        // Draw collision radius.
        al_draw_circle(Position.x, Position.y, shootRadius, al_map_rgb(255, 0, 0), 2);
    }
}

bool Army::InShootingRange(Engine::Point obj) {
    return (pow(abs(Position.x - obj.x), 2) + pow(abs(Position.y - obj.y), 2) <= pow(shootRadius, 2));
}

int Army::ManHattanDistance(Engine::Point target) {
    int tx = static_cast<int>(floor(target.x / PlayScene::BlockSize));
    int ty = static_cast<int>(floor(target.y / PlayScene::BlockSize));
    
    int mx = static_cast<int>(floor(Position.x / PlayScene::BlockSize));
    int my = static_cast<int>(floor(Position.y / PlayScene::BlockSize));
    
    return abs(tx - mx) + abs(ty - my);
}

void Army::CalcRegion(int x, int y) {
    PlayScene *scene = getPlayScene();
    
    // 1
    if (x < scene->corners[TOP_LEFT].x && y < scene->corners[TOP_LEFT].y) {
        region = 1;
    }
    
    // 2
    if (x >= scene->corners[TOP_LEFT].x && x <= scene->corners[TOP_RIGHT].x && y <= scene->corners[TOP_LEFT].y) {
        region = 2;
    }
    
    // 3
    if (x > scene->corners[TOP_RIGHT].x && y < scene->corners[TOP_RIGHT].y) {
        region = 3;
    }
    
    // 4
    if (x <= scene->corners[TOP_LEFT].x && y >= scene->corners[TOP_LEFT].y && y <= scene->corners[BOTTOM_LEFT].y) {
        region = 4;
    }
    
    // 5
    if (x >= scene->corners[TOP_LEFT].x && x <= scene->corners[TOP_RIGHT].x
        && y >= scene->corners[TOP_LEFT].y && y <= scene->corners[BOTTOM_LEFT].y) {
        region = 5;
    }
    
    // 6
    if (x >= scene->corners[TOP_RIGHT].x && y >= scene->corners[TOP_RIGHT].y && y <= scene->corners[BOTTOM_RIGHT].y) {
        region = 6;
    }
    
    // 7
    if (x < scene->corners[BOTTOM_LEFT].x && y > scene->corners[BOTTOM_LEFT].y) {
        region = 7;
    }
    
    // 8
    if (x >= scene->corners[BOTTOM_LEFT].x && x <= scene->corners[BOTTOM_RIGHT].x && y >= scene->corners[BOTTOM_LEFT].y) {
        region = 8;
    }
    
    // 9
    if (x > scene->corners[BOTTOM_RIGHT].x && y > scene->corners[BOTTOM_RIGHT].y) {
        region = 9;
    }
}

float Army::UpdateRotation(float deltaTime, Engine::Point target) {
    float ret;
    Engine::Point originRotation = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    Engine::Point targetRotation = (target - Position).Normalize();
    float maxRotateRadian = rotateRadian * deltaTime;
    float cosTheta = originRotation.Dot(targetRotation);
    // Might have floating-point precision error.
    if (cosTheta > 1) cosTheta = 1;
    else if (cosTheta < -1) cosTheta = -1;
    
    if (cosTheta == -1) ret = ALLEGRO_PI;
    else {
        float radian = acos(cosTheta);
        Engine::Point rotation;
        if (abs(radian) <= maxRotateRadian)
            rotation = targetRotation;
        else
            rotation = ((abs(radian) - maxRotateRadian) * originRotation + maxRotateRadian * targetRotation) / radian;
        // Add 90 degrees (PI/2 radian), since we assume the image is oriented upward.
        ret = atan2(rotation.y, rotation.x) + ALLEGRO_PI / 2;
    }
    return ret;
}
