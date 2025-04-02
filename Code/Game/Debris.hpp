#pragma once

#include "Game/Entity.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

class Debris : public Entity
{
public:
	static constexpr int NUM_VERTEXES = 12;
	Vertex_PCU m_vertexes[NUM_VERTEXES] = {};
	unsigned char m_opacity = 127;
	float m_scale = 1.f;
	float m_lifespan = 2.f;

public:
	~Debris() = default;
	Debris(Game* game, Vec2 const& startPosition, Vec2 const& startVelocity, Rgba8 const& color, float scale = 1.f, float lifespan = DEBRIS_DEFAULT_LIFESPAN);
	void Update(float deltaSeconds) override;
	void Render() const override;
	void TakeDamage(int damage) override;
};