#ifndef WallDefense_hpp
#define WallDefense_hpp

#include "Defense.hpp"

class WallDefense: public Defense{
public:
    WallDefense(float x, float y);
    void Update(float deltaTime) override;
    void CreateBullet(Engine::Point pt) override;
    void Hit(float damage) override;
    void UpdateBrokenWall();
};

#endif /* WallDefense_hpp */
