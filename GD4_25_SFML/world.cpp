#include "world.hpp"
#include "sprite_node.hpp"
#include <iostream>
#include "state.hpp"
#include <SFML/System/Angle.hpp>
#include "Projectile.hpp"
#include "pickup.hpp"
#include "pointbox.hpp"
#include "pointbox_type.hpp"
#include "utility.hpp"
#include "sound_node.hpp"

World::World(sf::RenderWindow& window, SoundPlayer& sounds, FontHolder& font)
	: m_window(window)
	, m_camera(window.getDefaultView())
	, m_textures()
	, m_sounds(sounds)
	, m_fonts(font)
	, m_scene_graph(ReceiverCategories::kNone)
	, m_scene_layers()
	, m_world_bounds(sf::Vector2f(0.f, 0.f), sf::Vector2f(m_camera.getSize().x, 3000.f))
	, m_spawn_position(m_camera.getSize().x / 2.f, m_world_bounds.size.y - m_camera.getSize().y/2.f)
	//, m_scroll_speed(-100.f)
	, m_player_aircraft()
	, m_player_aircraft2()
	, m_pointbox_spawn_timer(sf::Time::Zero) //Timer
	, m_player_score(0) //Player Score Count
{
	LoadTextures();
	BuildScene();
	m_camera.setCenter(m_spawn_position);
}

void World::Update(sf::Time dt)
{
	//Scroll the world
	//m_camera.move(sf::Vector2f(0, m_scroll_speed * dt.asSeconds()));


	m_player_aircraft->SetVelocity(0.f, 0.f);
	m_player_aircraft2->SetVelocity(0.f, 0.f);

	DestroyEntitiesOutsideView();
	GuideMissiles();

	UpdateSounds();

	//Process commands from the scenegraph
	while (!m_command_queue.IsEmpty())
	{
		m_scene_graph.OnCommand(m_command_queue.Pop(), dt);
	}
	AdaptPlayerVelocity();

	HandleCollisions();
	m_scene_graph.RemoveWrecks();

	m_scene_graph.Update(dt, m_command_queue);
	AdaptPlayerPosition();

	UpdatePointBoxSpawning(dt);
}



void World::Draw()
{
	m_window.setView(m_camera);
	m_window.draw(m_scene_graph);
}



CommandQueue& World::GetCommandQueue()
{
	return m_command_queue;
}

bool World::HasAlivePlayer() const
{
	return !m_player_aircraft->IsMarkedForRemoval();
}

bool World::HasPlayerReachedEnd() const
{
	return !m_world_bounds.contains(m_player_aircraft->getPosition());
}

void World::LoadTextures()
{
	m_textures.Load(TextureID::kEagle, "Media/Textures/Eagle.png");
	m_textures.Load(TextureID::kEagle2, "Media/Textures/Eagle1.png");
	m_textures.Load(TextureID::kRaptor, "Media/Textures/Raptor.png");
	m_textures.Load(TextureID::kLandscape, "Media/Textures/Desert.png");
	m_textures.Load(TextureID::kBullet, "Media/Textures/Bullet.png");
	m_textures.Load(TextureID::kMissile, "Media/Textures/Missile.png");

	m_textures.Load(TextureID::kHealthRefill, "Media/Textures/HealthRefill.png");
	m_textures.Load(TextureID::kMissileRefill, "Media/Textures/MissileRefill.png");
	m_textures.Load(TextureID::kFireSpread, "Media/Textures/FireSpread.png");
	m_textures.Load(TextureID::kFireRate, "Media/Textures/FireRate.png");
	m_textures.Load(TextureID::kFinishLine, "Media/Textures/FinishLine.png");

	m_textures.Load(TextureID::kPointBoxPlusOne, "Media/Textures/box_plus_one.png");
	m_textures.Load(TextureID::kPointBoxPlusTwo, "Media/Textures/box_plus_two.png");
	m_textures.Load(TextureID::kPointBoxPlusThree, "Media/Textures/box_plus_three.png");

}

