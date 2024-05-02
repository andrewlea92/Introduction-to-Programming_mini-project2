#include <allegro5/allegro.h>
#include <algorithm>
#include <random>
#include <cmath>
#include <fstream>
#include <functional>
#include <vector>
#include <queue>
#include <string>
#include <memory>
#include <numeric> 

// Fundamental
#include "AudioHelper.hpp"
#include "DirtyEffect.hpp"
#include "GameEngine.hpp"
#include "Group.hpp"
#include "IObject.hpp"
#include "Image.hpp"
#include "Label.hpp"
#include "Plane.hpp"
#include "PlayScene.hpp"
#include "Resources.hpp"
#include "Sprite.hpp"
#include "LOG.hpp"

// Button
#include "ArmyButton.hpp"

// Army
#include "BombArmy.hpp"
#include "ArcherArmy.hpp"

// Defense
#include "CannonDefense.hpp"
#include "WallDefense.hpp"

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define WALL_SIZE 4
#define MAX_ARMY_AMOUNT 6

// TODO 2 (4/8) : Add the BombArmy when click the imageButton and place it. You can search for the ArcherArmy to know where to add.

bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
const int PlayScene::MapWidth = 24, PlayScene::MapHeight = 12;//50;//13;
const int PlayScene::BlockSize = 64;
const float PlayScene::DangerTime = 7.61;
// TODO 4 (2/3): Set the code sequence correctly.
const std::vector<int> PlayScene::code = { ALLEGRO_KEY_UP,ALLEGRO_KEY_UP,ALLEGRO_KEY_DOWN,ALLEGRO_KEY_DOWN,ALLEGRO_KEY_LEFT,ALLEGRO_KEY_RIGHT,ALLEGRO_KEY_ENTER };
Engine::Point PlayScene::GetClientSize() {
	return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void PlayScene::Initialize() {
	mapState.clear();
	keyStrokes.clear();
	ticks = 0;
	deathCountDown = -1;
	SpeedMult = 1;
    MapId = 1;

	// Add groups from bottom to top.
	AddNewObject(TileMapGroup = new Group());
    AddNewObject(ArmyGroup = new Group());
    AddNewObject(IceGroup = new Group());
    AddNewObject(WallGroup = new Group());
    AddNewObject(DefenseGroup = new Group());
    AddNewObject(TrapGroup = new Group());
	AddNewObject(DebugIndicatorGroup = new Group());
	AddNewObject(BulletGroup = new Group());
    AddNewObject(GroundEffectGroup = new Group());
	AddNewObject(EffectGroup = new Group());
    
	// Should support buttons.
	AddNewControlObject(UIGroup = new Group());
	ReadMap();
	ConstructUI();
	imgTarget = new Engine::Image("play/target.png", 0, 0, BlockSize, BlockSize);
	imgTarget->Visible = false;
	preview = nullptr;
	UIGroup->AddNewObject(imgTarget);
	// Preload Lose Scene
	deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
	Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
	// Start BGM.
    if (!mute)
        bgmInstance = AudioHelper::PlaySample("play.ogg", true, AudioHelper::BGMVolume);
    else
        bgmInstance = AudioHelper::PlaySample("play.ogg", true, 0.0);
}
void PlayScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	AudioHelper::StopSample(deathBGMInstance);
	deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
	if (SpeedMult == 0)
		deathCountDown = -1;
	else if (deathCountDown != -1)
		SpeedMult = 1;

	for (int i = 0; i < SpeedMult; i++) {
	    IScene::Update(deltaTime);
		ticks += deltaTime;
	}
    
    // Win
    if (DefenseGroup->GetObjects().empty()) {
        Engine::GameEngine::GetInstance().ChangeScene("win");
    }
    
    // Lose
    bool armyEmpty = true;
    for (int i=0; i<totalArmy; i++) {
        if (armyAmount[i] > 0&& i!=3) {
            armyEmpty = false;
            break;
        }
    }
    //Engine::GameEngine::GetInstance().ChangeScene("lose");
    if (!ArmyGroup->GetObjects().empty()) armyEmpty = false;
    if (armyEmpty) {
        // Release the resources
        //Engine::GameEngine::GetInstance().ChangeScene("win");
        //delete TileMapGroup;
        //delete GroundEffectGroup;
        //delete DebugIndicatorGroup;
        //delete BulletGroup;
        //delete DefenseGroup;
        //delete WallGroup;
        //delete ArmyGroup;
        //delete EffectGroup;
        //delete UIGroup;
        Engine::GameEngine::GetInstance().ChangeScene("lose");
    }
    
	if (preview) {
		preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
		// To keep responding when paused.
		preview->Update(deltaTime);
	}
}
void PlayScene::Draw() const {
	IScene::Draw();
}
void PlayScene::OnMouseDown(int button, int mx, int my) {
	if ((button & 1) && !imgTarget->Visible && preview) {
		// Cancel army construct.
		UIGroup->RemoveObject(preview->GetObjectIterator());
		preview = nullptr;
	}
	IScene::OnMouseDown(button, mx, my);
}
void PlayScene::OnMouseMove(int mx, int my) {
	IScene::OnMouseMove(mx, my);
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
    
	if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) {
		imgTarget->Visible = false;
		return;
	}
    
    imgTarget->Visible = true;
    imgTarget->Position.x = x * BlockSize;
    imgTarget->Position.y = y * BlockSize;
}
void PlayScene::OnMouseUp(int button, int mx, int my) {
	IScene::OnMouseUp(button, mx, my);

    if (!imgTarget->Visible)
        return;
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	if(x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
	    return;
	if (button & 1) {
		if (!CheckOccupied(x, y)||preview->id==3) {
			if (!preview)
				return;

            ReduceAmount(preview->id);
            int remainId = armyAmount[preview->id]> 0 ? preview->id : -1;
			// Remove Preview.
			preview->GetObjectIterator()->first = false;
            UIGroup->RemoveObject(preview->GetObjectIterator());
            
			// Construct real turret.
			preview->Position.x = x * BlockSize + BlockSize / 2;
			preview->Position.y = y * BlockSize + BlockSize / 2;
			preview->Enabled = true;
            preview->isPreview = false;
			preview->Tint = al_map_rgba(255, 255, 255, 255);
            preview->CalcRegion(x, y);
            if (preview->id == 3)
            {
                IceGroup->AddNewObject(preview);
            }
            else
            {
                ArmyGroup->AddNewObject(preview);
            }
			
			// To keep responding when paused.
			preview->Update(0);
			// Remove Preview.
			preview = nullptr;
            
            // if amount > 0, keep the preview exists
            if (remainId != -1) {
                if (remainId == 0)
                    preview = new ArcherArmy(0, 0);
                else if (remainId == 1)
                {
                    preview= new BombArmy(0, 0);
                }
                else if (remainId == 2)
                {
                    preview = new TankArmy(0, 0);
                }
                else if (remainId == 3)
                {
                    preview = new IceArmy(0, 0);
                }
                else if (remainId == 4)
                {
                    preview = new AkaArmy(0, 0);
                }

                preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
                preview->Tint = al_map_rgba(255, 255, 255, 200);
                preview->Enabled = false;
                preview->isPreview = true;
                UIGroup->AddNewObject(preview);
            }

			OnMouseMove(mx, my);
		}
		else{
            Engine::Sprite* sprite;
            GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
            sprite->Rotation = 0;
			OnMouseMove(mx, my);
		}
	}
}
void PlayScene::OnKeyDown(int keyCode) {
	IScene::OnKeyDown(keyCode);
	if (keyCode == ALLEGRO_KEY_TAB) {
        // TODO 4 (1/3): Set Tab as a code to active / de-active the debug mode.
        if (DebugMode)
        {
            DebugMode = false;
        }
        else
        {
            DebugMode = true;
        }
        
	}
	else {
        
		keyStrokes.push_back(keyCode);
		if (keyStrokes.size() > code.size())
			keyStrokes.pop_front();
        //if(keyStrokes[0]==84|| keyStrokes[1] == 84|| keyStrokes[2] == 85 || keyStrokes[3] == 85 || keyStrokes[4] == 82 || keyStrokes[5] == 82 || keyStrokes[6] == 83 || keyStrokes[7] == 83 || keyStrokes[8] == 67)
        //keyStrokes.push_back(1);
        //if(keyStrokes==code)
        if (keyStrokes.size() == code.size()) {
            int i = 0;
            for (int i = 0; i < 7; i++) {
                if (keyStrokes.front() == code[i]) {
                    keyStrokes.pop_front();
                    if (i == 6) EffectGroup->AddNewObject(new Plane());
                }
                else {
                    break;
                }
            }
        }
        // TODO 4 (3/3): Check whether the input sequence corresponds to the code.
        // Active a plane : EffectGroup->AddNewObject(new Plane());
        
	}
	if (keyCode == ALLEGRO_KEY_Q) {
		// Hotkey for ArcherArmy.
		UIBtnClicked(0);
	}
	else if (keyCode == ALLEGRO_KEY_W) {
		// Hotkey for BombArmy.
		UIBtnClicked(1);
	}
	else if (keyCode == ALLEGRO_KEY_E) {
		// Hotkey for ...
		UIBtnClicked(2);
	}
	else if (keyCode == ALLEGRO_KEY_R) {
		// Hotkey for ...
		UIBtnClicked(3);
	}
    else if (keyCode == ALLEGRO_KEY_M) {
        // mute
        mute = !mute;
        if (!mute)
            AudioHelper::ChangeSampleVolume(bgmInstance, AudioHelper::BGMVolume);
        else
            AudioHelper::ChangeSampleVolume(bgmInstance, 0.0);
    }
	else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
		// Hotkey for Speed up.
		SpeedMult = keyCode - ALLEGRO_KEY_0;
	}
}

