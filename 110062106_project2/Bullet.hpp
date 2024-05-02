#ifndef BULLET_HPP
#define BULLET_HPP
#include <string>

#include "Sprite.hpp"

class Enemy;
class PlayScene;
class Turret;
class Army;
class Defense;

namespace Engine {
struct Point;
}  // namespace Engine

class Bullet : public Engine::Sprite {
protected:
	float speed;
	float damage;
	Turret* parent;
	Enemy* Eparent;
    Army* armyParent;
    Defense* defenseParent;
	PlayScene* getPlayScene();
    virtual void OnExplode(Army* army);
public:
    Army* Target = nullptr;
    explicit Bullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Army* armyParent);
    explicit Bullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Defense* defenseParent);
	void Update(float deltaTime) override;
};
#endif // BULLET_HPP
