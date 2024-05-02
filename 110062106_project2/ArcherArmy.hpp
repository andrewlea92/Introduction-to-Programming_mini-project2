#ifndef ArcherArmy_hpp
#define ArcherArmy_hpp

#include "Army.hpp"

class ArcherArmy: public Army {
public:
    ArcherArmy(int x, int y);
    void CreateBullet(Engine::Point pt) override;
};

#endif /* ArcherArmy_hpp */
