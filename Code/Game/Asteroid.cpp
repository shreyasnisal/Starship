#include "Game/Asteroid.hpp"

#include "Game/App.hpp"
#include "Game/Bullet.hpp"
#include "Game/GameCommon.hpp"

Asteroid::Asteroid(Game* game, Vec2 const& startPosition, int asteroidID) : Entity(game, startPosition)
{
	m_entityType = EntityType::ASTEROID;
	m_physicsRadius = ASTEROID_PHYSICS_RADIUS;
	m_cosmeticRadius = ASTEROID_COSMETIC_RADIUS;
	m_health = ASTEROID_HEALTH;
	m_color = Rgba8(100, 100, 100, 255);
	InitializeVertexes();
	Vec2 asteroidVelocity = Vec2(g_RNG->RollRandomFloatInRange(-1.f, 1.f), g_RNG->RollRandomFloatInRange(-1.f, 1.f));
	asteroidVelocity = asteroidVelocity.GetNormalized();
	m_velocity = asteroidVelocity * ASTEROID_SPEED;
	m_angularVelocity = g_RNG->RollRandomFloatInRange(-200.f, 200.f);
	m_entityID = asteroidID + ASTEROID_ID_OFFSET;
}

void Asteroid::InitializeVertexes()
{
	Vec3 previousVertexPosition = Vec3(0.f, 0.f, 0.f);
	for (int vertexIndex = 0; vertexIndex < NUM_VERTEXES; vertexIndex++)
	{
		if (vertexIndex % 3 == 0)
		{
			m_vertexes[vertexIndex] = Vertex_PCU(Vec3(0.f, 0.f, 0.f), m_color, Vec2(0.f, 0.f));
		}
		else if (vertexIndex % 3 == 1)
		{
			if (previousVertexPosition.x == 0 && previousVertexPosition.y == 0)
			{
				Vec2 vertexPosition = Vec2::MakeFromPolarDegrees(0.f, g_RNG->RollRandomFloatInRange(m_physicsRadius, m_cosmeticRadius));
				m_vertexes[vertexIndex] = Vertex_PCU(Vec3(vertexPosition.x, vertexPosition.y, 0.f), m_color, Vec2(0.f, 0.f));
			}
			else
			{
				m_vertexes[vertexIndex] = Vertex_PCU(previousVertexPosition, m_color, Vec2(0.f, 0.f));
			}
		}
		else
		{
			Vec2 vertexPosition = Vec2::MakeFromPolarDegrees((1 + vertexIndex) * 360.f / 48.f, g_RNG->RollRandomFloatInRange(m_physicsRadius, m_cosmeticRadius));
			m_vertexes[vertexIndex] = Vertex_PCU(Vec3(vertexPosition.x, vertexPosition.y, 0.f), m_color, Vec2(0.f, 0.f));
			previousVertexPosition = Vec3(vertexPosition.x, vertexPosition.y, 0.f);
		}
	}
}

void Asteroid::Update(float deltaSeconds)
{
	if (!m_isDead)
	{
		m_position += m_velocity * deltaSeconds;
		m_orientationDegrees += m_angularVelocity * deltaSeconds;

		//CheckOutOfScreenAndWrap();

		if (m_health == 0)
		{
			Die();
			SpawnDebrisCluster(MIN_DEBRIS_PER_ENTITY, MAX_DEBRIS_PER_ENTITY);
		}
	}
}

void Asteroid::CheckOutOfScreenAndWrap()
{
	// Left Wall
	if (m_position.x < 0.f - m_cosmeticRadius)
	{
		m_position.x = m_game->WORLD_X + m_cosmeticRadius;
	}

	// Right Wall
	if (m_position.x > m_game->WORLD_X + m_cosmeticRadius)
	{
		m_position.x = 0.f - m_cosmeticRadius;
	}

	// Bottom Wall
	if (m_position.y < 0 - m_cosmeticRadius)
	{
		m_position.y = m_game->WORLD_Y + m_cosmeticRadius;
	}

	// Top Wall
	if (m_position.y > m_game->WORLD_Y + m_cosmeticRadius)
	{
		m_position.y = 0 - m_cosmeticRadius;
	}
}

void Asteroid::Render() const
{
	if (IsOffScreen())
	{
		return;
	}

	if (!m_isDead)
	{
		Vertex_PCU asteroidVerts[NUM_VERTEXES];
		for (int vertexIndex = 0; vertexIndex < NUM_VERTEXES; vertexIndex++)
		{
			asteroidVerts[vertexIndex] = m_vertexes[vertexIndex];
		}

		TransformVertexArrayXY3D(NUM_VERTEXES, asteroidVerts, 1.f, m_orientationDegrees, m_position);

		g_renderer->BindTexture(nullptr);
		g_renderer->DrawVertexArray(NUM_VERTEXES, asteroidVerts);
	}

	if (m_game->m_drawDebug)
	{
		RenderDebug();
	}
}