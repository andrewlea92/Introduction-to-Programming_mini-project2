#include <allegro5/base.h>
#include <random>
#include <string>

#include "Collider.hpp"
#include "DirtyEffect.hpp"
#include "Defense.hpp"
#include "ArmyBullet.hpp"
#include "GameEngine.hpp"
#include "Group.hpp"
#include "IObject.hpp"
#include "IScene.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Sprite.hpp"
#include "ExplosionEffect.hpp"

class Defense;

ArmyBullet::ArmyBullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Army* parent) :
    Bullet(img, speed, damage, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent) {
    PlayScene* scene = getPlayScene();
    int x = static_cast<int>(floor(Position.x / scene->BlockSize));
    int y = static_cast<int>(floor(Position.y / scene->BlockSize));
    Engine::Point pos(x -1 , y);
    bound = pos;
}

void ArmyBullet::OnExplode(Defense* defense) {
    // Delete
    getPlayScene()->EffectGroup->AddNewObject(new ShootEffect(defense->Position.x , defense->Position.y));
}

void ArmyBullet::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    PlayScene* scene =   getPlayScene();
    // Can be improved by Spatial Hash, Quad Tree, ...
    // However simply loop through all enemies is enough for this program.
    for (auto& it : scene->DefenseGroup->GetObjects()) {
        Defense* defense = dynamic_cast<Defense*>(it);
        if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, defense->Position, defense->CollisionRadius)) {
            OnExplode(defense);
            defense->Hit(damage);
            getPlayScene()->BulletGroup->RemoveObject(objectIterator);
            return;
        }
    }
    for (auto& it : scene->WallGroup->GetObjects()) {
        Defense* defense = dynamic_cast<Defense*>(it);
        if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, defense->Position, defense->CollisionRadius)) {
            OnExplode(defense);
            defense->Hit(damage);
            getPlayScene()->BulletGroup->RemoveObject(objectIterator);
            return;
        }
    }
    
    // Check if out of boundary.
    if (Position.x < 0 || Position.x > PlayScene::GetClientSize().x || Position.y < 0 || Position.y > PlayScene::GetClientSize().y)
        getPlayScene()->BulletGroup->RemoveObject(objectIterator);
}