void World::BuildScene()
{
	//Initialise the different layers
	for (int i = 0; i < static_cast<int>(SceneLayers::kLayerCount); i++)
	{
		ReceiverCategories category = (i == static_cast<int>(SceneLayers::kAir)) ? ReceiverCategories::kScene : ReceiverCategories::kNone;
		SceneNode::Ptr layer(new SceneNode(category));
		m_scene_layers[i] = layer.get();
		m_scene_graph.AttachChild(std::move(layer));
	}

	//Prepare the background
	sf::Texture& texture = m_textures.Get(TextureID::kLandscape);
	sf::IntRect textureRect(m_world_bounds);
	texture.setRepeated(true);

	//Add the background sprite to the world
	std::unique_ptr<SpriteNode> background_sprite(new SpriteNode(texture, textureRect));
	background_sprite->setPosition(sf::Vector2f(0, 0));
	m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(background_sprite));

	//Add the finish line
	sf::Texture& finish_texture = m_textures.Get(TextureID::kFinishLine);
	std::unique_ptr<SpriteNode> finish_sprite(new SpriteNode(finish_texture));
	finish_sprite->setPosition(sf::Vector2f(0.f, -76.f));
	m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(finish_sprite));

	//Homework add the player's aircraft
	//Add two Raptor escort planes that are 50 units behind the plane and 80 units either side of the player's plane
	std::unique_ptr<Aircraft> leader(new Aircraft(AircraftType::kEagle, m_textures, m_fonts));
	m_player_aircraft = leader.get();
	m_player_aircraft->setPosition(m_spawn_position);
	m_player_aircraft->SetVelocity(40.f, m_scroll_speed);
	std::cout << "Player 1 spawn position: " << m_spawn_position.x << ", " << m_spawn_position.y << std::endl;

	m_scene_layers[static_cast<int>(SceneLayers::kAir)]->AttachChild(std::move(leader));


	
	std::unique_ptr<Aircraft> player2(new Aircraft(AircraftType::kEagle2, m_textures, m_fonts));
	m_player_aircraft2 = player2.get();
	m_player_aircraft2->setPosition(m_spawn_position2);
	m_player_aircraft2->SetVelocity(40.f, m_scroll_speed);

	std::cout << "Player 2 spawn position: " << m_spawn_position2.x << ", " << m_spawn_position2.y << std::endl;
	m_scene_layers[static_cast<int>(SceneLayers::kAir)]->AttachChild(std::move(player2));

	//left_escort->setPosition(sf::Vector2f(- 80.f, 50.f));
	//m_player_aircraft2 = left_escort.get();
	////m_player_aircraft->AttachChild(std::move(left_escort));
	//m_player_aircraft2->SetVelocity(40.f, m_scroll_speed);
	

	/*std::unique_ptr<Aircraft> right_escort(new Aircraft(AircraftType::kRaptor, m_textures, m_fonts));
	right_escort->setPosition(sf::Vector2f(80.f, 50.f));
	m_player_aircraft->AttachChild(std::move(right_escort));*/

}

void World::AdaptPlayerVelocity()
{
	sf::Vector2f velocity = m_player_aircraft->GetVelocity();
	sf::Vector2f velocity2 = m_player_aircraft2->GetVelocity();


	//If they are moving diagonally divide by sqrt 2
	if (velocity.x != 0.f && velocity.y != 0.f)
	{
		m_player_aircraft->SetVelocity(velocity / std::sqrt(2.f));
		
	}
	if (velocity2.x != 0.f && velocity2.y != 0.f)
	{
		m_player_aircraft2->SetVelocity(velocity2 / std::sqrt(2.f));

	}
	//Add scrolling velocity
}

void World::AdaptPlayerPosition()
{
	//keep player on the screen
	sf::FloatRect view_bounds(m_camera.getCenter() - m_camera.getSize() / 2.f, m_camera.getSize());
	const float border_distance = 40.f;

	sf::Vector2f position = m_player_aircraft->getPosition();
	position.x = std::min(position.x, view_bounds.size.x - border_distance);
	position.x = std::max(position.x, border_distance);
	position.y = std::min(position.y, view_bounds.position.y + view_bounds.size.y - border_distance);
	position.y = std::max(position.y, view_bounds.position.y + border_distance);
	m_player_aircraft->setPosition(position);

	sf::Vector2f position2 = m_player_aircraft2->getPosition();
	position2.x = std::min(position2.x, view_bounds.size.x - border_distance);
	position2.x = std::max(position2.x, border_distance);
	position2.y = std::min(position2.y, view_bounds.position.y + view_bounds.size.y - border_distance);
	position2.y = std::max(position2.y, view_bounds.position.y + border_distance);
	m_player_aircraft2->setPosition(position2);

}

