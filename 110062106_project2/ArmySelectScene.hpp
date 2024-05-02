#ifndef ArmySelectScene_hpp
#define ArmySelectScene_hpp

#include <allegro5/allegro_audio.h>
#include <memory>
#include "IScene.hpp"

class ArmySelectScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    enum ButtonType {
        BUTTON_SETTING,
        BUTTON_ADD,
        BUTTON_SUB,
        BUTTON_RESET,
        BUTTON_ENTER,
    };
public:
    explicit ArmySelectScene() = default;
    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(ButtonType type, int id, int spaceCost);
    void AddNewArmy(int id, std::string imageName, int spaceCost);
    void OnKeyDown(int keyCode) override;
    
    std::string ArmyImage[MAX_ARMY_AMOUNT];
    Engine::Label *UIArmyAmount[6];
    Engine::Label *UISpaceUsage;
    int armyAmount[6];
    int usedSpace, totalSpace;
    int totalArmy;
    bool mute = false;
    bool fromSetting = false;
};

#endif /* ArmySelectScene_hpp */
