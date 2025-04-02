#include "Game/Entity.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "Game/App.hpp"

Entity::Entity(Game* game, Vec2 const& startPosition)
{
	m_game = game;
	m_position = startPosition;

	m_hitSound = g_audio->CreateOrGetSound("Data/Audio/Entity_Hit.wav");
	m_dieSound = g_audio->CreateOrGetSound("Data/Audio/Entity_Die.wav");
}

bool Entity::IsOffScreen() const
{
	return 
	(
			(m_position.x + m_cosmeticRadius <= m_game->m_worldCamera.GetOrthoBottomLeft().x) ||
			(m_position.x - m_cosmeticRadius >= m_game->m_worldCamera.GetOrthoTopRight().x) ||
			(m_position.y + m_cosmeticRadius <= m_game->m_worldCamera.GetOrthoBottomLeft().y) ||
			( m_position.y - m_cosmeticRadius >= m_game->m_worldCamera.GetOrthoTopRight().y)
	);
}

Vec2 Entity::GetForwardNormal() const
{
	return Vec2(CosDegrees(m_orientationDegrees), SinDegrees(m_orientationDegrees));
}

void Entity::Die()
{
	m_isDead = true;
	m_isGarbage = true;
	if (m_entityType != EntityType::BULLET && m_entityType != EntityType::DEBRIS)
	{
		g_audio->StartSound(m_dieSound);
	}
}

void Entity::RenderDebug() const
{
	DebugDrawRing		(m_position,	m_physicsRadius,			0.2f, Rgba8(0, 255, 255, 255));
	DebugDrawRing		(m_position,	m_cosmeticRadius,			0.2f, Rgba8(255, 0, 255, 255));
	DebugDrawLine		(m_position,	m_position + m_velocity,	0.5f, Rgba8(255, 255, 0, 255));
	
	Vec2 const forwardNormal = GetForwardNormal();
	Vec2 const relativeLeftNormal = forwardNormal.GetRotated90Degrees();
	
	DebugDrawLine		(m_position,	m_position + forwardNormal * m_cosmeticRadius,		0.5f, Rgba8(255, 0, 0, 255));
	DebugDrawLine		(m_position,	m_position + relativeLeftNormal * m_cosmeticRadius, 0.5, Rgba8(0, 255, 0, 255));
}

void Entity::TakeDamage(int damage)
{
	m_health -= damage;

	if (m_health > 0)
	{
		g_audio->StartSound(m_hitSound);
	}
	else
	{
		SpawnDebrisCluster(MIN_DEBRIS_PER_ENTITY, MAX_DEBRIS_PER_ENTITY);
		Die();
	}
}

void Entity::SpawnDebrisCluster(int minDebris, int maxDebris, float debrisScale) const
{
	if (!m_isGarbage)
	{
		int numDebris = g_RNG->RollRandomIntInRange(minDebris, maxDebris);
		for (int debrisIndex = 0; debrisIndex < numDebris; debrisIndex++)
		{
			m_game->SpawnDebris(m_position, m_velocity, m_color, debrisScale);
		}
	}
}