#ifndef BombArmy_hpp
#define BombArmy_hpp

#include "Army.hpp"
#include"ExplosionEffect.hpp"
#include <allegro5/base.h>
#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <string>
#include <limits>
#include "Defense.hpp"
#include "ArcherBullet.hpp"

class BombArmy: public Army {
public:
    BombArmy(float x, float y);
    void Update(float deltaTime) override;
    void CreateBullet(Engine::Point pt) override;
    void Hit(float damage) override;
};

class TankArmy : public Army {
public:
    TankArmy(float x, float y);
    void Update(float deltaTime) override {}
    void CreateBullet(Engine::Point pt) override{}
    void Hit(float damage) override;
};

class IceArmy : public Army {
public:
    ALLEGRO_TIMER* timer;
    IceArmy(float x, float y);
    int flag = 0;
    void Update(float deltaTime) override {
        if (isPreview) return;
        PlayScene* scene = getPlayScene();
        float sz = scene->BlockSize;
        if (flag == 0)
        {
            flag = 1;
            al_start_timer(timer);
            getPlayScene()->EffectGroup->AddNewObject(new SnowflakeEffect(Position.x, Position.y));
            getPlayScene()->EffectGroup->AddNewObject(new SnowflakeEffect(Position.x+sz, Position.y));
            getPlayScene()->EffectGroup->AddNewObject(new SnowflakeEffect(Position.x, Position.y+sz));
            getPlayScene()->EffectGroup->AddNewObject(new SnowflakeEffect(Position.x-sz, Position.y));
            getPlayScene()->EffectGroup->AddNewObject(new SnowflakeEffect(Position.x, Position.y-sz));
            getPlayScene()->EffectGroup->AddNewObject(new SnowflakeEffect(Position.x+sz, Position.y+sz));
            getPlayScene()->EffectGroup->AddNewObject(new SnowflakeEffect(Position.x-sz, Position.y-sz));
            getPlayScene()->EffectGroup->AddNewObject(new SnowflakeEffect(Position.x+sz, Position.y-sz));
            getPlayScene()->EffectGroup->AddNewObject(new SnowflakeEffect(Position.x-sz, Position.y+sz));
            
            for (auto& it : scene->DefenseGroup->GetObjects())
            {
                if ((it->Position.x - Position.x <= sz && it->Position.x - Position.x >= sz * -1) && (it->Position.y - Position.y <= sz && it->Position.y - Position.y >= sz * -1))
                {
                    Defense* tgt = dynamic_cast<Defense*>(it);
                    tgt->setreload(10000);
                }
            }

        }
        if (al_get_timer_count(timer) / 60 >= 2)
        {
            al_stop_timer(timer);
            al_destroy_timer(timer);
            for (auto& it : scene->DefenseGroup->GetObjects())
            {
                if ((it->Position.x - Position.x <= sz || it->Position.x - Position.x >= sz * -1) && (it->Position.y - Position.y <= sz || it->Position.y - Position.y >= sz * -1))
                {
                    Defense* tgt = dynamic_cast<Defense*>(it);
                    tgt->setreload(0);
                }
            }
            Hit(10000);
        }
    }
    void CreateBullet(Engine::Point pt) override {}
    void Hit(float damage) override;
};

class AkaArmy : public Army {
public:
    int flag = 0;
    ALLEGRO_TIMER* timer;
    AkaArmy(float x, float y);
    void Update(float deltaTime) override;
    void CreateBullet(Engine::Point pt) override {
            int dx = pt.x - Position.x;
            int dy = pt.y - Position.y;
            double len = sqrt(pow(dx, 2) + pow(dy, 2));
            Engine::Point diff = Engine::Point(dx / len, dy / len);
            Engine::Point rotateDiff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
            float rotation = atan2(rotateDiff.y, rotateDiff.x) + ALLEGRO_PI / 2;
            getPlayScene()->BulletGroup->AddNewObject(new ArcherBullet(Position, diff, rotation, this));
            AudioHelper::PlayAudio("missile.wav");
    }
    void Hit(float damage) override;
};


#endif /* BombArmy_hpp */
