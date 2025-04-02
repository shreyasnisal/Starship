#pragma once

#include "Engine/Math/Vec2.hpp"

#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"

class Game;

class Planet : public Entity
{
public:
							~Planet								() = default;
							Planet								(Game* game, Vec2 const& startPosition, PlanetName name);
	void					Update								(float deltaSeconds) override;
	void					Render								() const override;

	PlanetName				GetPlanetName						() const { return m_name; }

public:
	Vec2							m_textureUVAtMins = Vec2::ZERO;

private:

	//void					ApplyGravityOnEntityList			(int numEntities, Entity** entities, float deltaSeconds);

private:

	static constexpr float			PLANET_RADII[PLANET_NUM] = { 15.f, 40.f, 40.f, 20.f, 80.f, 70.f, 75.f, 75.f };
	const Rgba8						PLANET_COLOR[PLANET_NUM] =
	{
		Rgba8(100, 100, 100, 255),
		Rgba8(200, 100, 20, 255),
		Rgba8(0, 150, 200, 255),
		Rgba8(200, 50, 50, 255),
		Rgba8(240, 220, 180, 255),
		Rgba8(250, 240, 190, 255),
		Rgba8(150, 220, 235, 255),
		Rgba8(60, 70, 200, 255),
	};

	const float						PLANET_REVOLUTION_ANGULAR_VELOCITIES[PLANET_NUM] =
	{
		0.f
	};

	const float						PLANET_GRAVITYSCALE[PLANET_NUM] =
	{
		3.7f, 9.f, 10.f, 3.8f, 25.f, 10.f, 8.9f, 11.f
	};

	PlanetName						m_name = PLANET_INVALID;
	float							m_gravityScale = 0.f;
	float							m_orientationDegreesRelativeToSun = 0.f;
	float							m_revolutionAngularVelocity = 0.f;
	float							m_distanceFromSun = 0.f;

	Texture*						m_baseTexture = nullptr;
};