void PlayScene::ReduceAmount(int id) {
    armyAmount[id]--;
    UIArmyAmount[id]->Text = "x" + std::to_string(armyAmount[id]);
}
void PlayScene::ReadMap() {
	std::string filename = std::string("resources/map") + std::to_string(MapId) + ".txt";
	// Read map file.
	char c;
	std::vector<TileType> mapData;
	std::ifstream fin(filename);
	while (fin >> c) {
        // 0: floor
        // 1: wall
        // 2: canon
		switch (c) {
		case '0': mapData.push_back(TILE_FLOOR); break;
		case '1': mapData.push_back(TILE_WALL); break;
        case '2': mapData.push_back(TILE_CANNON); break;
        case '3': mapData.push_back(TILE_ENEMY2); break;
        case '4': mapData.push_back(TILE_TRAP); break;
		case '\n':
		case '\r':
			if (static_cast<int>(mapData.size()) / MapWidth != 0)
				throw std::ios_base::failure("Map data is corrupted.");
			break;
		default: throw std::ios_base::failure("Map data is corrupted.");
		}
	}
	fin.close();
	// Validate map data.
	if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
		throw std::ios_base::failure("Map data is corrupted.");
	// Store map in 2d array.
	mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
    int cornerIdx = 0;
	for (int i = 0; i < MapHeight; i++) {
		for (int j = 0; j < MapWidth; j++) {
			const TileType md = mapData[i * MapWidth + j];
            TileMapGroup->AddNewObject(new Engine::Image("play/deep-pink.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            switch (md) {
                case TILE_WALL:
                    WallGroup->AddNewObject(new WallDefense(j * BlockSize + BlockSize / 2, i * BlockSize + BlockSize / 2));
                    if (j >= 2) {
                        if (mapState[i][j-2] == TILE_FLOOR && mapState[i][j-1] == TILE_WALL) {
                            corners[cornerIdx++] = Engine::Point(j-1, i);
                        }
                    }
                    break;
                case TILE_CANNON:
                    DefenseGroup->AddNewObject(new CannonDefense(j * BlockSize + BlockSize / 2, i * BlockSize + BlockSize / 2));
                    break;
                case TILE_ENEMY2:
                    DefenseGroup->AddNewObject(new CannonDefense2(j * BlockSize + BlockSize / 2, i * BlockSize + BlockSize / 2));
                    break;
                case TILE_TRAP:
                    TrapGroup->AddNewObject(new Trap(j * BlockSize + BlockSize / 2, i * BlockSize + BlockSize / 2));
                    break;
                case TILE_FLOOR:
                    if (j <= MapWidth-2 && j >= 2) {
                        if (mapState[i][j-2] == TILE_WALL && mapState[i][j-1] == TILE_WALL) {
                            corners[cornerIdx++] = Engine::Point(j-1, i);
                        }
                    }
                    break;
                default:
                    throw std::ios_base::failure("Map data is corrupted.");
            }
            mapState[i][j] = md;
		}
	}
    
    // There must be four corners in the map
    if (cornerIdx != 4)
        throw std::ios_base::failure("Corner size is wrong.");
}

void PlayScene::ConstructUI() {
	// Background
	UIGroup->AddNewObject(new Engine::Image("play/sand.png", 0, 64*MapHeight, 1536, 128));

    // TODO 2 (3/8) : Construct the select button for bomb army.
    ConstructButton(0, ArmyImage[0]);
    ConstructButton(1, ArmyImage[1]);
    ConstructButton(2, ArmyImage[2]);
    ConstructButton(3, ArmyImage[3]);
    ConstructButton(4, ArmyImage[4]);

}
void PlayScene::ConstructButton(int id, std::string imageName) {
    ArmyButton* btn;
    // Button
    btn = new ArmyButton("play/floor.png", "play/dirt.png",
        Engine::Sprite(imageName, 175 + 120 * id, BlockSize * MapHeight + 10, 80, 80, 0, 0)
        , 170 + 120 * id,  BlockSize * MapHeight, 0, id);
    // Reference: Class Member Function Pointer and std::bind.
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, id));
    UIGroup->AddNewControlObject(btn);
    // Button Label
    AddNewObject(UIArmyAmount[id] = new Engine::Label("x" + std::to_string(armyAmount[id]), "pirulen.ttf", 20.5, 230 + 120 * id, BlockSize * MapHeight + 110, 0, 0, 0, 255, 0.5, 0.5));
}

