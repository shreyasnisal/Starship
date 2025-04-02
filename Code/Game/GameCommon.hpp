#pragma once

#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"

class App;

extern					App*							g_app;
extern					InputSystem*					g_input;
extern					RandomNumberGenerator*			g_RNG;
extern					Renderer*						g_renderer;
extern					AudioSystem*					g_audio;
extern					Window*							g_window;

constexpr				int								NUM_LEVELS												= 5;
constexpr				int								ASTEROIDS_IN_LEVEL[NUM_LEVELS]							= {3, 5, 10, 15, 20};
constexpr				int								BEETLES_IN_LEVEL[NUM_LEVELS]							= {0, 3, 5, 8, 10};
constexpr				int								WASPS_IN_LEVEL[NUM_LEVELS]								= {2, 3, 5, 8, 10};
constexpr				float							GAMEOVER_DELAY_SECONDS									= 3.f;

constexpr				int								NUM_MERCURY_WAVES										= 3;
constexpr				float							TETHER_DISTANCE_MERCURY									= 150.f;
constexpr				int								WASPS_IN_MERCURY_WAVE[NUM_MERCURY_WAVES]				= {3, 5, 10};

constexpr				int								NUM_VENUS_WAVES											= 5;
constexpr				float							TETHER_DISTANCE_VENUS									= 200.f;
constexpr				int								WASPS_IN_VENUS_WAVE[NUM_VENUS_WAVES]					= {5, 10, 12};
constexpr				int								MISSILE_SHOOTERS_IN_VENUS_WAVE[NUM_VENUS_WAVES]			= {1, 2, 3};
constexpr				int								DUAL_SHOOTERS_IN_VENUS_WAVE[NUM_VENUS_WAVES]			= {2, 3, 5};

constexpr				int								NUM_MARS_WAVES											= 10;
constexpr				float							TETHER_DISTANCE_MARS									= 250.f;
constexpr				int								WASPS_IN_MARS_WAVE[NUM_MARS_WAVES]						= {3, 3, 5, 5, 10, 10, 10, 12, 12, 15};
constexpr				int								MISSILE_SHOOTERS_IN_MARS_WAVE[NUM_MARS_WAVES]			= {3, 3, 5, 6, 8, 10, 10, 12, 12, 15};
constexpr				int								DUAL_SHOOTERS_IN_MARS_WAVE[NUM_MARS_WAVES]				= {2, 2, 4, 4, 6, 6, 8, 8, 10, 10};
constexpr				int								MULTI_SHOOTERS_IN_MARS_WAVE[NUM_MARS_WAVES]				= {0, 1, 2, 3, 4, 5, 5, 6, 6, 7};

constexpr				int								PLAYERSHIP_MAX_BULLETS									= 500;
constexpr				float							PLAYERSHIP_THRUST										= 100.f;
constexpr				float							PLAYERSHIP_ANGULAR_SPEED								= 800.f;
constexpr				float							PLAYERSHIP_MAX_SPEED									= 100.f;
constexpr				float							PLAYERSHIP_PHYSICS_RADIUS								= 3.5f;
constexpr				float							PLAYERSHIP_COSMETIC_RADIUS								= 4.f;
constexpr				int								PLAYERSHIP_MAX_HEALTH									= 100;
constexpr				int								PLAYERSHIP_LIVES										= 0;
const					Rgba8							PLAYERSHIP_TAIL_COLOR									= Rgba8(255, 100, 0, 255);
const					Rgba8							PLAYERSHIP_COLOR										= Rgba8(102, 153, 204, 255);
constexpr				int								PLAYERSHIP_NUM_VERTEXES									= 39;
constexpr				int								PLAYERSHIP_NUM_TAIL_VERTEXES							= 3;

