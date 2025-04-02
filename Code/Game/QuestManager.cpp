#include "QuestManager.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Planet.hpp"

QuestManager::QuestManager(Game* game)
{
	m_game = game;
	m_gameState = GAMESTATE_EXPLORATION;
}

void QuestManager::ShowPlanetInteractionText(PlanetName planetName, Vec2 const& textPosition) const
{
	if (planetName == m_nextQuestPlanet && m_gameState == GAMESTATE_EXPLORATION)
	{
		std::vector<Vertex_PCU> questStartTextVerts;
		AddVertsForTextTriangles2D(questStartTextVerts, "Press R to land on planet", textPosition + Vec2(0.f, 30.f), 20.f, Rgba8::WHITE);
		AddVertsForTextTriangles2D(questStartTextVerts, "Press Q to start quest", textPosition, 20.f, Rgba8(255, 255, 255, 255));
		g_renderer->BindTexture(nullptr);
		g_renderer->DrawVertexArray((int)questStartTextVerts.size(), questStartTextVerts.data());
	}
}

void QuestManager::HandlePlanetInteraction(PlanetName planetName)
{
	if (planetName == m_nextQuestPlanet && m_gameState == GAMESTATE_EXPLORATION)
	{
		if (planetName == PLANET_MERCURY)
		{
			StartMercuryQuest();
		}
		else if (planetName == PLANET_VENUS)
		{
			StartVenusQuest();
		}
		else if (planetName == PLANET_MARS)
		{
			StartMarsQuest();
		}
	}
}

void QuestManager::StartNextWave()
{
	m_currentWaveID++;

	if (m_nextQuestPlanet == PLANET_MERCURY)
	{
		StartNextMercuryWave();
	}
	else if (m_nextQuestPlanet == PLANET_VENUS)
	{
		StartNextVenusWave();
	}
	else if (m_nextQuestPlanet == PLANET_MARS)
	{
		StartNextMarsWave();
	}
}

void QuestManager::StartMercuryQuest()
{
	m_gameState = GAMESTATE_COMBAT;

	StartNextWave();
}

void QuestManager::StartNextMercuryWave()
{
	if (m_currentWaveID == NUM_MERCURY_WAVES)
	{
		m_gameState = GAMESTATE_EXPLORATION;
		m_nextQuestPlanet = PLANET_VENUS;
		m_currentWaveID = -1;
		return;
	}

	for (int waspIndex = 0; waspIndex < WASPS_IN_MERCURY_WAVE[m_currentWaveID]; waspIndex++)
	{
		Vec2 const spawnPoint = m_game->GetRandomPointOffScreenByOffset(WASP_COSMETIC_RADIUS);
		m_game->SpawnNewWasp(spawnPoint);
	}
}

void QuestManager::StartVenusQuest()
{
	m_gameState = GAMESTATE_COMBAT;

	StartNextWave();
}

void QuestManager::StartNextVenusWave()
{
	if (m_currentWaveID == NUM_VENUS_WAVES)
	{
		m_gameState = GAMESTATE_EXPLORATION;
		m_nextQuestPlanet = PLANET_MARS;
		m_currentWaveID = -1;
		return;
	}

	for (int waspIndex = 0; waspIndex < WASPS_IN_VENUS_WAVE[m_currentWaveID]; waspIndex++)
	{
		Vec2 const spawnPoint = m_game->GetRandomPointOffScreenByOffset(WASP_COSMETIC_RADIUS);
		m_game->SpawnNewWasp(spawnPoint);
	}

	for (int missileShooterIndex = 0; missileShooterIndex < MISSILE_SHOOTERS_IN_VENUS_WAVE[m_currentWaveID]; missileShooterIndex++)
	{
		//Vec2 const spawnPoint = m_game->GetRandomPointOffScreenByOffset(MISSILE_SHOOOTER_COSMETIC_RADIUS);
		//m_game->SpawnNewMissileShooter(spawnPoint);
	}

	for (int dualShooterIndex = 0; dualShooterIndex < DUAL_SHOOTERS_IN_VENUS_WAVE[m_currentWaveID]; dualShooterIndex++)
	{
		Vec2 const spawnPoint = m_game->GetRandomPointOffScreenByOffset(DUAL_SHOOTER_COSMETIC_RADIUS);
		m_game->SpawnNewDualShooter(spawnPoint);
	}
}

void QuestManager::StartMarsQuest()
{
	m_gameState = GAMESTATE_COMBAT;

	StartNextWave();
}

void QuestManager::StartNextMarsWave()
{
	if (m_currentWaveID == NUM_VENUS_WAVES)
	{
		m_gameState = GAMESTATE_EXPLORATION;
		m_nextQuestPlanet = PLANET_INVALID;
		m_currentWaveID = -1;
		return;
	}

	for (int waspIndex = 0; waspIndex < WASPS_IN_VENUS_WAVE[m_currentWaveID]; waspIndex++)
	{
		Vec2 const spawnPoint = m_game->GetRandomPointOffScreenByOffset(WASP_COSMETIC_RADIUS);
		m_game->SpawnNewWasp(spawnPoint);
	}

	for (int missileShooterIndex = 0; missileShooterIndex < MISSILE_SHOOTERS_IN_VENUS_WAVE[m_currentWaveID]; missileShooterIndex++)
	{
		//Vec2 const spawnPoint = m_game->GetRandomPointOffScreenByOffset(MISSILE_SHOOOTER_COSMETIC_RADIUS);
		//m_game->SpawnNewMissileShooter(spawnPoint);
	}

	for (int dualShooterIndex = 0; dualShooterIndex < DUAL_SHOOTERS_IN_VENUS_WAVE[m_currentWaveID]; dualShooterIndex++)
	{
		Vec2 const spawnPoint = m_game->GetRandomPointOffScreenByOffset(DUAL_SHOOTER_COSMETIC_RADIUS);
		m_game->SpawnNewDualShooter(spawnPoint);
	}

	for (int multiShooterIndex = 0; multiShooterIndex < MULTI_SHOOTERS_IN_MARS_WAVE[m_currentWaveID]; multiShooterIndex++)
	{
		//Vec2 const spawnPoint = m_game->GetRandomPointOffScreenByOffset(MULTI_SHOOTER_COSMETIC_RADIUS);
		//m_game->SpawnNewMultiShooter(spawnPoint);
	}
}