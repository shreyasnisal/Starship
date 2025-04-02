#pragma once

#include "Game/GameCommon.hpp"

class Game;

enum GameState
{
	GAMESTATE_INVALID = -1,
	GAMESTATE_EXPLORATION,
	GAMESTATE_COMBAT,

	GAMESTATE_NUM,
};

class QuestManager
{
public:
	~QuestManager() = default;
	QuestManager() = default;
	QuestManager(Game* game);
	GameState GetGameState() { return m_gameState; };
	void ShowPlanetInteractionText(PlanetName planetName, Vec2 const& textPosition) const;
	void HandlePlanetInteraction(PlanetName planetName);
	void StartMercuryQuest();
	void StartVenusQuest();
	void StartMarsQuest();
	void StartNextWave();
	void StartNextMercuryWave();
	void StartNextVenusWave();
	void StartNextMarsWave();

public:
	float				m_combatTetherDistance = 0.f;

private:

private:
	Game*				m_game						= nullptr;
	GameState			m_gameState					= GAMESTATE_INVALID;
	PlanetName			m_nextQuestPlanet			= PLANET_MERCURY;
	int					m_currentWaveID				= -1;
};