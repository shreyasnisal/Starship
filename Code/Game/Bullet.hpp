#pragma once

#include "Engine/Core/Vertex_PCU.hpp"

#include "Game/Entity.hpp"

class Renderer;

extern Renderer* g_renderer;

class Bullet : public Entity
{

public:
						~Bullet							() = default;
						Bullet							(Game* game, Vec2 const& position, float direction, Rgba8 const& tailColor, int shooterID = 0);
	void				Update							(float deltaSeconds) override;
	void				Render							() const override;
	void				TakeDamage						(int damage) override;
	void				Die								() override;
	void				SpawnDebrisCluster				(int minDebris, int maxDebris, float debrisScale = DEBRIS_SCALE_ON_BULLET_HIT) const override;
	int					GetShooterID					() const { return m_shooterID; };


public:
	static constexpr int NUM_VERTEXES = 6;
	int m_shooterID = 0;

	Vertex_PCU m_vertexes[NUM_VERTEXES] =
	{
		Vertex_PCU(Vec3(1.f, 0.f, 0.f), Rgba8(255, 255, 0, 255), Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(0.f, 1.f, 0.f), Rgba8(255, 255, 0, 255), Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(0.f, -1.f, 0.f), Rgba8(255, 255, 0, 255), Vec2(0.f, 0.f)),
		/**/
		Vertex_PCU(Vec3(0.f, 1.f, 0.f), Rgba8(255, 0, 0, 255), Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(0.f, -1.f, 0.f), Rgba8(255, 0, 0, 255), Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-10.f, 0.f, 0.f), Rgba8(255, 0, 0, 0), Vec2(0.f, 0.f)),
	};

private:
	float m_age = 0;
	Rgba8 m_tailColor;
};