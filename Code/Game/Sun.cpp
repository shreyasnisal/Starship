#include "Game/Sun.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Beetle.hpp"
#include "Game/Wasp.hpp"

Sun::Sun(Game* game, Vec2 const& startPos) : Entity(game, startPos)
{
	m_cosmeticRadius = m_game->SUN_RADIUS;
	m_color = Rgba8(255, 127, 40, 255);
	//m_gravityScale = 2800.f;
}

void Sun::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	/*if (m_game->IsEntityAlive(m_game->m_playerShip))
	{
		Vec2 gravityDirection = (m_position - m_game->m_playerShip->m_position).GetNormalized();
		float entityDistanceFromPlanetCenter = GetDistance2D(m_game->m_playerShip->m_position, m_position);
		Vec2 gravityAcceleration = gravityDirection * m_gravityScale / entityDistanceFromPlanetCenter;
		m_game->m_playerShip->m_velocity += gravityAcceleration * deltaSeconds;
	}

	ApplyGravityOnEntityList(MAX_ASTEROIDS, m_game->m_asteroids, deltaSeconds);
	ApplyGravityOnEntityList(MAX_BEETLES, m_game->m_beetles, deltaSeconds);
	ApplyGravityOnEntityList(MAX_WASPS, m_game->m_wasps, deltaSeconds);*/
}

void Sun::Render() const
{
	DebugDrawGlow(m_position, m_cosmeticRadius * 2.f, m_color, Rgba8(m_color.r, m_color.g, m_color.b, 0));
	if (IsOffScreen())
	{
		return;
	}

	DebugDrawDisc(m_position, m_cosmeticRadius, m_color);
}