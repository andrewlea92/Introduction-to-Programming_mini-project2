#ifndef ArmyButton_hpp
#define ArmyButton_hpp

#include <string>

#include "ImageButton.hpp"
#include "Sprite.hpp"

class PlayScene;

class ArmyButton : public Engine::ImageButton {
protected:
    PlayScene* getPlayScene();
public:
    int money;
    int id;
    Engine::Sprite Army;
    ArmyButton(std::string img, std::string imgIn, Engine::Sprite Army, float x, float y, int money, int id);
    void Update(float deltaTime) override;
    void Draw() const override;
};

#endif /* ArmyButton_hpp */
