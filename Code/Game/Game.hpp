#pragma once

#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Game/GameCommon.hpp"
#include "Game/LightSource.hpp"
#include "Game/QuestManager.hpp"


class		App;
class		Asteroid;
class		Beetle;
class		Bullet;
class		Debris;
class		DualShooter;
class		Entity;
class		PlayerShip;
class		Wasp;

class Game
{
public:
	~Game			();
	Game			();
	void			Update										();
	void			Render										() const;
	
	Bullet*			SpawnNewBullet								(Vec2 const& bulletPosition, float bulletDirection, Rgba8 const& tailColor, int shooterID);
	Asteroid*		SpawnNewAsteroid							(Vec2 const& asteroidPosition);
	Beetle*			SpawnNewBeetle								(Vec2 const& beetlePosition);
	Wasp*			SpawnNewWasp								(Vec2 const& waspPosition);
	DualShooter*	SpawnNewDualShooter							(Vec2 const& dualShooterPosition);
	void			SpawnDebris									(Vec2 const& debrisPosition,
																 Vec2 const& debrisVelocity,
																 Rgba8 const& debrisColor,
																 float debrisScale = 1.f,
																 float debrisLifespan = DEBRIS_DEFAULT_LIFESPAN);
	void			SpawnPickups								(Vec2 const& planetCenter, float planetRadius);
	void			SpawnPickup									(Vec2 const& pickupPosition, PickupType type);

	bool			IsEntityAlive								(Entity* entity) const;
	void			StartGame									();
	void			ShowMenu									();
	void			ShakeCamera									(float trauma);

	void			SetTetherPlanet								(PlanetName tetherPlanet);
	void			StartQuest									();
	void			LandOnPlanet								();
	void			TakeOffFromPlanet							();


	Vec2 const		GetRandomPointOffScreenByOffset				(float offset) const;

	static bool		Event_GameClock										(EventArgs& args);


public:
	static constexpr float		SCREEN_X															= 1920.f;
	static constexpr float		SCREEN_Y															= 1200.f;
	static constexpr float		WORLD_X																= 240.f;
	static constexpr float		WORLD_Y																= 150.f;

	static constexpr float		LIVES_X_OFFSET														= 6.f * SCREEN_X / WORLD_X;
	static constexpr float		LIVES_Y_OFFSET														= 8.f * SCREEN_X / WORLD_X;
	static constexpr float		LIVES_STRIDE														= 10.f * SCREEN_X / WORLD_X;
	static constexpr float		LIVES_SCALE															= SCREEN_X / WORLD_X;

	static constexpr float		ATTRACT_SCREEN_PLAY_BUTTON_SCALE									= 10.f * SCREEN_X / WORLD_X;
	static constexpr float		ATTRACT_SCREEN_PLAYERSHIPS_SCALE									= 5.f * SCREEN_X / WORLD_X;
	
	static constexpr float		ATTRACT_SCREEN_SHIP_DEFAULT_OFFSET									= 75.f * SCREEN_X / WORLD_X;
	static constexpr float		ATTRACT_SCREEN_SHIP_MIN_OFFSET										= 50.f * SCREEN_X / WORLD_X;
	static constexpr float		ATTRACT_SCREEN_SHIP_MAX_OFFSET										= 100.f * SCREEN_X / WORLD_X;
	static constexpr float		ATTRACT_SCREEN_SHIP_SPEED											= 200.f;
	
	static constexpr int		ATTRACT_SCREEN_PLAY_BUTTON_VERTEXES									= 3;

	Vertex_PCU					m_playButtonVerts[ATTRACT_SCREEN_PLAY_BUTTON_VERTEXES]				= {
																										Vertex_PCU(Vec3(1.f, 0.f, 0.f), Rgba8(45, 150, 75, 255), Vec2(0.f, 0.f)),
																										Vertex_PCU(Vec3(-1.f, 1.f, 0.f), Rgba8(45, 150, 75, 255), Vec2(0.f, 0.f)),
																										Vertex_PCU(Vec3(-1.f, -1.f, 0.f), Rgba8(45, 150, 75, 255), Vec2(0.f, 0.f))
																									   };

	PlayerShip*					m_playerShip														= nullptr;
	Entity*						m_bullets[MAX_BULLETS]												= {};
	Entity*						m_enemyBullets[MAX_BULLETS]											= {};
	Entity*						m_asteroids[MAX_ASTEROIDS]											= {};
	Entity*						m_beetles[MAX_BEETLES]												= {};
	Entity*						m_wasps[MAX_WASPS]													= {};
	Entity*						m_dualShooters[MAX_DUAL_SHOOTERS]									= {};
	Entity*						m_debris[MAX_DEBRIS]												= {};
	Entity*						m_pickups[MAX_PICKUPS]												= {};
	Entity*						m_celestialObjects[NUM_PLANETS + 1];

	int							m_numSpawnedAsteroids												= 0;
	int							m_numSpawnedBeetles													= 0;
	int							m_numSpawnedWasps													= 0;
	int							m_numSpawnedDualShooters											= 0;
	int							m_numExtraLivesRemaining											= PLAYERSHIP_LIVES;
	int							m_currentLevel														= 0;
	float						m_gameoverDelay														= GAMEOVER_DELAY_SECONDS;

	unsigned char				m_attractScreenPlayButtonOpacity									= 255;
	bool						m_isPaused															= false;
	bool						m_pauseAfterUpdate													= false;
	bool						m_drawDebug															= false;
	bool						m_isShowingMenu														= false;
	bool						m_isShowingAttractScreen											= true;
	bool						m_attractScreenPlayButtonOpacityIsIncreasing						= false;
	bool						m_isShowingXboxControllerVisualTest									= false;
	float						m_attractScreenShipDistanceFromScreenCenter							= ATTRACT_SCREEN_SHIP_DEFAULT_OFFSET;
	bool						m_isAttractScreenShipMovingTowardsCenter							= true;

