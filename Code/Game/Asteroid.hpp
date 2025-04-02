#pragma once

#include "Engine/Core/Vertex_PCU.hpp"

#include "Game/Entity.hpp"

class Game;
class Renderer;

extern Renderer* g_renderer;

class Asteroid : public Entity
{
public:
							~Asteroid					() = default;
							Asteroid					(Game* game, Vec2 const& startPosition, int asteroidID);
	void					InitializeVertexes			();
	void					Update						(float deltaSeconds) override;
	void					CheckOutOfScreenAndWrap		();
	void					Render						() const override;

public:
	static constexpr int NUM_VERTEXES = 48;
	Vertex_PCU m_vertexes[NUM_VERTEXES] = {};
};