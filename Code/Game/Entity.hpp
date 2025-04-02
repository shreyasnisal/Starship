#pragma once

#include "Engine/Math/Vec2.hpp"

#include "Game/App.hpp"
#include "Game/GameCommon.hpp"

class Game;
class App;
struct Vertex_PCU;


enum class EntityType
{
	UNDEFINED,
	PLAYERSHIP,
	BULLET,
	ASTEROID,
	BEETLE,
	WASP,
	DEBRIS,
	PLANET,
	DUAL_SHOOTER,
	MUTLI_SHOOTER,
	MISSILE_SHOOTER
};

class Entity
{

public:
	virtual						~Entity										() = default;
								Entity										(Game* game, Vec2 const& startPosition);
	virtual void				Update										(float deltaSeconds) = 0;
	virtual void				Render										() const = 0;
	virtual void				TakeDamage									(int damage);
	
	virtual void				Die											();
	bool						IsOffScreen									() const;

	int							GetEntityID									() const { return m_entityID; };

public:
	Vec2					m_position;
	Vec2					m_velocity;
	float					m_orientationDegrees				= 0.f;
	float					m_angularVelocity					= 0.f;
	float					m_physicsRadius						= 0.f;
	float					m_cosmeticRadius					= 0.f;
	int						m_health							= 1;
	bool					m_isDead							= false;
	bool					m_isGarbage							= false;
	Game*					m_game;
	Rgba8					m_color;
	EntityType				m_entityType						= EntityType::UNDEFINED;
	float					m_scale								= 1.f;


protected:
	virtual void					SpawnDebrisCluster						(int minDebris, int maxDebris, float debrisScale = 1.f) const;
	Vec2							GetForwardNormal						() const;
	bool							IsAlive									() { return !m_isDead; }
	void							RenderDebug								() const;

protected:
	int								m_entityID								= -1;

private:
	SoundID							m_hitSound;
	SoundID							m_dieSound;
};