	static constexpr int		NUM_MENU_ITEMS														= 3;
	std::string					m_menuList[NUM_MENU_ITEMS]											= {"Continue Game", "New Game", "Exit"};
	int							m_selectedMenuItem													= 0;
	bool						m_isSavedGame														= false;

	bool						m_isAIEnabled														= false;
	float						m_aiStartDelay														= AI_GAME_RESTART_DELAY;
	
	Camera						m_worldCamera;
	LightSource					m_sunLightSource;

	Vec2 const					SUN_POSITION														= Vec2(0.f, 0.f);
	float						SUN_RADIUS															= 100.f;

	QuestManager				m_questManager;
	bool						m_isLandingOnPlanet = false;
	bool						m_isTakingOffFromPlanet = false;

	Clock						m_gameClock = Clock();

private:
	void					GenerateBackgroundStarPositions							(bool canDrawOnScreen = false);
	void					GenerateCelestialObjects								();

	void					UpdateAttractScreen										(float deltaSeconds);
	void					UpdateMenu												(float deltaSeconds);
	void					HandleDeveloperCheats									();
	void					UpdateEntities											(float deltaSeconds);
	void					UpdateEntitiesInList									(int numEntities, Entity** entities, float deltaSeconds);

	void					UpdateXboxControllerVisualTest							(float deltaSeconds);
	
	void					UpdateCameras											(float deltaSeconds);

	void					CheckCollisionsBulletsVsEnemies							();
	void					CheckCollisionsBulletVsEntityList						(Entity* bullet, int numEntities, Entity** entities);
	void					CheckCollisionsPlayerShipVsEnemies						();
	bool					CheckCollisionsPlayerShipVsEntityList					(PlayerShip* playerShip, int numEntitites, Entity** entities);
	void					CheckCollisionsEnemiesVsAsteroids							();
	void					CheckCollisionsEntityListVsEntityList					(int numEntitiesInList1, Entity** entityList1, int numEntitiesInList2, Entity** entityList2);
	void					CheckCollisionsPlayerShipVsPlanets						();
	void					CheckCollisionsBulletsVsPlanets							();
	void					CheckCollisionsEnemiesVsPlanets							();
	void					CheckCollisionsPlayerShipVsPickups						();

	void					RenderAttractScreen										() const;
	void					RenderMenu												() const;
	void					RenderLives												() const;
	void					RenderEntities											() const;
	void					RenderEntitiesInList									(int numEntities, Entity* const* entities) const;
	void					RenderXboxControllerVisualTest							() const;
	void					RenderAnalogJoystick									(AnalogJoystick const& joystick, Vec2 const& center) const;
	void					RenderXboxButtons										() const;
	void					RenderXboxButton										(int buttonIndex, Vec2 const& center) const;
	void					RenderXboxTrigger										(float triggerValue, Vec2 const& startPosition, float maxLength) const;
	void					RenderCameras											() const;
	void					RenderBackground										() const;
	void					RenderRadar												() const;
	void					RenderHUD												() const;
	void					RenderBulletsRemaining									() const;
	void					RenderEntitiesOnRadar									(Vec2 const& radarPosition, float radarRadius) const;
	void					RenderAsteroidsOnRadar									(Vec2 const& radarPosition, float radarRadius) const;
	void					RenderPlanetsOnRadar									(Vec2 const& radarPosition, float radarRadius) const;
	void					RenderEnemiesOnRadar									(int numEnemies, Entity* const* enemies, Vec2 const& radarPosition, float radarRadius) const;
	void					RenderCelestialObjects									() const;

	void					SpawnCurrentLevel										();
	
	void					DeleteGarbageEntities									();
	void					DeleteGarbageEntitiesInList								(int numEntities, Entity** entities);
	void					DeleteAllEntities										();
	void					DeleteAllEntitiesInList									(int numEntities, Entity** entities);
	void					DeleteStarsOffScreen									(AABB2 const& screenBoundedBox);
	void					DeleteAllStars											();
	
	void					LoadSounds												();
	void					PlayBackgroundMusics									();
	void					ToggleBackgroundMusicSpeed								(bool playSlow);

	void					UpdatePlanetLanding										(float deltaSeconds);
	void					UpdatePlanetTakeOff										(float deltaSeconds);
	

private:
	static constexpr float	PLANET_DISTANCE_FROM_SUN[NUM_PLANETS]				= { 360.f,  672.f, 930.f, 1115.f, 1840.f, 2400.f, 3370.f, 3980.f};
	static constexpr float	ASTEROID_BELT_DISTANCE_FROM_SUN						= 1000.f;
	static constexpr float	ASTEROID_BELT_WIDTH									= 200.f;

	Camera					m_screenCamera;
	
	float					m_trauma											= 0.f;
	float					m_traumaDecay										= 2.f;
	SoundID					m_backgroundMusic;
	SoundPlaybackID			m_backgroundMusicPlayback;
	SoundID					m_backgroundMusicSlowed;
	SoundPlaybackID			m_backgroundMusicSlowedPlayback;
	SoundID					m_landingSound;
	SoundID					m_landedSound;
	Vertex_PCU*				m_backgroundStars[MAX_STARS_IN_BACKGROUND];
	Vec2					m_previousStarFieldCenter;
	PlanetName				m_interactablePlanetName							= PLANET_INVALID;
	PlanetName				m_combatTetherPlanet								= PLANET_INVALID;

	float					m_cameraZoomScale									= 1.f;

	float					m_timeTillLanding									= 5.f;
	float					m_timeUntilTakenOff									= 5.f;
};