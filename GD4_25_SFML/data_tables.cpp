#include "data_tables.hpp"
#include "aircraft_type.hpp"
#include "projectile_type.hpp"
#include "pickup_type.hpp"
#include "aircraft.hpp"
#include "constants.hpp"
#include "pointbox_type.hpp"

//Ben Arrowsmith D00257746

std::vector<AircraftData> InitializeAircraftData()
{
	std::vector<AircraftData> data(static_cast<int>(AircraftType::kAircraftCount));

	data[static_cast<int>(AircraftType::kEagle)].m_hitpoints = 100;
	data[static_cast<int>(AircraftType::kEagle)].m_speed = 200.f;
	data[static_cast<int>(AircraftType::kEagle)].m_fire_interval = sf::seconds(1);
	data[static_cast<int>(AircraftType::kEagle)].m_texture = TextureID::kEagle;

	data[static_cast<int>(AircraftType::kEagle2)].m_hitpoints = 100;//Ben Arrowsmith
	data[static_cast<int>(AircraftType::kEagle2)].m_speed = 200.f;//Ben Arrowsmith
	data[static_cast<int>(AircraftType::kEagle2)].m_fire_interval = sf::seconds(1);//Ben Arrowsmith
	data[static_cast<int>(AircraftType::kEagle2)].m_texture = TextureID::kEagle2;//Ben Arrowsmith

	data[static_cast<int>(AircraftType::kRaptor)].m_hitpoints = 20;
	data[static_cast<int>(AircraftType::kRaptor)].m_speed = 80.f;
	data[static_cast<int>(AircraftType::kRaptor)].m_fire_interval = sf::Time::Zero;
	data[static_cast<int>(AircraftType::kRaptor)].m_texture = TextureID::kRaptor;

	data[static_cast<int>(AircraftType::kRaptor)].m_directions.emplace_back(Direction(+45.f, 80.f));
	data[static_cast<int>(AircraftType::kRaptor)].m_directions.emplace_back(Direction(-45.f, 160.f));
	data[static_cast<int>(AircraftType::kRaptor)].m_directions.emplace_back(Direction(+45.f, 80.f));
	return data;
}

std::vector<ProjectileData> InitializeProjectileData()
{
	std::vector<ProjectileData> data(static_cast<int>(ProjectileType::kProjectileCount));
	data[static_cast<int>(ProjectileType::kAlliedBullet)].m_damage = 10;
	data[static_cast<int>(ProjectileType::kAlliedBullet)].m_speed = 300;
	data[static_cast<int>(ProjectileType::kAlliedBullet)].m_texture = TextureID::kBullet;

	data[static_cast<int>(ProjectileType::kEnemyBullet)].m_damage = 10;
	data[static_cast<int>(ProjectileType::kEnemyBullet)].m_speed = 300;
	data[static_cast<int>(ProjectileType::kEnemyBullet)].m_texture = TextureID::kBullet;

	data[static_cast<int>(ProjectileType::kMissile)].m_damage = 200;
	data[static_cast<int>(ProjectileType::kMissile)].m_speed = 150;
	data[static_cast<int>(ProjectileType::kMissile)].m_texture = TextureID::kMissile;


	return data;
}

std::vector<PickupData> InitializePickupData()
{
	std::vector<PickupData> data(static_cast<int>(PickupType::kPickupCount));
	data[static_cast<int>(PickupType::kHealthRefill)].m_texture = TextureID::kHealthRefill;
	data[static_cast<int>(PickupType::kHealthRefill)].m_action = [](Aircraft& a)
		{
			a.Repair(25);
		};

	data[static_cast<int>(PickupType::kMissileRefill)].m_texture = TextureID::kMissileRefill;
	data[static_cast<int>(PickupType::kMissileRefill)].m_action = std::bind(&Aircraft::CollectMissile, std::placeholders::_1, kMissileRefill);

	data[static_cast<int>(PickupType::kFireSpread)].m_texture = TextureID::kFireSpread;
	data[static_cast<int>(PickupType::kFireSpread)].m_action = std::bind(&Aircraft::IncreaseFireSpread, std::placeholders::_1);

	data[static_cast<int>(PickupType::kFireRate)].m_texture = TextureID::kFireRate;
	data[static_cast<int>(PickupType::kFireRate)].m_action = std::bind(&Aircraft::IncreaseFireRate, std::placeholders::_1);
	return data;
}

std::vector<PointBoxData> InitializePointBoxData()
{
	std::vector<PointBoxData> data(static_cast<int>(PointBoxType::kPointBoxCount));

	data[static_cast<int>(PointBoxType::kPlusOne)].m_point_value = 1;
	data[static_cast<int>(PointBoxType::kPlusOne)].m_speed = 100.f; //Fastest box cause its lower points
	data[static_cast<int>(PointBoxType::kPlusOne)].m_texture = TextureID::kPointBoxPlusOne;
	data[static_cast<int>(PointBoxType::kPlusOne)].m_directions.emplace_back(Direction(+10.f, 50.f));
	data[static_cast<int>(PointBoxType::kPlusOne)].m_directions.emplace_back(Direction(-10.f, 100.f));
	data[static_cast<int>(PointBoxType::kPlusOne)].m_directions.emplace_back(Direction(+10.f, 50.f));

	data[static_cast<int>(PointBoxType::kPlusTwo)].m_point_value = 2;
	data[static_cast<int>(PointBoxType::kPlusTwo)].m_speed = 85.f; //Slightly slower
	data[static_cast<int>(PointBoxType::kPlusTwo)].m_texture = TextureID::kPointBoxPlusTwo;
	data[static_cast<int>(PointBoxType::kPlusTwo)].m_directions.emplace_back(Direction(+20.f, 50.f));
	data[static_cast<int>(PointBoxType::kPlusTwo)].m_directions.emplace_back(Direction(+20.f, 100.f));
	data[static_cast<int>(PointBoxType::kPlusTwo)].m_directions.emplace_back(Direction(+20.f, 50.f));

	data[static_cast<int>(PointBoxType::kPlusThree)].m_point_value = 3;
	data[static_cast<int>(PointBoxType::kPlusThree)].m_speed = 75.f; //Keep the slowest
	data[static_cast<int>(PointBoxType::kPlusThree)].m_texture = TextureID::kPointBoxPlusThree;
	data[static_cast<int>(PointBoxType::kPlusThree)].m_directions.emplace_back(Direction(+30.f, 50.f));
	data[static_cast<int>(PointBoxType::kPlusThree)].m_directions.emplace_back(Direction(-30.f, 100.f));
	data[static_cast<int>(PointBoxType::kPlusThree)].m_directions.emplace_back(Direction(+30.f, 50.f));

	return data;
}
