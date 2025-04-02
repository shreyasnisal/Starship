#include "Game/Pickup.hpp"

#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

Pickup::Pickup(Game* game, Vec2 const& position, PickupType type)
	: Entity(game, position)
	, m_type(type)
{
	m_cosmeticRadius = 0.4f;
	m_physicsRadius = 0.1f;
}

void Pickup::Update(float)
{
}

void Pickup::Render() const
{
	Rgba8 glowColor = Rgba8::MAGENTA;

	switch (m_type)
	{
		case PickupType::HEALTH:
		{
			glowColor = Rgba8::GREEN;
			break;
		}
		case PickupType::AMMO:
		{
			glowColor = Rgba8::YELLOW;
			break;
		}
	}

	DebugDrawDisc(m_position, m_physicsRadius, glowColor);
	DebugDrawGlow(m_position, m_cosmeticRadius, Rgba8(glowColor.r, glowColor.g, glowColor.b, 127), Rgba8(glowColor.r, glowColor.g, glowColor.b, 0));

}
