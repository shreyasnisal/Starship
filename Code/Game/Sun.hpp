#pragma once

#include "Engine/Math/Vec2.hpp"

#include "Game/Entity.hpp"

class Game;

class Sun : public Entity
{
public:
							~Sun								() = default;
							Sun									(Game* game, Vec2 const& startPos);
	void					Update								(float deltaSeconds) override;
	void					Render								() const override;

public:


private:


private:
	float							m_gravityScale = 0.f;
};