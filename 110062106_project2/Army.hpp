#ifndef Army_hpp
#define Army_hpp

#include <list>
#include <vector>
#include <string>

#include "Point.hpp"
#include "Role.hpp"

class Bullet;
class PlayScene;
class Defense;

class Army : public Role {
protected:
    Engine::Point target_tmp;
    float speed;
    float coolDown = 1;
    float reload = 0;
    float rotateRadian = 2 * ALLEGRO_PI;
    float shootRadius;
    std::list<Army*>::iterator lockedArmyIterator;
public:
    int id;
    bool isPreview = true;
    bool Enabled = true;
    float reachEndTime;
    Defense* Target = nullptr;
    Engine::Point wallPos;
    bool movingToWall = false;
    int dir = -1;
    std::list<Defense*> lockedDefenses;
    int region;
    
    Army(std::string img, float x, float y, float radius, float coolDown, float speed, float hp, int id, float shootRadius);
    // Reference: Design Patterns - Factory Method.
    virtual void CreateBullet(Engine::Point pt) = 0;
    void Hit(float damage) override;
    void Update(float deltaTime) override;
    void Draw() const override;
    bool InShootingRange(Engine::Point obj);
    int ManHattanDistance(Engine::Point target);
    void CalcRegion(int x, int y);
    float UpdateRotation(float deltaTime, Engine::Point target);
};

#endif /* Army_hpp */
