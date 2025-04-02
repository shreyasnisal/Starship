#include "Game/Wasp.hpp"

#include "Game/Bullet.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Game/PlayerShip.hpp"

Wasp::Wasp(Game* game, Vec2 const& startPos, int waspID) : Entity(game, startPos)
{
	m_entityType = EntityType::WASP;
	m_orientationDegrees = (game->m_playerShip->m_position - startPos).GetOrientationDegrees();
	m_health = 3;
	m_physicsRadius = WASP_PHYSICS_RADIUS;
	m_cosmeticRadius = WASP_COSMETIC_RADIUS;
	//m_color = Rgba8(255, 255, 0, 255);
	unsigned char redByte = (unsigned char)g_RNG->RollRandomIntLessThan(255);
	unsigned char greenByte = (unsigned char)g_RNG->RollRandomIntLessThan(255);
	unsigned char blueByte = (unsigned char)g_RNG->RollRandomIntLessThan(255);
	m_color = Rgba8(redByte, greenByte, blueByte, 255);
	m_entityID = WASP_ID_OFFSET + waspID;
}

void Wasp::Update(float deltaSeconds)
{
	Vec2 acceleration = Vec2();
	if (m_game->IsEntityAlive(m_game->m_playerShip))
	{
		m_orientationDegrees = (m_game->m_playerShip->m_position - m_position).GetOrientationDegrees();
		acceleration = (m_game->m_playerShip->m_position - m_position).GetNormalized() * WASP_THRUST;
	}
	m_velocity += acceleration * deltaSeconds;
	m_velocity.SetLength(GetClamped(m_velocity.GetLength(), 0.f, WASP_MAX_SPEED));
	m_position += m_velocity * deltaSeconds;

  	FireBulletTowardsPlayer(deltaSeconds);

	if (deltaSeconds > 0.f)
	{
		SpawnExhaustDebris();
		m_tailLength = g_RNG->RollRandomFloatInRange(TAIL_END_MIN, TAIL_END_MAX);
	
		if (m_health <= 1)
		{
			SpawnSmoke();
		}
	}

	if (m_health == 0)
	{
		Die();
		SpawnDebrisCluster(MIN_DEBRIS_PER_ENTITY, MAX_DEBRIS_PER_ENTITY);
	}
}

void Wasp::Render() const
{
	if (IsOffScreen())
	{
		return;
	}

	if (!m_isDead)
	{
		Vertex_PCU waspVerts[NUM_VERTEXES];

		for (int vertexIndex = 0; vertexIndex < NUM_VERTEXES; vertexIndex++)
		{
			waspVerts[vertexIndex] = m_vertexes[vertexIndex];

			if (vertexIndex >= 18 && vertexIndex <= 29)
			{
				waspVerts[vertexIndex].m_color = m_color;
			}
		}

		TransformVertexArrayXY3D(NUM_VERTEXES, waspVerts, 1.f, m_orientationDegrees, m_position);
		g_renderer->BindTexture(nullptr);
		g_renderer->DrawVertexArray(NUM_VERTEXES, waspVerts);

		Vertex_PCU waspTailVerts[NUM_TAIL_VERTEXES];
		for (int vertexIndex = 0; vertexIndex < NUM_TAIL_VERTEXES; vertexIndex++)
		{
			waspTailVerts[vertexIndex] = m_tailVertexes[vertexIndex];

			if (vertexIndex == NUM_TAIL_VERTEXES - 1)
			{
				waspTailVerts[vertexIndex].m_position.x = m_tailLength;
			}
		}

		TransformVertexArrayXY3D(NUM_TAIL_VERTEXES, waspTailVerts, 1.f, m_orientationDegrees, m_position);
		g_renderer->BindTexture(nullptr);
		g_renderer->DrawVertexArray(NUM_TAIL_VERTEXES, waspTailVerts);
	}

	if (m_game->m_drawDebug)
	{
		RenderDebug();
	}
}

void Wasp::SpawnExhaustDebris()
{
	// Spawn 0 or 1 debris
	// This function is called each frame, so shouldn't spawn too many exhaust debris at once
	int numDebris = g_RNG->RollRandomIntInRange(0, 1);
	for (int debrisIndex = 0; debrisIndex < numDebris; debrisIndex++)
	{
		m_game->SpawnDebris(m_position + GetForwardNormal() * TAIL_END_MAX, -GetForwardNormal() * 100.f, Rgba8(255, 100, 0, 255), 0.4f, 0.02f);
	}
}

void Wasp::FireBulletTowardsPlayer(float deltaSeconds)
{
	m_shootDelay -= deltaSeconds;

	if (m_shootDelay > 0.f)
	{
		return;
	}

	float distanceToPlayer = GetDistance2D(m_position, m_game->m_playerShip->m_position);
	float bulletTime = distanceToPlayer / BULLET_SPEED;

	if (bulletTime > BULLET_LIFESPAN * 0.5f)
	{
		return;
	}

	m_game->SpawnNewBullet(m_position + GetForwardNormal() * 4.f, m_orientationDegrees, m_color, m_entityID);

	m_shootDelay = WASP_SHOOT_DELAY;
}

void Wasp::SpawnSmoke()
{
	// Spawn 0 or 1 debris
	// This function is called each frame, so shouldn't spawn too many exhaust debris at once
	int numDebris = g_RNG->RollRandomIntInRange(0, 1);
	for (int debrisIndex = 0; debrisIndex < numDebris; debrisIndex++)
	{
		m_game->SpawnDebris(m_position, Vec2(0.f, 100.f), Rgba8(50, 50, 50, 255), 2.f, 0.2f);
	}
}