#include "LightSource.hpp"

#include "Game/GameCommon.hpp"

LightSource::LightSource(Rgba8 const& color, Vec2 const& position, float intensity)
	: m_color(color)
	, m_position(position)
	, m_intensity(intensity)
{
}

void LightVertexArray(int numVertexes, Vertex_PCU* vertexes, Vec2 const& entityCenter, LightSource const& lightSource)
{
	for (int vertexIndex = 0; vertexIndex < numVertexes; vertexIndex++)
	{
		LightVertex(&vertexes[vertexIndex], entityCenter, lightSource);
	}
}

void LightVertex(Vertex_PCU* vertex, Vec2 const& entityCenter, LightSource const& lightSource)
{
	Vec2 vertexPosition = Vec2(vertex->m_position.x, vertex->m_position.y);
	Vec2 dirCenterToVertex = (vertexPosition - entityCenter).GetNormalized();
	Vec2 dirToLight = (lightSource.m_position - entityCenter).GetNormalized();

	float lightStrength = DotProduct2D(dirToLight, dirCenterToVertex);
	lightStrength = GetClamped(lightStrength, 0.1f, 1.f);
	vertex->m_color.MultiplyRGBScaled(lightSource.m_color, lightStrength);
}