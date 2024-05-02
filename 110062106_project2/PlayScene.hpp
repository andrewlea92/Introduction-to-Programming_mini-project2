#ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include <tuple>

#include "IScene.hpp"
#include "Point.hpp"

#define MAX_ARMY_AMOUNT 6
#define WALL_SIZE 4

class Defense;
class Army;
namespace Engine {
	class Group;
	class Image;
	class Label;
	class Sprite;
}  // namespace Engine

class PlayScene final : public Engine::IScene {
private:
	enum TileType {
		TILE_DIRT,
		TILE_FLOOR,
        TILE_WALL,
        TILE_CANNON,
		TILE_ENEMY2,
		TILE_TRAP,
		TILE_OCCUPIED,
	};
	ALLEGRO_SAMPLE_ID bgmId;
	std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
protected:
	int SpeedMult;
    // Enemy Amounts
    int armyAmount[MAX_ARMY_AMOUNT];
    // Total enemies;
    int totalArmy;
public:
    bool mute;
	static bool DebugMode;
	static const std::vector<Engine::Point> directions;
	static const int MapWidth, MapHeight;
	static const int BlockSize;
	static const float DangerTime;
	static const std::vector<int> code;
	int MapId;
    Engine::Point corners[WALL_SIZE];
    std::vector<Engine::Point> brokenWall[WALL_SIZE];
	float ticks;
	float deathCountDown;
	// Map tiles.
	Group* TileMapGroup;
	Group* GroundEffectGroup;
	Group* DebugIndicatorGroup;
	Group* BulletGroup;
    Group* DefenseGroup;
	Group* TrapGroup;
    Group* WallGroup;
    Group* ArmyGroup;
	Group* IceGroup;
	Group* EffectGroup;
	Group* UIGroup;
    std::string ArmyImage[MAX_ARMY_AMOUNT];
    Engine::Label *UIArmyAmount[MAX_ARMY_AMOUNT];
	Engine::Image* imgTarget;
    Army* preview;
	std::vector<std::vector<TileType>> mapState;
    std::list<int> keyStrokes;
    static Engine::Point GetClientSize();
	explicit PlayScene() = default;
	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Draw() const override;
	void OnMouseDown(int button, int mx, int my) override;
	void OnMouseMove(int mx, int my) override;
	void OnMouseUp(int button, int mx, int my) override;
	void OnKeyDown(int keyCode) override;
    // Reduce the enemy amounts when release
    void ReduceAmount(int id);
    // get & set enemy amount
    int GetArmyAmount(int id);
    void SetArmyAmount(int id, int amount);
    // get & set totalEnemy amount
    int GetTotalArmyAmount();
    void SetTotalArmyAmount(int total);
	void ReadMap();
	void ConstructUI();
    void ConstructButton(int id, std::string imageName);
	void UIBtnClicked(int id);
    bool CheckOccupied(int x, int y);
    void ClearMapState(int x, int y);
};
#endif // PLAYSCENE_HPP
