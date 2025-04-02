#include "Game/Beetle.hpp"

#include "Game/Bullet.hpp"
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"

Beetle::Beetle(Game* game, Vec2 const& startPos, int beetleID) : Entity(game, startPos)
{
	m_entityType = EntityType::BEETLE;
	m_orientationDegrees = (game->m_playerShip->m_position - startPos).GetOrientationDegrees();
	m_velocity = GetForwardNormal() * g_RNG->RollRandomFloatInRange(BEETLE_MIN_SPEED, BEETLE_MAX_SPEED);
	m_health = 3;
	m_physicsRadius = BEETLE_PHYSICS_RADIUS;
	m_cosmeticRadius = BEETLE_COSMETIC_RADIUS;
	m_color = Rgba8(160, 50, 50, 255);
	m_entityID = beetleID + BEETLE_ID_OFFSET;
}

void Beetle::Update(float deltaSeconds)
{
	if (m_game->IsEntityAlive(m_game->m_playerShip))
	{
		m_orientationDegrees = GetTurnedTowardDegrees(m_orientationDegrees, (m_game->m_playerShip->m_position - m_position).GetOrientationDegrees(), BEETLE_ANGULAR_VELOCITY * deltaSeconds);
		m_velocity = GetForwardNormal() * m_velocity.GetLength();
	}

	m_position += m_velocity * deltaSeconds;

	if (deltaSeconds > 0.f)
	{
		SpawnExhaustDebris();
		m_tailLength = g_RNG->RollRandomFloatInRange(TAIL_END_MIN, TAIL_END_MAX);
	}

	if (m_health == 0)
	{
		Die();
		SpawnDebrisCluster(MIN_DEBRIS_PER_ENTITY, MAX_DEBRIS_PER_ENTITY);
	}
}

void Beetle::Render() const
{
	if (IsOffScreen())
	{
		return;
	}

	if (!m_isDead)
	{
		Vertex_PCU beetleVerts[NUM_VERTEXES];
		for (int vertexIndex = 0; vertexIndex < NUM_VERTEXES; vertexIndex++)
		{
			beetleVerts[vertexIndex] = m_vertexes[vertexIndex];
		}

		TransformVertexArrayXY3D(NUM_VERTEXES, beetleVerts, 1.f, m_orientationDegrees, m_position);
		g_renderer->BindTexture(nullptr);
		g_renderer->DrawVertexArray(NUM_VERTEXES, beetleVerts);

		Vertex_PCU beetleTailVerts[NUM_TAIL_VERTEXES];
		for (int vertexIndex = 0; vertexIndex < NUM_TAIL_VERTEXES; vertexIndex++)
		{
			beetleTailVerts[vertexIndex] = m_tailVertexes[vertexIndex];

			if (vertexIndex == NUM_TAIL_VERTEXES - 1)
			{
				beetleTailVerts[vertexIndex].m_position.x = m_tailLength;
			}
		}

		TransformVertexArrayXY3D(NUM_TAIL_VERTEXES, beetleTailVerts, 1.f, m_orientationDegrees, m_position);
		g_renderer->BindTexture(nullptr);
		g_renderer->DrawVertexArray(NUM_TAIL_VERTEXES, beetleTailVerts);
	}

	if (m_game->m_drawDebug)
	{
		RenderDebug();
	}
}

void Beetle::SpawnExhaustDebris()
{
	// Spawn 0 or 1 debris
	// This function is called each frame, so shouldn't spawn too many exhaust debris at once
	int numDebris = g_RNG->RollRandomIntInRange(0, 1);
	for (int debrisIndex = 0; debrisIndex < numDebris; debrisIndex++)
	{
		m_game->SpawnDebris(m_position + GetForwardNormal() * TAIL_END_MAX, -GetForwardNormal() * 100.f, Rgba8(255, 100, 0, 255), 0.4f, 0.02f);
	}
}