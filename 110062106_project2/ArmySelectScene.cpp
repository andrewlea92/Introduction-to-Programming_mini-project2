#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>
#include <stdio.h>
#include "AudioHelper.hpp"
#include "GameEngine.hpp"
#include "ImageButton.hpp"
#include "Label.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Resources.hpp"
#include "Slider.hpp"
#include "ArmySelectScene.hpp"

void ArmySelectScene::Initialize() {
    // parameter initialization
    // TODO 2 (1/8): modify the totalArmy amount.
    totalArmy = 5;

    // Space status background
    AddNewObject(new Engine::Image("play/sand.png", 1250, 0, 336, 896));

    // TODO 1 (1/8): Initialize the usedSpace and totalSpace.
    usedSpace = 0;
    totalSpace = 5;

    // TODO 1 (2/8): Add the usedSpace and totalSpace to the label.
    char tmp_0[100];
    sprintf_s(tmp_0, "Space:%d/%d", usedSpace, totalSpace);
    AddNewObject(UISpaceUsage = new Engine::Label(tmp_0, "pirulen.ttf", 30, 1395, 150, 0, 0, 0, 255, 0.5, 0.5));

    // initialize armyAmount
    if (!fromSetting) {
        for (int i = 0; i < totalArmy; i++) {
            armyAmount[i] = 0;
        }
        armyAmount[3] = 2;
        armyAmount[4] = 1;
    }
    fromSetting = false;
    armyAmount[0] = 0;

    // set ArmyImage
    ArmyImage[0] = "play/sasha.png";
    // TODO 2 (2/8): Create the bomb image. You can find image in the play/ folder.
    ArmyImage[1] = "play/lyna2.png";
    ArmyImage[2] = "play/boy1.png";
    ArmyImage[3] = "play/ice2.png";
    ArmyImage[4] = "play/aka.png";
    // Add new enemy
    for (int i = 0; i < totalArmy; i++) {
        if (i != 3&&i!=4)
        {
            AddNewArmy(i, ArmyImage[i], 1);
        }
        
    }

    Engine::ImageButton* btn;

    // Setting button
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", 1300, 450, 190, 80);
    btn->SetOnClickCallback(std::bind(&ArmySelectScene::PlayOnClick, this, BUTTON_SETTING, -1, 0));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Setting", "pirulen.ttf", 30, 1395, 490, 0, 0, 0, 255, 0.5, 0.5));

    // TODO 1 (7/8): Create the reset button. You can imitate the enter button construction in the Initialize() function.
    // Suggestion of ImageButton's position setting: x(1300), y(600), w(190), h(80).
    // Suggestion of Label position settings: x(1395), y(640).
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", 1300, 600, 190, 80);
    btn->SetOnClickCallback(std::bind(&ArmySelectScene::PlayOnClick, this, BUTTON_RESET, -1, 0));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Reset", "pirulen.ttf", 30, 1395, 640, 0, 0, 0, 255, 0.5, 0.5));


    // Enter button
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", 1300, 750, 190, 80);
    btn->SetOnClickCallback(std::bind(&ArmySelectScene::PlayOnClick, this, BUTTON_ENTER, -1, 0));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Enter", "pirulen.ttf", 30, 1395, 790, 0, 0, 0, 255, 0.5, 0.5));

    // play bgm
    if (!mute)
        bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
    else
        bgmInstance = AudioHelper::PlaySample("select.ogg", true, 0.0);
}
void ArmySelectScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void ArmySelectScene::PlayOnClick(ButtonType type, int id, int spaceCost) {
    if (type == BUTTON_ENTER) {
        PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"));
        scene->SetTotalArmyAmount(totalArmy);
        for (int i = 0; i < totalArmy; i++) {
            scene->SetArmyAmount(i, armyAmount[i]);
            scene->ArmyImage[i] = ArmyImage[i];
        }

        Engine::GameEngine::GetInstance().ChangeScene("play");
        scene->mute = mute;
    }
    else if (type == BUTTON_RESET) {
        // TODO 1 (8/8): Reset the usedSpace and the amount of every army to 0.
        usedSpace = 0;
        for (int i = 0; i < totalArmy; i++)
        {
            armyAmount[i] = 0;
            if (i != 3)
            {
                UIArmyAmount[i]->Text = std::to_string(armyAmount[i]);
            }
        }
        char tmp_0[100];
        sprintf_s(tmp_0, "Space:%d/%d", usedSpace, totalSpace);
        UISpaceUsage->Text = tmp_0;
        armyAmount[3] = 2;
        armyAmount[4] = 1;
    }
    else if (type == BUTTON_ADD) {
        // TODO 1 (5/8): When the add(+) button is clicked, update the usedSpace and the armyAmount of that army. Make sure that the labels shown on the screen also update.
        // Notice that the button won't take effect when the usedSpace is equal to totalSpace.
        if (usedSpace < totalSpace) {
            {
                if (armyAmount[id] < totalSpace)
                {
                    usedSpace++;
                    armyAmount[id]++;
                    char tmp_0[100];
                    sprintf_s(tmp_0, "Space:%d/%d", usedSpace, totalSpace);
                    UISpaceUsage->Text = tmp_0;
                    UIArmyAmount[id]->Text = std::to_string(armyAmount[id]);
                }
            }
            
        }
    }
    else if (type == BUTTON_SUB) {
        // TODO 1 (6/8): When the sub(-) button is clicked, update the usedSpace and the armyAmount of that army. Make sure that the labels shown on the screen also update.
        // Notice that the button won't take effect when the armyAmount is equal to 0.
        if (usedSpace > 0) {
            if (armyAmount[id])
            {
                usedSpace--;
                armyAmount[id]--;
                char tmp_0[100];
                sprintf_s(tmp_0, "Space:%d/%d", usedSpace, totalSpace);
                UISpaceUsage->Text = tmp_0;
                UIArmyAmount[id]->Text = std::to_string(armyAmount[id]);
            }
            
        }
    }
    else if (type == BUTTON_SETTING) {
        Engine::GameEngine::GetInstance().ChangeScene("setting");
        fromSetting = true;
    }
}
void ArmySelectScene::AddNewArmy(int id, std::string imageName, int spaceCost) {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    int oneThirdW = w / 3;
    int oneThirdH = h / 3;
    int offsetW = (id % 3) * 380;
    int offsetH = id / 3 * 425;

    Engine::ImageButton* btn;

    // current amount
    AddNewObject(UIArmyAmount[id] = new Engine::Label(std::to_string(armyAmount[id]), "pirulen.ttf", 48, halfW / 4 + 80 + offsetW, oneThirdH - 225 + offsetH, 255, 255, 255, 255, 0.5, 0.5));

    // Army image
    AddNewObject(new Engine::Image(imageName, halfW / 4 + offsetW, oneThirdH - 175 + offsetH, 175, 175));
    // TODO 1 (3/8): Create the add(+) button. You can imitate the enter button construction in the Initialize() function.
    // Suggestion of ImageButton's position setting: x(halfW / 4 + offsetW), y(oneThirdH + 25 + offsetH), w(75), h(50).
    // Suggestion of Label position settings: x(halfW / 4 + 35 + offsetW), y(oneThirdH + 50 + offsetH).
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW / 4 + offsetW, oneThirdH + 25 + offsetH, 75, 50);
    btn->SetOnClickCallback(std::bind(&ArmySelectScene::PlayOnClick, this, BUTTON_ADD, id, 0));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("+", "pirulen.ttf", 30, halfW / 4 + 35 + offsetW, oneThirdH + 50 + offsetH, 0, 0, 0, 255, 0.5, 0.5));
    // TODO 1 (4/8): Create the sub(-) button. You can imitate the enter button construction in the Initialize() function.
    // Suggestion of ImageButton's position settings: x(halfW / 4 + offsetW + 100), y(oneThirdH + 25 + offsetH), w(75), h(50).
    // Suggestion of Label position settings: x(halfW / 4 + 135 + offsetW), y(oneThirdH + 50 + offsetH).
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW / 4 + offsetW + 100, oneThirdH + 25 + offsetH, 75, 50);
    btn->SetOnClickCallback(std::bind(&ArmySelectScene::PlayOnClick, this, BUTTON_SUB, id, 0));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("-", "pirulen.ttf", 30, halfW / 4 + 135 + offsetW, oneThirdH + 50 + offsetH, 0, 0, 0, 255, 0.5, 0.5));
}

void ArmySelectScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (keyCode == ALLEGRO_KEY_M) {
        mute = !mute;
        if (!mute)
            AudioHelper::ChangeSampleVolume(bgmInstance, AudioHelper::BGMVolume);
        else
            AudioHelper::ChangeSampleVolume(bgmInstance, 0.0);
    }
}
