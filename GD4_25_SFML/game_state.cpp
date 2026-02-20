#include "game_state.hpp"
#include "mission_status.hpp"
#include <iostream>

GameState::GameState(StateStack& stack, Context context) : State(stack, context), m_world(*context.window, *context.fonts), m_player(*context.player), m_player2(*context.player2)
{

}

void GameState::Draw()
{
	m_world.Draw();
}

bool GameState::Update(sf::Time dt)
{
	m_world.Update(dt);

	if (!m_world.HasAlivePlayer())
	{
		m_player.SetMissionStatus(MissionStatus::kMissionFailure);
		RequestStackPush(StateID::kGameOver);
	}
	else if (m_world.HasPlayerReachedEnd())
	{
		m_player.SetMissionStatus(MissionStatus::kMissionSuccess);
		RequestStackPush(StateID::kGameOver);
	}
	else if (m_world.HasPlayerReachedPoints()) {
		int winner = m_world.GetWinningPlayer();

		if (winner == 1) {
			std::cout << "Player 1 Wins" << std::endl;
			m_player.SetMissionStatus(MissionStatus::kMissionSuccess);

		}
		else if (winner == 2) {
			std::cout << "Player 2 Wins" << std::endl;
			m_player.SetMissionStatus(MissionStatus::kMissionSuccess);
		}
 	}

	CommandQueue& commands = m_world.GetCommandQueue();
	m_player.HandleRealTimeInput(commands);
	m_player2.HandleRealTimeInput(commands);
	return true;
}

bool GameState::HandleEvent(const sf::Event& event)
{
	CommandQueue& commands = m_world.GetCommandQueue();
	m_player.HandleEvent(event, commands);
	m_player2.HandleEvent(event, commands);

	//Escape should bring up the pause menu
	const auto* keypress = event.getIf<sf::Event::KeyPressed>();
	if(keypress && keypress->scancode == sf::Keyboard::Scancode::Escape)
	{
		RequestStackPush(StateID::kPause);
	}
	return true;
}


