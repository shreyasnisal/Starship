#include "Game/DualShooter.hpp"

#include "Game/Bullet.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Game/PlayerShip.hpp"

DualShooter::DualShooter(Game* game, Vec2 const& startPos, int dualShooterID) : Entity(game, startPos)
{
	m_entityType = EntityType::DUAL_SHOOTER;
	m_orientationDegrees = (game->m_playerShip->m_position - startPos).GetOrientationDegrees();
	m_health = 3;
	m_physicsRadius = DUAL_SHOOTER_PHYSICS_RADIUS;
	m_cosmeticRadius = DUAL_SHOOTER_COSMETIC_RADIUS;
	unsigned char redByte = (unsigned char)g_RNG->RollRandomIntLessThan(255);
	unsigned char greenByte = (unsigned char)g_RNG->RollRandomIntLessThan(255);
	unsigned char blueByte = (unsigned char)g_RNG->RollRandomIntLessThan(255);
	m_color = Rgba8(redByte, greenByte, blueByte, 255);
	m_entityID = WASP_ID_OFFSET + dualShooterID;
}

void DualShooter::Update(float deltaSeconds)
{
	Vec2 acceleration = Vec2();
	if (m_game->IsEntityAlive(m_game->m_playerShip))
	{
		m_orientationDegrees = (m_game->m_playerShip->m_position - m_position).GetOrientationDegrees();
		acceleration = (m_game->m_playerShip->m_position - m_position).GetNormalized() * DUAL_SHOOTER_THRUST;
	}
	m_velocity += acceleration * deltaSeconds;
	m_velocity.SetLength(GetClamped(m_velocity.GetLength(), 0.f, DUAL_SHOOTER_MAX_SPEED));
	m_position += m_velocity * deltaSeconds;

	FireBulletsTowardsPlayer(deltaSeconds);

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

void DualShooter::Render() const
{
	if (IsOffScreen())
	{
		return;
	}

	if (!m_isDead)
	{
		Vertex_PCU dualShooterVerts[NUM_VERTEXES];

		for (int vertexIndex = 0; vertexIndex < NUM_VERTEXES; vertexIndex++)
		{
			dualShooterVerts[vertexIndex] = m_vertexes[vertexIndex];

			if (vertexIndex >= 18 && vertexIndex <= 47)
			{
				dualShooterVerts[vertexIndex].m_color = m_color;
			}
		}

		TransformVertexArrayXY3D(NUM_VERTEXES, dualShooterVerts, 1.f, m_orientationDegrees, m_position);
		g_renderer->BindTexture(nullptr);
		g_renderer->DrawVertexArray(NUM_VERTEXES, dualShooterVerts);

		Vertex_PCU dualShooterTailVerts[NUM_TAIL_VERTEXES];
		for (int vertexIndex = 0; vertexIndex < NUM_TAIL_VERTEXES; vertexIndex++)
		{
			dualShooterTailVerts[vertexIndex] = m_tailVertexes[vertexIndex];

			if (vertexIndex % 3 == 2)
			{
				dualShooterTailVerts[vertexIndex].m_position.x = m_tailLength;
				dualShooterTailVerts[vertexIndex].m_color.a = 0;
			}
		}

		TransformVertexArrayXY3D(NUM_TAIL_VERTEXES, dualShooterTailVerts, 1.f, m_orientationDegrees, m_position);
		g_renderer->BindTexture(nullptr);
		g_renderer->DrawVertexArray(NUM_TAIL_VERTEXES, dualShooterTailVerts);
	}

	if (m_game->m_drawDebug)
	{
		RenderDebug();
	}
}

void DualShooter::SpawnExhaustDebris()
{
	// Spawn 0 or 1 debris
	// This function is called each frame, so shouldn't spawn too many exhaust debris at once
	int numDebris = g_RNG->RollRandomIntInRange(0, 1);
	for (int debrisIndex = 0; debrisIndex < numDebris; debrisIndex++)
	{
		Vec2 leftTailExhaustPosition = m_position + Vec2(GetForwardNormal().x * TAIL_END_MAX, GetForwardNormal().GetRotated90Degrees().y * 1.5f);
		Vec2 rightTailExhaustPosition = m_position + Vec2(GetForwardNormal().x * TAIL_END_MAX, GetForwardNormal().GetRotatedMinus90Degrees().y * 1.5f);
		m_game->SpawnDebris(leftTailExhaustPosition, -GetForwardNormal() * 100.f, Rgba8(255, 100, 0, 255), 0.4f, 0.02f);
		m_game->SpawnDebris(rightTailExhaustPosition, -GetForwardNormal() * 100.f, Rgba8(255, 100, 0, 255), 0.4f, 0.02f);
	}
}

void DualShooter::FireBulletsTowardsPlayer(float deltaSeconds)
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

	Vec2 leftBulletSpawnPosition = m_position + Vec2(GetForwardNormal().x * 8.f, GetForwardNormal().GetRotated90Degrees().y * 6.f);
	Vec2 rightBulletSpawnPosition = m_position + Vec2(GetForwardNormal().x * 8.f, GetForwardNormal().GetRotatedMinus90Degrees().y * 6.f);
	m_game->SpawnNewBullet(leftBulletSpawnPosition, m_orientationDegrees, m_color, m_entityID);
	m_game->SpawnNewBullet(rightBulletSpawnPosition, m_orientationDegrees, m_color, m_entityID);

	m_shootDelay = DUAL_SHOOTER_SHOOT_DELAY;
}

void DualShooter::SpawnSmoke()
{
	// Spawn 0 or 1 debris
	// This function is called each frame, so shouldn't spawn too many exhaust debris at once
	int numDebris = g_RNG->RollRandomIntInRange(0, 1);
	for (int debrisIndex = 0; debrisIndex < numDebris; debrisIndex++)
	{
		m_game->SpawnDebris(m_position, Vec2(0.f, 100.f), Rgba8(50, 50, 50, 255), 2.f, 0.2f);
	}
}