#pragma once

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

#include "Game/Entity.hpp"
#include "Game/Planet.hpp"

class Renderer;

extern Renderer* g_renderer;

class PlayerShip : public Entity
{
public:
								~PlayerShip									() = default;
								PlayerShip									(Game* game, Vec2 const& startPosition);
	void						InitializeVertexes							();
	void						Update										(float deltaSeconds) override;
	void						UpdateFromController						(float& deltaSeconds);
	void						UpdateFromKeyboard							(float& deltaSeconds);
	void						CheckCollisionsWithWalls					();
	void						Render										() const override;
	void						Die											() override;
	void						ResetAndRespawn								(Vec2 const& resetPosition);
	void						UpdateFromAI								(float deltaSeconds);
	Entity*						GetNearestEnemy								();
	Entity*						GetNearestEnemyInList						(int numEnemies, Entity** enemies);
	void						RenderLowHealthIndicator					() const;
	void						UpdateLandingOnPlanet						(float timeUntilLanding);
	void						UpdateTakingOffFromPlanet					(float timeUntilTakenOff);

public:
	static constexpr int		NUM_VERTEXES = PLAYERSHIP_NUM_VERTEXES;
	Vertex_PCU					m_vertexes[NUM_VERTEXES];
	float						m_thrustFraction = 0.f;
	float						m_aiShootDelay = AI_SHOOT_DELAY;
	float						m_aiRespawnDelay = AI_RESPAWN_DELAY;
	int							m_numExtraLivesRemaining = PLAYERSHIP_LIVES;
	float						m_bulletInaccuracy = 0.f;
	int							m_numBulletsRemaining = PLAYERSHIP_MAX_BULLETS;
	float						m_maxThrust = 1.f;
	bool						m_isOnPlanet = false;
	Planet*						m_landedPlanet = nullptr;

private:
	void						SpawnExhaustDebris							();
	void						RenderHealthBar								() const;
	void						HyperSpeedTowardsEarth						(float deltaSeconds);

private:
	SoundID						m_shootSound;
	SoundID						m_dieSound;
	float						m_tailLength								= 0.f;
	Rgba8						m_hyperSpeedTailColor						= Rgba8(100, 0, 255, 255);
	bool						m_isHyperSpeedModeOn						= false;
	static constexpr float		HEALTH_INDICATOR_Y_OFFSET					= 6.f;
	static constexpr float		HEALTH_INDICATOR_BOX_WIDTH					= 10.f;
	static constexpr float		HEALTH_INDICATOR_BOX_HEIGHT					= 1.f;
};