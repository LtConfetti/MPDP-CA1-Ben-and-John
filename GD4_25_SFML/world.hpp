#pragma once
#include <SFML/Graphics.hpp>
#include "resource_identifiers.hpp"
#include "scene_node.hpp"
#include "scene_layers.hpp"
#include "aircraft.hpp"
#include "sound_player.hpp"
#include "command_queue.hpp"
#include "pointbox.hpp"
#include "pointbox_type.hpp"

class World
{
public:
	explicit World(sf::RenderWindow& window, SoundPlayer& sound, FontHolder& font);
	void Update(sf::Time dt);
	void Draw();

	CommandQueue& GetCommandQueue();

	bool HasAlivePlayer() const;
	bool HasPlayerReachedEnd() const;
	bool HasPlayerReachedPoints() const; //New Win Condition for Points

	int GetPlayer1Score() const; //Player1 Get Points
	int GetPlayer2Score() const; //Player 2 Get Points
	int GetWinningPlayer() const; //Winner for GameState

private:
	void LoadTextures();
	void BuildScene();
	void AdaptPlayerVelocity();
	void AdaptPlayerPosition();

	void SpawnEnemies();
	void AddEnemies();
	void AddEnemy(AircraftType type, float relx, float rely);

	sf::FloatRect GetViewBounds() const;
	sf::FloatRect GetBattleFieldBounds() const;

	void GuideMissiles();

	void HandleCollisions();

	void DestroyEntitiesOutsideView();

	void SpawnPointBoxes();
	void UpdatePointBoxSpawning(sf::Time dt);

	void UpdateSounds();

private:
	struct SpawnPoint
	{
		SpawnPoint(AircraftType type, float x, float y) :m_type(type), m_x(x), m_y(y)
		{

		}
		AircraftType m_type;
		float m_x;
		float m_y;
	};

private:
	sf::RenderWindow& m_window;
	sf::View m_camera;
	TextureHolder m_textures;
	FontHolder& m_fonts;
	SoundPlayer& m_sounds;
	SceneNode m_scene_graph;
	std::array<SceneNode*, static_cast<int>(SceneLayers::kLayerCount)> m_scene_layers;
	sf::FloatRect m_world_bounds;
	sf::Vector2f m_spawn_position;
	sf::Vector2f m_spawn_position2;
	float m_scroll_speed;
	Aircraft* m_player_aircraft;
	Aircraft* m_player_aircraft2;

	sf::Time m_pointbox_spawn_timer; //Timer to track when to spawn the boxes
	int m_player1_score; //Score for player 1
	int m_player2_score; //Score for player 2

	CommandQueue m_command_queue;

	std::vector<SpawnPoint> m_enemy_spawn_points;
	std::vector<Aircraft*> m_active_enemies;
};

