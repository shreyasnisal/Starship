#pragma once

#include "Game/Entity.hpp"

class Game;

class Pickup : public Entity
{
public:
	~Pickup() = default;
	Pickup(Game* game, Vec2 const& position, PickupType type);
	virtual void				Update(float deltaSeconds) override;
	virtual void				Render() const override;
	
public:
	PickupType					m_type;
};