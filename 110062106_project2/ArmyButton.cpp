#include <allegro5/color.h>

#include "GameEngine.hpp"
#include "IScene.hpp"
#include "PlayScene.hpp"
#include "ArmyButton.hpp"
PlayScene* ArmyButton::getPlayScene() {
    return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
ArmyButton::ArmyButton(std::string img, std::string imgIn, Engine::Sprite Army, float x, float y, int money, int id) :
    ImageButton(img, imgIn, x, y, 90, 100), money(money), Army(Army), id(id) {
}
void ArmyButton::Update(float deltaTime) {
    ImageButton::Update(deltaTime);
    if (getPlayScene()->GetArmyAmount(id) > 0) {
        Enabled = true;
        Army.Tint = al_map_rgba(255, 255, 255, 255);
    }
    else {
        Enabled = false;
        Army.Tint = al_map_rgba(0, 0, 0, 160);
    }
}
void ArmyButton::Draw() const {
    ImageButton::Draw();
    Army.Draw();
}