void PlayScene::UIBtnClicked(int id) {
    // might have chosen an army already, hence we need to delete first
	if (preview) {
		UIGroup->RemoveObject(preview->GetObjectIterator());
        preview = nullptr;
    }
    
	if (id == 0)
        preview = new ArcherArmy(0, 0);
    else if (id == 1)
    {
        preview = new BombArmy(0, 0);
    }
    else if (id == 2)
    {
        preview = new TankArmy(0, 0);
    }
    else if (id == 3)
    {
        preview = new IceArmy(0, 0);
    }
    else if (id == 4)
    {
        preview = new AkaArmy(0, 0);
    }

	if (!preview)
		return;
    
    // set preview profile
	preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
	preview->Tint = al_map_rgba(255, 255, 255, 200);
	preview->Enabled = false;
    preview->isPreview = true;
	UIGroup->AddNewObject(preview);
	OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
}


bool PlayScene::CheckOccupied(int x, int y) {
    if (x < 0 || x >= MapWidth  || y < 0 || y >= MapHeight)
        return true;
    
    TileType tt = mapState[y][x];
    if (tt == TILE_WALL || tt == TILE_CANNON || tt==TILE_ENEMY2||tt==TILE_TRAP) return true;
    
    if (x >= corners[0].x && x <= corners[1].x
        && y >= corners[0].y && y <= corners[2].y
        ) return true;
    
    return false;
}

int PlayScene::GetArmyAmount(int id) {
    return armyAmount[id];
}

void PlayScene::SetArmyAmount(int id, int amount) {
    armyAmount[id] = amount;
}

int PlayScene::GetTotalArmyAmount() {
    return totalArmy;
}

void PlayScene::SetTotalArmyAmount(int total) {
    totalArmy = total;
}

void PlayScene::ClearMapState(int x, int y) {
    mapState[y][x] = TILE_FLOOR;
}