//const Vertex_PCU PLAYERSHIP_VERTEXES[PLAYERSHIP_NUM_VERTEXES] =
//{
//	Vertex_PCU(Vec3(4.f, 0.f, 0.f),		PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
//	Vertex_PCU(Vec3(0.f, 2.f, 0.f),		PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
//	Vertex_PCU(Vec3(0.f, -2.f, 0.f),	PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
//	/**/
//	Vertex_PCU(Vec3(0.f, 2.f, 0.f),		PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
//	Vertex_PCU(Vec3(-2.f, 1.f, 0.f),	PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
//	Vertex_PCU(Vec3(-2.f, -1.f, 0.f),	PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
//	/**/
//	Vertex_PCU(Vec3(0.f, 2.f, 0.f),		PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
//	Vertex_PCU(Vec3(-2.f, -1.f, 0.f),	PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
//	Vertex_PCU(Vec3(0.f, -2.f, 0.f),	PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
//	/**/
//	/* Window */
//	Vertex_PCU(Vec3(3.f, 0.f, 0.f),		Rgba8(155, 155, 155, 255),		Vec2(0.f, 0.f)),
//	Vertex_PCU(Vec3(1.f, 1.f, 0.f),		Rgba8(20, 20, 20, 255),			Vec2(0.f, 0.f)),
//	Vertex_PCU(Vec3(1.f, -1.f, 0.f),	Rgba8(20, 20, 20, 255),			Vec2(0.f, 0.f)),
//	/**/
//	/* Left Wing */
//	Vertex_PCU(Vec3(2.f, 1.f, 0.f),		Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
//	Vertex_PCU(Vec3(-1.f, 4.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
//	Vertex_PCU(Vec3(0.f, 2.f, 0.f),		Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
//	/**/								
//	Vertex_PCU(Vec3(0.f, 2.f, 0.f),		Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
//	Vertex_PCU(Vec3(-1.f, 4.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
//	Vertex_PCU(Vec3(-2.f, 3.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
//	/**/								
//	Vertex_PCU(Vec3(0.f, 2.f, 0.f),		Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
//	Vertex_PCU(Vec3(-2.f, 3.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
//	Vertex_PCU(Vec3(-1.f, 1.5f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
//	/**/								
//	/* Right Wing */					
//	Vertex_PCU(Vec3(2.f, -1.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
//	Vertex_PCU(Vec3(-1.f, -4.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
//	Vertex_PCU(Vec3(0.f, -2.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
//	/**/								
//	Vertex_PCU(Vec3(0.f, -2.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
//	Vertex_PCU(Vec3(-1.f, -4.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
//	Vertex_PCU(Vec3(-2.f, -3.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
//	/**/								
//	Vertex_PCU(Vec3(0.f, -2.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
//	Vertex_PCU(Vec3(-2.f, -3.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
//	Vertex_PCU(Vec3(-1.f, -1.5f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
//};

