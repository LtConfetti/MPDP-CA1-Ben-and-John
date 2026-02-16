#pragma once
#include <SFML/Graphics.hpp>
#include "resource_identifiers.hpp"
#include "scene_node.hpp"
#include "scene_layers.hpp"
#include "aircraft.hpp"
#include "command_queue.hpp"
#include "pointbox.hpp"
#include "pointbox_type.hpp"

class World
{
public:
	explicit World(sf::RenderWindow& window, FontHolder& font);
	void Update(sf::Time dt);
	void Draw();

	CommandQueue& GetCommandQueue();

	bool HasAlivePlayer() const;
	bool HasPlayerReachedEnd() const;

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
	SceneNode m_scene_graph;
	std::array<SceneNode*, static_cast<int>(SceneLayers::kLayerCount)> m_scene_layers;
	sf::FloatRect m_world_bounds;
	sf::Vector2f m_spawn_position;
	float m_scroll_speed;
	Aircraft* m_player_aircraft;
	Aircraft* m_player_aircraft2;

	sf::Time m_pointbox_spawn_timer; //Timer to track when to spawn the boxes
	int m_player_score; //Score for players

	CommandQueue m_command_queue;

	std::vector<SpawnPoint> m_enemy_spawn_points;
	std::vector<Aircraft*> m_active_enemies;
};

