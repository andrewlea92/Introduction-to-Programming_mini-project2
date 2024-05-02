#include "Bullet.hpp"
#include "Collider.hpp"
#include "Army.hpp"
#include "GameEngine.hpp"
#include "Group.hpp"
#include "IObject.hpp"
#include "IScene.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Sprite.hpp"

PlayScene* Bullet::getPlayScene() {
	return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
void Bullet::OnExplode(Army* army) {
}

// army
Bullet::Bullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Army* armyParent) :
    Sprite(img, position.x, position.y), speed(speed), damage(damage), armyParent(armyParent) {
    Velocity = forwardDirection * speed;
    Rotation = rotation;
    CollisionRadius = 4;
}

// defense
Bullet::Bullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Defense* defenseParent) :
    Sprite(img, position.x, position.y), speed(speed), damage(damage), defenseParent(defenseParent) {
    Velocity = forwardDirection * speed;
    Rotation = rotation;
    CollisionRadius = 4;
}

void Bullet::Update(float deltaTime) {
	Sprite::Update(deltaTime);
	PlayScene* scene = getPlayScene();

    for (auto& it : scene->ArmyGroup->GetObjects()) {
        Army* army= dynamic_cast<Army*>(it);
        if (!army->Visible)
            continue;
        if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, army->Position, army->CollisionRadius)) {
            army->Hit(damage);
            getPlayScene()->BulletGroup->RemoveObject(objectIterator);
            return;
        }
    }
	// Check if out of boundary.
    if (Position.x < 0 || Position.x > PlayScene::GetClientSize().x || Position.y < 0 || Position.y > PlayScene::GetClientSize().y)
        getPlayScene()->BulletGroup->RemoveObject(objectIterator);
}