/*void World::SpawnEnemies()
{
	//Spawn an enemy when it is relevent i.e in BattlefieldBounds
	while (!m_enemy_spawn_points.empty() && m_enemy_spawn_points.back().m_y > GetBattleFieldBounds().position.y)
	{
		SpawnPoint spawn = m_enemy_spawn_points.back();
		std::unique_ptr<Aircraft> enemy(new Aircraft(spawn.m_type, m_textures, m_fonts));
		enemy->setPosition(sf::Vector2f(spawn.m_x, spawn.m_y));
		enemy->setRotation(sf::degrees(180.f));
		m_scene_layers[static_cast<int>(SceneLayers::kAir)]->AttachChild(std::move(enemy));
		m_enemy_spawn_points.pop_back();
	}
}

void World::AddEnemies()
{
	AddEnemy(AircraftType::kRaptor, 0.f, 500.f);
	AddEnemy(AircraftType::kRaptor, 0.f, 1000.f);
	AddEnemy(AircraftType::kRaptor, 100.f, 1100.f);
	AddEnemy(AircraftType::kRaptor, -100.f, 1100.f);
	AddEnemy(AircraftType::kRaptor, -70.f, 1400.f);
	AddEnemy(AircraftType::kRaptor, 70.f, 1400.f);
	AddEnemy(AircraftType::kRaptor, 70.f, 1600.f);

	//Sort the enemies according to y-value
	std::sort(m_enemy_spawn_points.begin(), m_enemy_spawn_points.end(), [](SpawnPoint lhs, SpawnPoint rhs)
		{
			return lhs.m_y < rhs.m_y;
		});
}

void World::AddEnemy(AircraftType type, float relx, float rely)
{
	SpawnPoint spawn(type, m_spawn_position.x + relx, m_spawn_position.y - rely);
	m_enemy_spawn_points.emplace_back(spawn);
}
*/
sf::FloatRect World::GetViewBounds() const
{
	return sf::FloatRect(m_camera.getCenter() - m_camera.getSize() / 2.f, m_camera.getSize());;
}

sf::FloatRect World::GetBattleFieldBounds() const
{
	//Return camera bounds + a small area off screen where the enemies spawn
	sf::FloatRect bounds = GetViewBounds();
	bounds.position.y -= 100.f;
	bounds.size.y += 100.f;
	return bounds;
}

void World::GuideMissiles()
{
	//Target the closest enemy in the world
	Command enemyCollector;
	enemyCollector.category = static_cast<int>(ReceiverCategories::kEnemyAircraft);
	enemyCollector.action = DerivedAction<Aircraft>([this](Aircraft& enemy, sf::Time)
		{
			if (!enemy.IsDestroyed())
			{
				m_active_enemies.emplace_back(&enemy);
			}
		});

	Command missileGuider;
	missileGuider.category = static_cast<int>(ReceiverCategories::kAlliedProjectile);
	missileGuider.action = DerivedAction<Projectile>([this](Projectile& missile, sf::Time)
		{
			if (!missile.IsGuided())
			{
				return;
			}

			float min_distance = std::numeric_limits<float>::max();
			Aircraft* closest_enemy = nullptr;

			for (Aircraft* enemy : m_active_enemies)
			{
				float enemy_distance = Distance(missile, *enemy);
				if (enemy_distance < min_distance)
				{
					closest_enemy = enemy;
					min_distance = enemy_distance;
				}
			}
			if (closest_enemy)
			{
				missile.GuideTowards(closest_enemy->GetWorldPosition());
			}
		});
	m_command_queue.Push(enemyCollector);
	m_command_queue.Push(missileGuider);
	m_active_enemies.clear();
}

bool MatchesCategories(SceneNode::Pair& colliders, ReceiverCategories type1, ReceiverCategories type2)
{
	unsigned int category1 = colliders.first->GetCategory();
	unsigned int category2 = colliders.second->GetCategory();

	if ((static_cast<int>(type1) & category1) && (static_cast<int>(type2) & category2))
	{
		return true;
	}
	else if ((static_cast<int>(type1) & category2) && (static_cast<int>(type2) & category1))
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}

}

