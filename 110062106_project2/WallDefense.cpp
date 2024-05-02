#include <allegro5/base.h>
#include <cmath>
#include <iostream>
#include <string>

#include "AudioHelper.hpp"
#include "Group.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Army.hpp"
#include "WallDefense.hpp"

#define TOP_LEFT 0
#define TOP_RIGHT 1
#define BOTTOM_LEFT 2
#define BOTTOM_RIGHT 3

#define LEFT 0
#define UP 1
#define RIGHT 2
#define DOWN 3


//Defense(std::string imgDefense, float x, float y, float radius, float coolDown, int hp, int id, float shootRadius);
WallDefense::WallDefense(float x, float y) : Defense("play/wall.png", x, y, 20, 10, 500, 1, 0) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}

void WallDefense::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    if (!Enabled)
        return ;
}

void WallDefense::CreateBullet(Engine::Point pt) {}

void WallDefense::Hit(float damage) {
    HP -= damage;
    if (HP <= 0) {
        OnExplode();
        // Remove all armies' reference to target
        for (auto& it : lockedArmies)
            it->Target = nullptr;
        
        // update playscene brokenwall
        UpdateBrokenWall();
        
        int x = static_cast<int>(floor(Position.x / PlayScene::BlockSize));
        int y = static_cast<int>(floor(Position.y / PlayScene::BlockSize));
        getPlayScene()->ClearMapState(x, y);
        getPlayScene()->WallGroup->RemoveObject(objectIterator);
        AudioHelper::PlayAudio("explosion.wav");
    }
}

void WallDefense::UpdateBrokenWall() {
    PlayScene *scene = getPlayScene();
    
    // Position
    int x = static_cast<int>(floor(Position.x / PlayScene::BlockSize));
    int y = static_cast<int>(floor(Position.y / PlayScene::BlockSize));
    
    // up
    if (y == scene->corners[TOP_LEFT].y && x > scene->corners[TOP_LEFT].x && x < scene->corners[TOP_RIGHT].x) {
        scene->brokenWall[UP].push_back(Engine::Point(x, y));
    }
    
    // down
    else if (y == scene->corners[BOTTOM_LEFT].y && x > scene->corners[BOTTOM_LEFT].x && x < scene->corners[BOTTOM_RIGHT].x) {
        scene->brokenWall[DOWN].push_back(Engine::Point(x, y));
    }
    
    // left
    else if (x == scene->corners[TOP_LEFT].x && y > scene->corners[TOP_LEFT].y && y < scene->corners[BOTTOM_LEFT].y) {
        scene->brokenWall[LEFT].push_back(Engine::Point(x, y));
    }
    
    // right
    else if (x == scene->corners[TOP_RIGHT].x && y > scene->corners[TOP_RIGHT].y && y < scene->corners[BOTTOM_RIGHT].y) {
        scene->brokenWall[RIGHT].push_back(Engine::Point(x, y));
    }
}
