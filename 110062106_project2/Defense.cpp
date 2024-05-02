#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <utility>

#include "AudioHelper.hpp"
#include "Army.hpp"
#include "GameEngine.hpp"
#include "Group.hpp"
#include "IObject.hpp"
#include "IScene.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Defense.hpp"

Defense::Defense(std::string imgDefense, float x, float y, float radius, float coolDown, int hp, int id, float shootRadius) :
    Role(imgDefense, x, y), coolDown(coolDown), id(id), shootRadius(shootRadius) {
    CollisionRadius = radius;
    HP = hp;
}
void Defense::Hit(float damage) {
    HP -= damage;
    if (HP <= 0) {
        OnExplode();
        // Remove all armies' reference to target
        for (auto& it : lockedArmies)
            it->Target = nullptr;
        int x = static_cast<int>(floor(Position.x / PlayScene::BlockSize));
        int y = static_cast<int>(floor(Position.y / PlayScene::BlockSize));
        getPlayScene()->ClearMapState(x, y);
        getPlayScene()->DefenseGroup->RemoveObject(objectIterator);
        AudioHelper::PlayAudio("explosion.wav");
    }
}
void Defense::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    PlayScene* scene = getPlayScene();
    if (!Enabled)
        return;

    if (!Target) {
        // Lock first seen target.
        // Can be improved by Spatial Hash, Quad Tree, ...
        // However simply loop through all enemies is enough for this program.
        int ey;
        for (auto& it : scene->ArmyGroup->GetObjects()) {
            ey = static_cast<int>(floor(it->Position.y / PlayScene::BlockSize));
            if (InShootingRange(it->Position)) {
                Target = dynamic_cast<Army*>(it);
                if (Target->id == 3)
                {
                    Target = nullptr;
                    break;
                }
                Target->lockedDefenses.push_back(this);
                lockedDefenseIterator = std::prev(Target->lockedDefenses.end());
                break;
            }
        }
    }
    if (Target) {
        Engine::Point originRotation = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
        Engine::Point targetRotation = (Target->Position - Position).Normalize();
        float maxRotateRadian = rotateRadian * deltaTime * 2.5;
        float cosTheta = originRotation.Dot(targetRotation);
        // Might have floating-point precision error.
        if (cosTheta > 1) cosTheta = 1;
        else if (cosTheta < -1) cosTheta = -1;
        
        if (cosTheta == -1) Rotation = ALLEGRO_PI;
        else {
            float radian = acos(cosTheta);
            Engine::Point rotation;
            if (abs(radian) <= maxRotateRadian)
                rotation = targetRotation;
            else
                rotation = ((abs(radian) - maxRotateRadian) * originRotation + maxRotateRadian * targetRotation) / radian;
            // Add 90 degrees (PI/2 radian), since we assume the image is oriented upward.
            Rotation = atan2(rotation.y, rotation.x) + ALLEGRO_PI / 2;
        }
        // Shoot reload.
        reload -= deltaTime;
        if (reload <= 0) {
            // shoot.
            reload = coolDown;
                CreateBullet(Target->Position);
        }
    }
}
void Defense::Draw() const {
    Sprite::Draw();
    if (PlayScene::DebugMode) {
        // Draw target radius.
        al_draw_circle(Position.x, Position.y, shootRadius, al_map_rgb(0, 0, 255), 2);
    }
}

bool Defense::InShootingRange(Engine::Point obj) {
    float x = Position.x;
    float y = Position.y;
    
    return (pow(abs(x - obj.x), 2) + pow(abs(y - obj.y), 2) <= pow(shootRadius, 2));
}

