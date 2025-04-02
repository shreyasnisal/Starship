#pragma once

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"

struct Vertex_PCU;

struct LightSource
{
	Rgba8 m_color;
	Vec2 m_position;
	float m_intensity = 0.f;

public:
	~LightSource() = default;
	LightSource() = default;
	LightSource(Rgba8 const& color, Vec2 const& position, float intensity);
};

void LightVertexArray(int numVertexes, Vertex_PCU* vertexes, Vec2 const& entityCenter, LightSource const& lightSource);
void LightVertex(Vertex_PCU* vertex, Vec2 const& entityCenter, LightSource const& lightSource);