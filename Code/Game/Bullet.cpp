#include "Game/Bullet.hpp"

#include "Game/GameCommon.hpp"

Bullet::Bullet(Game* game, Vec2 const& startPosition, float direction, Rgba8 const& tailColor, int shooterID) : Entity(game, startPosition)
{
	m_entityType = EntityType::BULLET;
	m_physicsRadius = BULLET_PHYSICS_RADIUS;
	m_cosmeticRadius = BULLET_COSMETIC_RADIUS;
	m_orientationDegrees = direction;
	m_velocity = BULLET_SPEED * GetForwardNormal();
	m_color = Rgba8(255, 255, 0, 255);
	m_tailColor = tailColor;
	m_shooterID = shooterID;
}

void Bullet::Update(float deltaSeconds)
{
	if (!m_isDead)
	{
		m_position += m_velocity * deltaSeconds;
		m_age += deltaSeconds;

		if (m_age > BULLET_LIFESPAN)
		{
			Die();
		}

		if (m_age >= 2.f || m_health == 0)
		{
			Die();
		}
	}
}

void Bullet::Render() const
{
	if (!m_isDead)
	{
		Vertex_PCU bulletVerts[NUM_VERTEXES];
		for (int vertexIndex = 0; vertexIndex < NUM_VERTEXES; vertexIndex++)
		{
			bulletVerts[vertexIndex] = m_vertexes[vertexIndex];

			if (vertexIndex >= 3)
			{
				bulletVerts[vertexIndex].m_color = m_tailColor;
			}

			if (vertexIndex == NUM_VERTEXES - 1)
			{
				bulletVerts[vertexIndex].m_color.a = 0;
			}
		}

		TransformVertexArrayXY3D(NUM_VERTEXES, bulletVerts, 1.f, m_orientationDegrees, m_position);
		g_renderer->BindTexture(nullptr);
		g_renderer->DrawVertexArray(NUM_VERTEXES, bulletVerts);

		DebugDrawGlow(m_position, m_physicsRadius * 5.f, m_tailColor, Rgba8(m_tailColor.r, m_tailColor.g, m_tailColor.b, 0));
	}

	if (m_game->m_drawDebug)
	{
		RenderDebug();
	}
}

void Bullet::TakeDamage(int damage)
{
	m_health -= damage;
	SpawnDebrisCluster(MIN_DEBRIS_ON_BULLET_HIT, MAX_DEBRIS_ON_BULLET_HIT, DEBRIS_SCALE_ON_BULLET_HIT);
}

void Bullet::Die()
{
	m_isDead = true;
	m_isGarbage = true;
}

void Bullet::SpawnDebrisCluster(int minDebris, int maxDebris, float debrisScale) const
{
	int numDebris = g_RNG->RollRandomIntInRange(minDebris, maxDebris);
	for (int debrisIndex = 0; debrisIndex < numDebris; debrisIndex++)
	{
		m_game->SpawnDebris(m_position, -m_velocity, m_color, debrisScale);
	}
}