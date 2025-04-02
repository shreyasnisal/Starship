#include "Game/Debris.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"

Debris::Debris(Game* game, Vec2 const& startPosition, Vec2 const& startVelocity, Rgba8 const& color, float scale, float lifespan) : Entity(game, startPosition)
{
	m_entityType = EntityType::DEBRIS;
	m_velocity = startVelocity + g_RNG->RollRandomVec2InRange(
											-DEBRIS_SPAWN_RANDOM_SPEED_LIMITS,
											DEBRIS_SPAWN_RANDOM_SPEED_LIMITS,
											-DEBRIS_SPAWN_RANDOM_SPEED_LIMITS,
											DEBRIS_SPAWN_RANDOM_SPEED_LIMITS);
	m_scale = scale;
	m_physicsRadius = DEBRIS_PHYSICS_RADIUS;
	m_cosmeticRadius = DEBRIS_COSMETIC_RADIUS;
	m_angularVelocity = g_RNG->RollRandomFloatInRange(0, DEBRIS_ANGULAR_SPEED);

	if (lifespan != 0)
	{
		m_lifespan = lifespan;
	}

	Vec3 previousVertexPosition = Vec3(0.f, 0.f, 0.f);
	for (int vertexIndex = 0; vertexIndex < NUM_VERTEXES; vertexIndex++)
	{
		if (vertexIndex % 3 == 0)
		{
			m_vertexes[vertexIndex] = Vertex_PCU(Vec3(0.f, 0.f, 0.f), color, Vec2(0.f, 0.f));
		}
		else if (vertexIndex % 3 == 1)
		{
			if (previousVertexPosition.x == 0 && previousVertexPosition.y == 0)
			{
				Vec2 vertexPosition = Vec2::MakeFromPolarDegrees(0.f, g_RNG->RollRandomFloatInRange(m_physicsRadius, m_cosmeticRadius));
				m_vertexes[vertexIndex] = Vertex_PCU(Vec3(vertexPosition.x, vertexPosition.y, 0.f), color, Vec2(0.f, 0.f));
			}
			else
			{ 
				m_vertexes[vertexIndex] = Vertex_PCU(previousVertexPosition, color, Vec2(0.f, 0.f));
			}
		}
		else
		{
			Vec2 vertexPosition = Vec2::MakeFromPolarDegrees((1 + vertexIndex) * 360.f / (float)(NUM_VERTEXES), g_RNG->RollRandomFloatInRange(m_physicsRadius, m_cosmeticRadius));
			m_vertexes[vertexIndex] = Vertex_PCU(Vec3(vertexPosition.x, vertexPosition.y, 0.f), color, Vec2(0.f, 0.f));
			previousVertexPosition = Vec3(vertexPosition.x, vertexPosition.y, 0.f);
		}
	}
}

void Debris::Update(float deltaSeconds)
{
	m_position += m_velocity * deltaSeconds;
	m_lifespan -= deltaSeconds;
	m_orientationDegrees += m_angularVelocity * deltaSeconds;
	float opacity = (float)m_opacity - 127.f * 1.f / m_lifespan * deltaSeconds;
	if (opacity > 0.f)
	{
		m_opacity = (unsigned char)(opacity);
	}

	if (opacity <= 0 || m_lifespan <= 0.f)
	{
		Die();
	}
}

void Debris::Render() const
{
	if (!m_isDead)
	{
		Vertex_PCU debrisVerts[NUM_VERTEXES];
		for (int vertexIndex = 0; vertexIndex < NUM_VERTEXES; vertexIndex++)
		{
			debrisVerts[vertexIndex] = m_vertexes[vertexIndex];
			debrisVerts[vertexIndex].m_color.a = m_opacity;
		}

		TransformVertexArrayXY3D(NUM_VERTEXES, debrisVerts, m_scale, m_orientationDegrees, m_position);
		g_renderer->BindTexture(nullptr);
		g_renderer->DrawVertexArray(NUM_VERTEXES, debrisVerts);
	}

	if (m_game->m_drawDebug)
	{
		RenderDebug();
	}
}

void Debris::TakeDamage(int damage)
{
	UNUSED(damage);
	ERROR_RECOVERABLE("Trying to deal damage to debris. Debris have no collisions effects and cannot take damage");
}