#ifndef ArcherBullet_hpp
#define ArcherBullet_hpp

#include "ArmyBullet.hpp"

class Army;
class Defense;
namespace Engine {
struct Point;
}  // namespace Engine

class ArcherBullet: public ArmyBullet{
public:
    explicit ArcherBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Army* parent);
    void OnExplode(Defense* defense) override;
};


#endif /* ArcherBullet_hpp */