const Vertex_PCU PLAYERSHIP_VERTEXES[PLAYERSHIP_NUM_VERTEXES] =
{
	Vertex_PCU(Vec3(4.f, 2.f, 0.f),		PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(0.f, 2.f, 0.f),		PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(0.f, -2.f, 0.f),	PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
	/**/
	Vertex_PCU(Vec3(4.f, 2.f, 0.f),		PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(0.f, -2.f, 0.f),	PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(4.f, -2.f, 0.f),	PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
	/**/
	Vertex_PCU(Vec3(4.f, 2.f, 0.f),		PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(4.f, -2.f, 0.f),	PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(5.f, 0.f, 0.f),		PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
	/**/
	/* Window */
	Vertex_PCU(Vec3(3.5f, 1.f, 0.f),	Rgba8(155, 155, 155, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(2.5f, 1.f, 0.f),	Rgba8(20, 20, 20, 255),			Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(2.5f, -1.f, 0.f),	Rgba8(20, 20, 20, 255),			Vec2(0.f, 0.f)),
	/**/
	Vertex_PCU(Vec3(3.5f, 1.f, 0.f),	Rgba8(155, 155, 155, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(2.5f, -1.f, 0.f),	Rgba8(20, 20, 20, 255),			Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(3.5f, -1.f, 0.f),	Rgba8(20, 20, 20, 255),			Vec2(0.f, 0.f)),
	/* Left Wing */
	Vertex_PCU(Vec3(2.f, 2.f, 0.f),		Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(2.f, 3.f, 0.f),		Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(1.f, 3.f, 0.f),		Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	/**/
	Vertex_PCU(Vec3(2.f, 2.f, 0.f),		Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(1.f, 3.f, 0.f),		Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(1.f, 2.f, 0.f),		Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	/**/
	Vertex_PCU(Vec3(4.f, 3.f, 0.f),		Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(4.f, 4.f, 0.f),		Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(0.f, 4.f, 0.f),		Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	/**/								
	Vertex_PCU(Vec3(4.f, 3.f, 0.f),		Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(0.f, 4.f, 0.f),		Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(0.f, 3.f, 0.f),		Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	/**/								
	/* Right Wing */					
	Vertex_PCU(Vec3(2.f, -2.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(2.f, -3.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(1.f, -3.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	/**/
	Vertex_PCU(Vec3(2.f, -2.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(1.f, -3.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(1.f, -2.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	/**/
	Vertex_PCU(Vec3(4.f, -3.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(4.f, -4.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(0.f, -4.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	/**/
	Vertex_PCU(Vec3(4.f, -3.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(0.f, -4.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(0.f, -3.f, 0.f),	Rgba8(200, 200, 200, 255),		Vec2(0.f, 0.f)),
};

const Vertex_PCU PLAYERSHIP_LAND_VERTEXES[PLAYERSHIP_NUM_VERTEXES] =
{
	Vertex_PCU(Vec3(5.f, 2.f, 0.f),		PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(0.f, 2.f, 0.f),		PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(0.f, -2.f, 0.f),	PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
	/**/
	Vertex_PCU(Vec3(5.f, 2.f, 0.f),		PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(0.f, -2.f, 0.f),	PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(5.f, -2.f, 0.f),	PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
	/**/
	Vertex_PCU(Vec3(4.f, 2.f, 0.f),		PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(4.f, -2.f, 0.f),	PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(5.f, 0.f, 0.f),		PLAYERSHIP_COLOR,				Vec2(0.f, 0.f)),
	/**/
	/* Window */
	Vertex_PCU(Vec3(3.5f, 1.f, 0.f),	Rgba8(155, 155, 155, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(2.5f, 1.f, 0.f),	Rgba8(20, 20, 20, 255),			Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(2.5f, -1.f, 0.f),	Rgba8(20, 20, 20, 255),			Vec2(0.f, 0.f)),
	/**/
	Vertex_PCU(Vec3(3.5f, 1.f, 0.f),	Rgba8(155, 155, 155, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(2.5f, -1.f, 0.f),	Rgba8(20, 20, 20, 255),			Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(3.5f, -1.f, 0.f),	Rgba8(20, 20, 20, 255),			Vec2(0.f, 0.f)),
	/* Left Wing */
	Vertex_PCU(Vec3(2.f, 2.f, 0.f),		Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(2.f, 2.f, 0.f),		Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(1.f, 2.f, 0.f),		Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	/**/
	Vertex_PCU(Vec3(2.f, 2.f, 0.f),		Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(1.f, 2.f, 0.f),		Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(1.f, 2.f, 0.f),		Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	/**/
	Vertex_PCU(Vec3(4.f, 2.f, 0.f),		Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(4.f, 3.f, 0.f),		Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(0.f, 3.f, 0.f),		Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	/**/
	Vertex_PCU(Vec3(4.f, 2.f, 0.f),		Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(0.f, 3.f, 0.f),		Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(0.f, 2.f, 0.f),		Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	/**/
	/* Right Wing */
	Vertex_PCU(Vec3(2.f, -2.f, 0.f),	Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(2.f, -2.f, 0.f),	Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(1.f, -2.f, 0.f),	Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	/**/
	Vertex_PCU(Vec3(2.f, -2.f, 0.f),	Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(1.f, -2.f, 0.f),	Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(1.f, -2.f, 0.f),	Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	/**/
	Vertex_PCU(Vec3(4.f, -2.f, 0.f),	Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(4.f, -3.f, 0.f),	Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(0.f, -3.f, 0.f),	Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	/**/
	Vertex_PCU(Vec3(4.f, -2.f, 0.f),	Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(0.f, -3.f, 0.f),	Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(0.f, -2.f, 0.f),	Rgba8(0, 0, 0, 255),		Vec2(0.f, 0.f)),
};

const Vertex_PCU PLAYERSHIP_TAIL_VERTEXES[PLAYERSHIP_NUM_TAIL_VERTEXES] =
{
	Vertex_PCU(Vec3(-2.f, -1.f, 0.f),	PLAYERSHIP_TAIL_COLOR,		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(-2.f, 1.f, 0.f),	PLAYERSHIP_TAIL_COLOR,		Vec2(0.f, 0.f)),
	Vertex_PCU(Vec3(-2.f, 0.f, 0.f),	Rgba8(255, 100, 0, 127),	Vec2(0.f, 0.f))
};


constexpr				int								MAX_BULLETS												= 100;
constexpr				float							BULLET_PHYSICS_RADIUS									= 0.5f;
constexpr				float							BULLET_COSMETIC_RADIUS									= 2.f;
constexpr				float							BULLET_SPEED											= 200.f;
constexpr				float							BULLET_LIFESPAN											= 2.f;

constexpr				int								MAX_ASTEROIDS											= 60;
constexpr				int								NUM_INITIAL_ASTEROIDS									= 6;
constexpr				float							ASTEROID_PHYSICS_RADIUS									= 3.f;
constexpr				float							ASTEROID_COSMETIC_RADIUS								= 4.f;
constexpr				float							ASTEROID_SPEED											= 10.f;
constexpr				int								ASTEROID_HEALTH											= 3;
constexpr				int								ASTEROID_ID_OFFSET										= 100;

constexpr				int								MAX_BEETLES												= 20;
constexpr				float							BEETLE_MIN_SPEED										= 40.f;
constexpr				float							BEETLE_MAX_SPEED										= 50.f;
constexpr				float							BEETLE_PHYSICS_RADIUS									= 4.5f;
constexpr				float							BEETLE_COSMETIC_RADIUS									= 5.f;
constexpr				float							BEETLE_ANGULAR_VELOCITY									= 50.f;
constexpr				int								BEETLE_ID_OFFSET										= 200;

constexpr				int								MAX_WASPS												= 25;
constexpr				float							WASP_MAX_SPEED											= 200.f;
constexpr				float							WASP_THRUST												= 40.f;
constexpr				float							WASP_PHYSICS_RADIUS										= 5.f;
constexpr				float							WASP_COSMETIC_RADIUS									= 4.5f;
constexpr				int								WASP_ID_OFFSET											= 300;
constexpr				float							WASP_SHOOT_DELAY										= 0.5f;

constexpr				int								MAX_DUAL_SHOOTERS										= 20;
constexpr				float							DUAL_SHOOTER_MAX_SPEED									= 200.f;
constexpr				float							DUAL_SHOOTER_THRUST										= 40.f;
constexpr				float							DUAL_SHOOTER_PHYSICS_RADIUS								= 6.f;
constexpr				float							DUAL_SHOOTER_COSMETIC_RADIUS							= 6.5f;
constexpr				int								DUAL_SHOOTER_ID_OFFSET									= 400;
constexpr				float							DUAL_SHOOTER_SHOOT_DELAY								= 0.5f;

constexpr				int								MAX_DEBRIS												= 1000;
constexpr				float							DEBRIS_PHYSICS_RADIUS									= 0.5f;
constexpr				float							DEBRIS_COSMETIC_RADIUS									= 2.f;
constexpr				float							DEBRIS_ANGULAR_SPEED									= 100.f;
constexpr				float							DEBRIS_SPAWN_RANDOM_SPEED_LIMITS						= 100.f;
constexpr				float							DEBRIS_DEFAULT_LIFESPAN									= 0.5f;

constexpr				int								MIN_DEBRIS_PER_ENTITY									= 10;
constexpr				int								MAX_DEBRIS_PER_ENTITY									= 20;
constexpr				int								MIN_DEBRIS_ON_BULLET_HIT								= 1;
constexpr				int								MAX_DEBRIS_ON_BULLET_HIT								= 3;
constexpr				float							DEBRIS_SCALE_ON_BULLET_HIT								= 0.2f;
constexpr				int								MIN_DEBRIS_PER_PLAYERSHIP								= 20;
constexpr				int								MAX_DEBRIS_PER_PLAYERSHIP								= 40;

constexpr				float							AI_SHOOT_DELAY											= 0.1f;
constexpr				float							AI_SHOOT_MIN_ANGE_DEGREES								= 2.f;
constexpr				float							AI_RESPAWN_DELAY										= 5.f;
constexpr				float							AI_GAME_RESTART_DELAY									= 10.f;
constexpr				float							AI_ENEMY_DISTANCE_FOR_RUN								= 20.f;
constexpr				float							AI_MIN_AWAY_FROM_ENEMY_ROTATION_DEGREES_FOR_RUN			= 45.f;
constexpr				float							AI_MAX_SPEED											= 50.f;
constexpr				float							AI_WASP_DISTANCE_ADVNATAGE								= 20.f;

constexpr				float							MINIMAP_CAMERA_SCALE									= 4.f;
constexpr				float							MAX_CAMERA_SHAKE_X										= 8.f;
constexpr				float							MAX_CAMERA_SHAKE_Y										= 4.f;

constexpr				int								MIN_STARS_IN_BACKGROUND									= 500;
constexpr				int								MAX_STARS_IN_BACKGROUND									= 5000;
constexpr				float							STAR_RADIUS												= 0.2f;
const					Rgba8							STAR_COLOR												= Rgba8(255, 255, 255, 255);
constexpr				unsigned char					MIN_STAR_OPACITY										= 55;
constexpr				unsigned char					MAX_STAR_OPACITY										= 255;

constexpr				int								NUM_PLANETS												= 8;

enum PlanetName
{
	PLANET_INVALID = -1,

	PLANET_MERCURY,
	PLANET_VENUS,
	PLANET_EARTH,
	PLANET_MARS,
	PLANET_JUPITER,
	PLANET_SATURN,
	PLANET_URANUS,
	PLANET_NEPTUNE,

	PLANET_NUM
};

constexpr int			MAX_PICKUPS = 10;

enum class PickupType
{
	HEALTH,
	AMMO
};

void					DebugDrawRing					(Vec2 const& center, float radius, float thickness, Rgba8 const& color);
void					DebugDrawLine					(Vec2 const& pointA, Vec2 const& pointB, float thickness, Rgba8 const& color);
void					DebugDrawDisc					(Vec2 const& center, float radius, Rgba8 const& color);
void					DebugDrawGlow					(Vec2 const& center, float radius, Rgba8 const& centerColor, Rgba8 const& edgeColor);
void					DrawBoxOutline					(AABB2 const& boundedBox, float thickness, Rgba8 const& color);
