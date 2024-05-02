#ifndef Defense_hpp
#define Defense_hpp

#include <allegro5/base.h>
#include <list>
#include <string>

#include "Role.hpp"

class Army;
class PlayScene;

class Defense: public Role {
protected:
    float shootRadius;
    float coolDown;
    float reload = 0;
    float rotateRadian = 2 * ALLEGRO_PI;    
    std::list<Defense*>::iterator lockedDefenseIterator;
    virtual void CreateBullet(Engine::Point pt) = 0;
    
public:
    int id;
    bool Enabled = true;
    bool Preview = false;
    Army* Target = nullptr;
    std::list<Army*> lockedArmies;
    Defense(std::string imgDefense, float x, float y, float radius, float coolDown, int hp, int id, float shootRadius);
    void Hit(float damage) override;
    void Update(float deltaTime) override;
    void Draw() const override;
    bool InShootingRange(Engine::Point obj);
    void setreload(int a)
    {
        reload = a;
    }
    void cool(float a)
    {
        coolDown *= a;
    }
};

#endif /* Defense_hpp */
