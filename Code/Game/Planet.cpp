#include "Game/Planet.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Beetle.hpp"
#include "Game/Wasp.hpp"

Planet::Planet(Game* game, Vec2 const& startPosition, PlanetName name) : Entity(game, startPosition)
{
	m_entityType = EntityType::PLANET;
	m_cosmeticRadius = PLANET_RADII[(int)name];
	m_physicsRadius = PLANET_RADII[(int)name] * 3.f; // interaction radius for the planet - player will be able to land/start quest when within this radius
	m_color = PLANET_COLOR[(int)name];
	m_name = name;
	m_revolutionAngularVelocity = PLANET_REVOLUTION_ANGULAR_VELOCITIES[(int)name];
	m_baseTexture = g_renderer->CreateOrGetTextureFromFile(Stringf("Data/Images/planet0%d.png", (int)name).c_str());
}

void Planet::Update(float deltaSeconds)
{
	m_position.RotateDegrees(m_revolutionAngularVelocity * deltaSeconds);
}

void Planet::Render() const
{
	if (IsOffScreen())
	{
		return;
	}

	std::vector<Vertex_PCU> planetVerts;
	AddVertsForDisc2D(planetVerts, Vec2::ZERO, m_cosmeticRadius, Rgba8::WHITE, m_textureUVAtMins, Vec2::ONE);
	TransformVertexArrayXY3D(planetVerts, 1.f, 0.f, m_position);
	LightVertexArray(static_cast<int>(planetVerts.size()), planetVerts.data(), m_position, m_game->m_sunLightSource);
	g_renderer->BindTexture(m_baseTexture);
	g_renderer->DrawVertexArray(planetVerts);

	if (m_game->m_drawDebug)
	{
		RenderDebug();
	}
}