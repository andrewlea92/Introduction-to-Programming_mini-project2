#ifndef CannonDefense_hpp
#define CannonDefense_hpp

#include "Defense.hpp"


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

class CannonDefense: public Defense {
public:
    CannonDefense(float x, float y);
    void CreateBullet(Engine::Point pt) override;
};

class CannonDefense2 : public Defense {
public:
    CannonDefense2(float x, float y);
    void CreateBullet(Engine::Point pt) override; 
};

class Trap : public Defense {
public:
    Trap(float x, float y);
    //void CreateBullet(Engine::Point pt) override;
    void Draw() const override{
        if (PlayScene::DebugMode) {
            Sprite::Draw();
            // Draw target radius.
            al_draw_circle(Position.x, Position.y, shootRadius, al_map_rgb(0, 0, 255), 2);
        }
    }
    void Update(float deltaTime) override
    {
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
        if (Target)
        {
            Target->Hit(10000);
            Hit(10000);
        }
    }
    void CreateBullet(Engine::Point pt) override{}
};

#endif /* CannonDefense_hpp */
