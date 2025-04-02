#pragma once

#pragma once

#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"

class Bullet;

class DualShooter : public Entity
{

public:
	~DualShooter() = default;
	DualShooter(Game* game, Vec2 const& startPosition, int dualShooterID);
	void					Update(float deltaSeconds) override;
	void					Render() const override;

private:
	void					SpawnExhaustDebris();
	void					SpawnSmoke();

private:
	static constexpr			int						NUM_VERTEXES = 48;
	static constexpr			int						NUM_TAIL_VERTEXES = 6;
	static constexpr			float					TAIL_END_MIN = -7.f;
	static constexpr			float					TAIL_END_MAX = -3.f;

	float												m_tailLength = 0.f;

	Vertex_PCU m_vertexes[NUM_VERTEXES] =
	{
		/* Body */
		Vertex_PCU(Vec3(5.f, 1.f, 0.f),			Rgba8(200, 200, 200, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(0.f, 3.f, 0.f),			Rgba8(200, 200, 200, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(0.f, -3.f, 0.f),		Rgba8(200, 200, 200, 255),	Vec2(0.f, 0.f)),
		/**/
		Vertex_PCU(Vec3(5.f, 1.f, 0.f),			Rgba8(200, 200, 200, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(0.f, -3.f, 0.f),		Rgba8(200, 200, 200, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(5.f, -1.f, 0.f),		Rgba8(200, 200, 200, 255),	Vec2(0.f, 0.f)),
		/**/
		Vertex_PCU(Vec3(0.f, 3.f, 0.f),			Rgba8(200, 200, 200, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-1.f, 2.f, 0.f),		Rgba8(200, 200, 200, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-1.f, -2.f, 0.f),		Rgba8(200, 200, 200, 255),	Vec2(0.f, 0.f)),
		/**/
		Vertex_PCU(Vec3(0.f, 3.f, 0.f),			Rgba8(200, 200, 200, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-1.f, -2.f, 0.f),		Rgba8(200, 200, 200, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(0.f, -3.f, 0.f),		Rgba8(200, 200, 200, 255),	Vec2(0.f, 0.f)),
		/* Window */
		Vertex_PCU(Vec3(4.f, 0.5f, 0.f),		Rgba8(155, 155, 155, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(2.f, 1.5f, 0.f),		Rgba8(20, 20, 20, 255),		Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(2.f, -1.5f, 0.f),		Rgba8(20, 20, 20, 255),		Vec2(0.f, 0.f)),
		/**/
		Vertex_PCU(Vec3(4.f, 0.5f, 0.f),		Rgba8(155, 155, 155, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(2.f, -1.5f, 0.f),		Rgba8(20, 20, 20, 255),		Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(4.f, -0.5f, 0.f),		Rgba8(155, 155, 155, 255),	Vec2(0.f, 0.f)),
		/* Left Wing */
		Vertex_PCU(Vec3(1.5f, 2.5f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(1.5f, 3.5f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(0.f, 3.f, 0.f),			Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		/**/
		Vertex_PCU(Vec3(0.f, 3.f, 0.f),			Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(1.5f, 3.5f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(0.f, 4.f, 0.f),			Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		/**/
		Vertex_PCU(Vec3(0.f, 4.f, 0.f),			Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(1.5f, 3.5f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(5.f, 6.f, 0.f),			Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		/* Right Wing */
		Vertex_PCU(Vec3(1.5f, -2.5f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(1.5f, -3.5f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(0.f, -3.f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		/**/
		Vertex_PCU(Vec3(0.f, -3.f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(1.5f, -3.5f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(0.f, -4.f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		/**/
		Vertex_PCU(Vec3(0.f, -4.f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(1.5f, -3.5f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(5.f, -6.f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		/* Back*/
		Vertex_PCU(Vec3(-1.f, 2.f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-2.f, 2.f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-2.f, 1.f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		/**/
		Vertex_PCU(Vec3(-1.f, 2.f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-2.f, 1.f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-1.f, 1.f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		/**/
		Vertex_PCU(Vec3(-1.f, -2.f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-2.f, -2.f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-2.f, -1.f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		/**/
		Vertex_PCU(Vec3(-1.f, -2.f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-2.f, -1.f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-1.f, -1.f, 0.f),		Rgba8(255, 255, 0, 255),	Vec2(0.f, 0.f)),
	};

	Vertex_PCU m_tailVertexes[NUM_TAIL_VERTEXES] =
	{
		Vertex_PCU(Vec3(-2.f, 2.f, 0.f),	Rgba8(255, 100, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-2.f, 1.f, 0.f),	Rgba8(255, 100, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(0.f, 1.5f, 0.f),		Rgba8(255, 100, 0, 55),		Vec2(0.f, 0.f)),
		/**/
		Vertex_PCU(Vec3(-2.f, -1.f, 0.f),	Rgba8(255, 100, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-2.f, -2.f, 0.f),	Rgba8(255, 100, 0, 255),	Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(0.f, -1.5f, 0.f),		Rgba8(255, 100, 0, 55),		Vec2(0.f, 0.f)),
	};

private:
	void									FireBulletsTowardsPlayer(float deltaSeconds);

private:
	float									m_shootDelay = DUAL_SHOOTER_SHOOT_DELAY;
};