void World::HandleCollisions()
{
	//std::cout << "Player 2 Active: " << !m_player_aircraft2->IsMarkedForRemoval() << std::endl;
	std::set<SceneNode::Pair> collision_pairs;
	m_scene_graph.CheckSceneCollision(m_scene_graph, collision_pairs);


	for (SceneNode::Pair pair : collision_pairs)
	{
		if (MatchesCategories(pair, ReceiverCategories::kPlayerAircraft, ReceiverCategories::kEnemyAircraft))
		{
			auto& player = static_cast<Aircraft&>(*pair.first);
			auto& enemy = static_cast<Aircraft&>(*pair.second);
			//Collision response
			std::cout << "Collision: Player vs Enemy" << std::endl;
			player.Damage(enemy.GetHitPoints());
			enemy.Destroy();
		}
		else if (MatchesCategories(pair, ReceiverCategories::kPlayerAircraft, ReceiverCategories::kPickup))
		{
			auto& player = static_cast<Aircraft&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);
			std::cout << "Collision: Player vs Pickup" << std::endl;

			//Collision response
			pickup.Apply(player);
			pickup.Destroy();
		}
		else if (MatchesCategories(pair, ReceiverCategories::kPlayerAircraft, ReceiverCategories::kEnemyProjectile) || MatchesCategories(pair,ReceiverCategories::kEnemyAircraft, ReceiverCategories::kAlliedProjectile))
		{
			auto& aircraft = static_cast<Aircraft&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);
			//Collision response
			std::cout << "Collision: Aircraft vs Projectile" << std::endl;

			aircraft.Damage(projectile.GetDamage());
			projectile.Destroy();
		}
		else if (MatchesCategories(pair, ReceiverCategories::kPlayerAircraft, ReceiverCategories::kPointBox)) {
			auto& player = static_cast<Aircraft&>(*pair.first);
			auto& pointbox = static_cast<PointBox&>(*pair.second);

			int points = pointbox.GetPointValue();
			m_player_score += points;
			player.AddScore(points);

			std::cout << "current player score: " << m_player_score << std::endl;

			pointbox.Destroy();
		}
	}
}

void World::DestroyEntitiesOutsideView()
{
	Command command;
	command.category = static_cast<int>(ReceiverCategories::kEnemyAircraft) | static_cast<int>(ReceiverCategories::kProjectile)| static_cast<int>(ReceiverCategories::kPointBox);
	command.action = DerivedAction<Entity>([this](Entity& e, sf::Time dt)
	{
		//Does the object intersect with the battlefield
		if (GetBattleFieldBounds().findIntersection(e.GetBoundingRect()) == std::nullopt)
		{
			e.Destroy();
		}
	});
	m_command_queue.Push(command);

}

void World::UpdatePointBoxSpawning(sf::Time dt) { //Timer for boxes spawning
	const sf::Time kSpawenInterval = sf::seconds(1.0f); //Spawn every X Seconds (3 ATM)
	m_pointbox_spawn_timer += dt;

	if (m_pointbox_spawn_timer >= kSpawenInterval)
	{
		SpawnPointBoxes();
		m_pointbox_spawn_timer = sf::Time::Zero; //Timer reset after summon
	}
}

void World::SpawnPointBoxes() {
	int random_type = Utility::RandomInt(static_cast<int>(PointBoxType::kPointBoxCount));
	PointBoxType type = static_cast<PointBoxType>(random_type);

	std::unique_ptr<PointBox> box(new PointBox(type, m_textures, m_fonts));

	sf::FloatRect view_bounds = GetViewBounds();

	float min_x = view_bounds.position.x + 50.f;
	float max_x = view_bounds.position.x + view_bounds.size.x - 50.f;
	int range = static_cast<int>(max_x - min_x);

	float spawn_x = min_x + static_cast<float>(Utility::RandomInt(range + 1));

	float spawn_y = view_bounds.position.y - 50.f;

	box->setPosition(sf::Vector2f(spawn_x, spawn_y));

	m_scene_layers[static_cast<int>(SceneLayers::kAir)]->AttachChild(std::move(box));

	std::cout << "X Spawn: " << spawn_x << " Y Spawn: " << spawn_y << std::endl;
}

bool World::HasPlayerReachedPoints() const{
	return m_player_aircraft->GetScore() >= 30; 
}


void World::UpdateSounds()
{
	sf::Vector2f listener_position;

	listener_position = m_camera.getCenter();



	m_sounds.SetListenerPosition(listener_position);

	m_sounds.RemoveStoppedSounds();
}


