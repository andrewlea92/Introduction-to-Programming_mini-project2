#ifndef ArmyBullet_hpp
#define ArmyBullet_hpp

#include "Bullet.hpp"

class Army;
class Defense;

namespace Engine {
struct Point;
}  // namespace Engine

class ArmyBullet : public Bullet {
private:
    Engine::Point bound;
public:
    explicit ArmyBullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Army* parent);
    virtual void OnExplode(Defense* defense);
    void Update(float deltaTime) override;
};

#endif /* ArmyBullet_hpp */
