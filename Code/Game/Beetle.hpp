#pragma once

#include "Game/Entity.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

class Bullet;

class Beetle : public Entity
{
public:
								~Beetle					() = default;
								Beetle					(Game* game, Vec2 const& startPos, int beetleID);
	void						Update					(float deltaSeconds) override;
	void						Render					() const override;

private:
	void						SpawnExhaustDebris		();

private:
	static constexpr			int						NUM_VERTEXES			= 21;
	static constexpr			int						NUM_TAIL_VERTEXES		= 3;
	static constexpr			float					TAIL_END_MIN			= -10.f;
	static constexpr			float					TAIL_END_MAX			= -8.f;

	float												m_tailLength			= 0.f;

	Vertex_PCU m_vertexes[NUM_VERTEXES] =
	{
		/* Nose */
		Vertex_PCU(Vec3(5.f, 0.f, 0.f),		Rgba8(160, 50, 50, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(3.f, 1.f, 0.f),		Rgba8(160, 50, 50, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(3.f, -1.f, 0.f),	Rgba8(160, 50, 50, 255),	Vec2(0.f, 0.f)),
		/* Body */
		Vertex_PCU(Vec3(3.f, 1.f, 0.f),		Rgba8(200, 200, 200, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-2.f, 1.f, 0.f),	Rgba8(200, 200, 200, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-2.f, -1.f, 0.f),	Rgba8(200, 200, 200, 255),	Vec2(0.f, 0.f)),
		/**/
		Vertex_PCU(Vec3(3.f, 1.f, 0.f),		Rgba8(200, 200, 200, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-2.f, -1.f, 0.f),	Rgba8(200, 200, 200, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(3.f, -1.f, 0.f),	Rgba8(200, 200, 200, 255),	Vec2(0.f, 0.f)),
		/* Left Wing*/
		Vertex_PCU(Vec3(1.f, 1.f, 0.f),		Rgba8(160, 50, 50, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-1.f, 2.f, 0.f),	Rgba8(160, 50, 50, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-1.f, 1.f, 0.f),	Rgba8(160, 50, 50, 255),	Vec2(0.f, 0.f)),
		/* Right Wing */
		Vertex_PCU(Vec3(1.f, -1.f, 0.f),	Rgba8(160, 50, 50, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-1.f, -2.f, 0.f),	Rgba8(160, 50, 50, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-1.f, -1.f, 0.f),	Rgba8(160, 50, 50, 255),	Vec2(0.f, 0.f)),
		/* Back */
		Vertex_PCU(Vec3(-2.f, 0.75f, 0.f),	Rgba8(160, 50, 50, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-3.f, 0.5f, 0.f),	Rgba8(160, 50, 50, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-3.f, -0.5f, 0.f),	Rgba8(160, 50, 50, 255),	Vec2(0.f, 0.f)),
		/**/
		Vertex_PCU(Vec3(-2.f, 0.75f, 0.f),	Rgba8(160, 50, 50, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-3.f, -0.5f, 0.f),	Rgba8(160, 50, 50, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-2.f, -0.75f, 0.f),	Rgba8(160, 50, 50, 255),	Vec2(0.f, 0.f)),
		/**/
	};

	Vertex_PCU m_tailVertexes[NUM_TAIL_VERTEXES] =
	{
		Vertex_PCU(Vec3(-3.f, -0.5f, 0.f),	Rgba8(255, 100, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-3.f, 0.5f, 0.f),	Rgba8(255, 100, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(0.f, 0.f, 0.f),		Rgba8(255, 100, 0, 55),		Vec2(0.f, 0.f)),
		/**/
	};
};