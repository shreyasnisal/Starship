#include "Game/Game.hpp"

#include "Game/Asteroid.hpp"
#include "Game/Beetle.hpp"
#include "Game/Bullet.hpp"
#include "Game/Debris.hpp"
#include "Game/DualShooter.hpp"
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Planet.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/Sun.hpp"
#include "Game/Wasp.hpp"
#include "Game/Pickup.hpp"

#include "Engine/Core/DevConsole.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"

bool Game::Event_GameClock(EventArgs& args)
{
	bool isHelp = args.GetValue("help", false);
	if (isHelp)
	{
		g_console->AddLine("Modifies settings for the game clock", false);
		g_console->AddLine("Parameters", false);
		g_console->AddLine(Stringf("\t\t%-20s: pauses the game clock", "pause"), false);
		g_console->AddLine(Stringf("\t\t%-20s: resumes the game clock", "unpause"), false);
		g_console->AddLine(Stringf("\t\t%-20s: [float >= 0.f] changes the game clock timescale to the specified value", "timescale"), false);
	}

	bool pause = args.GetValue("pause", false);
	if (pause)
	{
		g_app->m_game->m_gameClock.Pause();
	}

	bool unpause = args.GetValue("unpause", false);
	if (unpause)
	{
		g_app->m_game->m_gameClock.Unpause();
	}

	float timeScale = args.GetValue("timescale", -1.f);
	if (timeScale >= 0.f)
	{
		g_app->m_game->m_gameClock.SetTimeScale(timeScale);
	}

	return true;
}

Game::Game()
{
	m_questManager = QuestManager(this);
	
	// ShowMenu();
	// StartGame();
	LoadSounds();

	SubscribeEventCallbackFunction("Gameclock", Event_GameClock, "Modifies settings for the game clock");
}

Game::~Game()
{
	delete m_playerShip;
	m_playerShip = nullptr;

	DeleteAllEntities();

	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++)
	{
		delete m_asteroids[asteroidIndex];
		m_asteroids[asteroidIndex] = nullptr;
	}
}

void Game::LoadSounds()
{
	m_backgroundMusic = g_audio->CreateOrGetSound("Data/Audio/BackgroundMusic.ogg");
	m_backgroundMusicSlowed = g_audio->CreateOrGetSound("Data/Audio/BackgroundMusic_Slowed.ogg");
	m_landingSound = g_audio->CreateOrGetSound("Data/Audio/starship_landing1.wav");
	m_landedSound = g_audio->CreateOrGetSound("Data/Audio/landed1.wav");
	PlayBackgroundMusics();
}

void Game::PlayBackgroundMusics()
{
	m_backgroundMusicPlayback = g_audio->StartSound(m_backgroundMusic, true, 0.1f, 0.f, 1.f);
	m_backgroundMusicSlowedPlayback = g_audio->StartSound(m_backgroundMusicSlowed, true, 0.f, 0.f, 10.f);
}

void Game::ToggleBackgroundMusicSpeed(bool playSlow)
{
	if (playSlow)
	{
		g_audio->SetSoundPlaybackVolume(m_backgroundMusicPlayback, 0.f);
		g_audio->SetSoundPlaybackSpeed(m_backgroundMusicPlayback, 0.1f);

		g_audio->SetSoundPlaybackVolume(m_backgroundMusicSlowedPlayback, 0.1f);
		g_audio->SetSoundPlaybackSpeed(m_backgroundMusicSlowedPlayback, 1.f);
	}
	else
	{
		g_audio->SetSoundPlaybackVolume(m_backgroundMusicPlayback, 0.1f);
		g_audio->SetSoundPlaybackSpeed(m_backgroundMusicPlayback, 1.f);

		g_audio->SetSoundPlaybackVolume(m_backgroundMusicSlowedPlayback, 0.f);
		g_audio->SetSoundPlaybackSpeed(m_backgroundMusicSlowedPlayback, 10.f);
	}
}

void Game::DeleteAllEntities()
{
	DeleteAllEntitiesInList		(MAX_ASTEROIDS,		m_asteroids);
	DeleteAllEntitiesInList		(MAX_BEETLES,		m_beetles);
	DeleteAllEntitiesInList		(MAX_BULLETS,		m_bullets);
	DeleteAllEntitiesInList		(MAX_BULLETS,		m_enemyBullets);
	DeleteAllEntitiesInList		(MAX_DEBRIS,		m_debris);
	DeleteAllEntitiesInList		(MAX_DUAL_SHOOTERS,	m_dualShooters);
	DeleteAllEntitiesInList		(MAX_WASPS,			m_wasps);
	DeleteAllEntitiesInList		(NUM_PLANETS + 1,	m_celestialObjects);

	m_numSpawnedAsteroids = 0;
	m_numSpawnedBeetles = 0;
	m_numSpawnedWasps = 0;
}

void Game::DeleteAllEntitiesInList(int numEntities, Entity** entities)
{
	for (int entityIndex = 0; entityIndex < numEntities; entityIndex++)
	{
		delete entities[entityIndex];
		entities[entityIndex] = nullptr;
	}
}

void Game::DeleteStarsOffScreen(AABB2 const& screenBoundedBox)
{
	for (int starIndex = 0; starIndex < MAX_STARS_IN_BACKGROUND; starIndex++)
	{
		if (m_backgroundStars[starIndex])
		{
			Vertex_PCU*& star = m_backgroundStars[starIndex];

			if (!screenBoundedBox.IsPointInside(Vec2(star->m_position.x, star->m_position.y)))
			{
				delete m_backgroundStars[starIndex];
				m_backgroundStars[starIndex] = nullptr;
			}
		}
	}
}

void Game::DeleteAllStars()
{
	for (int starIndex = 0; starIndex < MAX_STARS_IN_BACKGROUND; starIndex++)
	{
		if (m_backgroundStars[starIndex])
		{
			delete m_backgroundStars[starIndex];
			m_backgroundStars[starIndex] = nullptr;
		}
	}
}

void Game::Update()
{
	float deltaSeconds = m_gameClock.GetDeltaSeconds();

	if (m_isShowingMenu)
	{
		UpdateMenu(deltaSeconds);
		return;
	}
	if (m_isShowingAttractScreen)
	{
		UpdateAttractScreen(deltaSeconds);
		return;
	}
	if (m_isShowingXboxControllerVisualTest)
	{
		UpdateXboxControllerVisualTest(deltaSeconds);
		return;
	}

	HandleDeveloperCheats();

	if (m_pauseAfterUpdate)
	{
		m_isPaused = false;
	}
	if (m_isPaused)
	{
		deltaSeconds *= 0.f;
	}

	UpdateEntities(deltaSeconds);

	if (m_isLandingOnPlanet)
	{
		UpdatePlanetLanding(deltaSeconds);
	}
	if (m_isTakingOffFromPlanet)
	{
		UpdatePlanetTakeOff(deltaSeconds);
	}

	m_playerShip->Update(deltaSeconds);
	if (m_playerShip->m_isDead && m_playerShip->m_numExtraLivesRemaining == 0)
	{
		// Game over, delay a few seconds and return to attract screen
		m_gameoverDelay -= deltaSeconds;
		if (m_gameoverDelay <= 0)
		{
			m_gameoverDelay = GAMEOVER_DELAY_SECONDS;
			m_isShowingAttractScreen = true;
			//m_isShowingMenu = true;
		}
	}


	CheckCollisionsPlayerShipVsEnemies();
	CheckCollisionsBulletsVsEnemies();
	//CheckCollisionsEnemiesVsAsteroids();
	CheckCollisionsPlayerShipVsPlanets();

	if (m_playerShip->m_isOnPlanet)
	{
		CheckCollisionsPlayerShipVsPickups();
	}
	//CheckCollisionsBulletsVsPlanets();
	//CheckCollisionsEnemiesVsPlanets();

	if (m_questManager.GetGameState() == GAMESTATE_COMBAT)
	{
		/*Vec2 combatTetherPosition = m_celestialObjects[(int)m_combatTetherPlanet]->m_position;
		Vec2 playerShipPositionWRTPlanet = (m_playerShip->m_position - combatTetherPosition);
		playerShipPositionWRTPlanet.ClampLength(m_questManager.m_combatTetherDistance);
		m_playerShip->m_position = combatTetherPosition + playerShipPositionWRTPlanet;*/
	}

	UpdateCameras(deltaSeconds);

	DeleteGarbageEntities();

	if (m_pauseAfterUpdate)
	{
		m_pauseAfterUpdate = false;
		m_isPaused = true;
	}
}


void Game::HandleDeveloperCheats()
{
	XboxController xboxController = g_input->GetController(0);

	m_isShowingAttractScreen = g_input->WasKeyJustPressed(KEYCODE_ESC) || xboxController.WasButtonJustPressed(XBOX_BUTTON_BACK);
	// m_isShowingMenu = g_input->WasKeyJustPressed(KEYCODE_ESC) || xboxController.WasButtonJustPressed(XBOX_BUTTON_BACK);
	
	if (g_input->IsKeyDown('O'))
	{
		m_gameClock.StepSingleFrame();
	}

	if (g_input->WasKeyJustPressed('T'))
	{
		// slow-motion just started
		// slow down background music
		ToggleBackgroundMusicSpeed(true);
		m_gameClock.SetTimeScale(0.1f);
	}
	if (g_input->WasKeyJustReleased('T'))
	{
		// slow-motion just ended
		// reset background music to normal speed
		ToggleBackgroundMusicSpeed(false);
		m_gameClock.SetTimeScale(1.f);
	}

	if (g_input->WasKeyJustPressed('P'))
	{
		m_gameClock.TogglePause();
	}
	if (g_input->WasKeyJustPressed('I'))
	{
		// Spawn a new Asteroid just off the screen
		SpawnNewAsteroid(GetRandomPointOffScreenByOffset(ASTEROID_COSMETIC_RADIUS));
	}
	if (g_input->WasKeyJustPressed(KEYCODE_F1))
	{
		m_drawDebug = !m_drawDebug;
	}
	if (g_input->WasKeyJustPressed('K'))
	{
		DeleteAllEntitiesInList(MAX_BEETLES, m_beetles);
		DeleteAllEntitiesInList(MAX_WASPS, m_wasps);
	}
}

void Game::ShakeCamera(float trauma)
{
	m_trauma = trauma;
}

void Game::UpdateEntities(float deltaSeconds)
{
	UpdateEntitiesInList			(MAX_ASTEROIDS,				m_asteroids,			deltaSeconds);
	UpdateEntitiesInList			(MAX_BEETLES,				m_beetles,				deltaSeconds);
	UpdateEntitiesInList			(MAX_BULLETS,				m_bullets,				deltaSeconds);
	UpdateEntitiesInList			(MAX_BULLETS,				m_enemyBullets,			deltaSeconds);
	UpdateEntitiesInList			(MAX_DEBRIS,				m_debris,				deltaSeconds);
	UpdateEntitiesInList			(MAX_DUAL_SHOOTERS,			m_dualShooters,			deltaSeconds);
	UpdateEntitiesInList			(MAX_WASPS,					m_wasps,				deltaSeconds);
	UpdateEntitiesInList			(NUM_PLANETS + 1,			m_celestialObjects,		deltaSeconds);
	UpdateEntitiesInList			(MAX_PICKUPS,				m_pickups,				deltaSeconds);
}

void Game::UpdateEntitiesInList(int numEntities, Entity** entities, float deltaSeconds)
{
	for (int entityIndex = 0; entityIndex < numEntities; entityIndex++)
	{
		if (IsEntityAlive(entities[entityIndex]))
		{
			entities[entityIndex]->Update(deltaSeconds);
		}
	}
}


void Game::CheckCollisionsBulletsVsEnemies()
{
	// Check Collisions (bullets and asteroids, beetles, wasps)
	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++)
	{
		if (IsEntityAlive(m_bullets[bulletIndex]))
		{
			Entity*& bullet = m_bullets[bulletIndex];

			CheckCollisionsBulletVsEntityList			(bullet,		MAX_ASTEROIDS,		m_asteroids);
			CheckCollisionsBulletVsEntityList			(bullet,		MAX_BEETLES,		m_beetles);
			CheckCollisionsBulletVsEntityList			(bullet,		MAX_WASPS,			m_wasps);
			CheckCollisionsBulletVsEntityList			(bullet,		MAX_DUAL_SHOOTERS,	m_dualShooters);
		}
	}

	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++)
	{
		if (IsEntityAlive(m_enemyBullets[bulletIndex]))
		{
			Entity*& bullet = m_enemyBullets[bulletIndex];

			CheckCollisionsBulletVsEntityList(bullet, MAX_ASTEROIDS, m_asteroids);
			CheckCollisionsBulletVsEntityList(bullet, MAX_BEETLES, m_beetles);
			CheckCollisionsBulletVsEntityList(bullet, MAX_WASPS, m_wasps);
		}
	}
}

void Game::CheckCollisionsBulletVsEntityList(Entity* bullet, int numEntities, Entity** entities)
{
	for (int entityIndex = 0; entityIndex < numEntities; entityIndex++)
	{
		if (IsEntityAlive(entities[entityIndex]))
		{
			Entity*& entity = entities[entityIndex];

			if (((Bullet*)bullet)->GetShooterID() == m_playerShip->GetEntityID())
			{
				if (DoDiscsOverlap(bullet->m_position, bullet->m_physicsRadius, entity->m_position, entity->m_physicsRadius))
				{
					entities[entityIndex]->TakeDamage(1);
					bullet->TakeDamage(1);
				}
			}
		}
	}
}

void Game::CheckCollisionsPlayerShipVsEnemies()
{
	if (!IsEntityAlive(m_playerShip))
	{
		return;
	}

	CheckCollisionsPlayerShipVsEntityList			(m_playerShip,		MAX_ASTEROIDS,			m_asteroids);
	CheckCollisionsPlayerShipVsEntityList			(m_playerShip,		MAX_BEETLES,			m_beetles);
	CheckCollisionsPlayerShipVsEntityList			(m_playerShip,		MAX_WASPS,				m_wasps);
	CheckCollisionsPlayerShipVsEntityList			(m_playerShip,		MAX_DUAL_SHOOTERS,		m_dualShooters);
	CheckCollisionsPlayerShipVsEntityList			(m_playerShip,		MAX_BULLETS,			m_enemyBullets);
}

bool Game::CheckCollisionsPlayerShipVsEntityList(PlayerShip* playerShip, int numEntities, Entity** entities)
{
	for (int entityIndex = 0; entityIndex < numEntities; entityIndex++)
	{
		if (IsEntityAlive(entities[entityIndex]))
		{
			Entity*& entity = entities[entityIndex];

			if (entity->m_entityType == EntityType::BULLET && ((Bullet*)entity)->GetShooterID() == playerShip->GetEntityID())
			{
				return false;
			}

			if (DoDiscsOverlap(playerShip->m_position, playerShip->m_physicsRadius, entity->m_position, entity->m_physicsRadius))
			{
				if (entity->m_entityType == EntityType::PLANET)
				{
					//m_cameraZoomScale = 15.f;
					//m_playerShip->m_scale = 0.1f;
					////m_playerShip->m_velocity = Vec2::ZERO;
					//m_playerShip->m_maxThrust = 0.1f;
				}
				if (entity->m_entityType == EntityType::BULLET)
				{
					entities[entityIndex]->TakeDamage(1);
					playerShip->TakeDamage(1);
					ShakeCamera(0.2f);
				}
				else if (entity->m_entityType == EntityType::PLANET)
				{
					 m_interactablePlanetName = ((Planet*)entity)->GetPlanetName();
				}
				else
				{
					entities[entityIndex]->TakeDamage(10);
					playerShip->TakeDamage(10);
					ShakeCamera(1.f);
				}

				return true;
			}
		}
	}

	return false;
}

void Game::CheckCollisionsPlayerShipVsPickups()
{
	for (int pickupIndex = 0; pickupIndex < MAX_PICKUPS; pickupIndex++)
	{
		if (!IsEntityAlive(m_pickups[pickupIndex]))
		{
			continue;
		}

		Pickup* pickup = dynamic_cast<Pickup*>(m_pickups[pickupIndex]);
		if (DoDiscsOverlap(m_playerShip->m_position, m_playerShip->m_physicsRadius, pickup->m_position, pickup->m_physicsRadius))
		{
			if (pickup->m_type == PickupType::AMMO)
			{
				m_playerShip->m_numBulletsRemaining += 100;
			}
			else if (pickup->m_type == PickupType::HEALTH)
			{
				m_playerShip->m_health = RoundDownToInt(GetClamped(static_cast<float>(m_playerShip->m_health + 20), 0.f, 100.f));
			}

			pickup->m_isDead = true;
			pickup->m_isGarbage = true;
		}
	}
}

void Game::CheckCollisionsEnemiesVsPlanets()
{
	CheckCollisionsEntityListVsEntityList	(MAX_ASTEROIDS,		m_asteroids,	NUM_PLANETS + 1,	m_celestialObjects);
	CheckCollisionsEntityListVsEntityList	(MAX_BEETLES,		m_beetles,		NUM_PLANETS + 1,	m_celestialObjects);
	CheckCollisionsEntityListVsEntityList	(MAX_WASPS,			m_wasps,		NUM_PLANETS + 1,	m_celestialObjects);
	CheckCollisionsEntityListVsEntityList	(MAX_BULLETS,		m_enemyBullets,	NUM_PLANETS + 1,	m_celestialObjects);
}

void Game::CheckCollisionsBulletsVsPlanets()
{
	CheckCollisionsEntityListVsEntityList	(MAX_BULLETS,		m_bullets,		NUM_PLANETS + 1,	m_celestialObjects);
}

void Game::CheckCollisionsPlayerShipVsPlanets()
{
	if (!IsEntityAlive(m_playerShip))
	{
		return;
	}

	if (!CheckCollisionsPlayerShipVsEntityList	(m_playerShip,		NUM_PLANETS + 1,	m_celestialObjects))
	{
		m_interactablePlanetName = PLANET_INVALID;
	}
}

void Game::CheckCollisionsEnemiesVsAsteroids()
{
	CheckCollisionsEntityListVsEntityList	(MAX_ASTEROIDS,		m_asteroids,	MAX_BEETLES,		m_beetles);
	CheckCollisionsEntityListVsEntityList	(MAX_ASTEROIDS,		m_asteroids,	MAX_WASPS,			m_wasps);
}

void Game::CheckCollisionsEntityListVsEntityList(int numEntitiesInList1, Entity** entityList1, int numEntitiesInList2, Entity** entityList2)
{
	for (int entity1Index = 0; entity1Index < numEntitiesInList1; entity1Index++)
	{
		if (entityList1[entity1Index])
		{
			for (int entity2Index = 0; entity2Index < numEntitiesInList2; entity2Index++)
			{
				if (entityList2[entity2Index])
				{
					Entity*& entity1 = entityList1[entity1Index];
					Entity*& entity2 = entityList2[entity2Index];

					if (DoDiscsOverlap(entity1->m_position, entity1->m_physicsRadius, entity2->m_position, entity2->m_physicsRadius))
					{
						if (entity1->m_entityType == EntityType::BULLET)
						{
							entity2->TakeDamage(1);
							entity1->TakeDamage(1);
						}
						else if (entity2->m_entityType == EntityType::BULLET)
						{
							entity1->TakeDamage(1);
							entity2->TakeDamage(1);
						}
						else
						{
							entity1->TakeDamage(10);
							entity2->TakeDamage(10);
						}
					}
				}
			}
		}
	}
}

void Game::UpdateCameras(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	m_worldCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(WORLD_X, WORLD_Y));
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_X, SCREEN_Y));
	
	if (!m_playerShip)
	{
		return;
	}

	m_worldCamera.Translate2D(m_playerShip->m_position - 0.5f * Vec2(WORLD_X, WORLD_Y));
	m_worldCamera.Zoom(m_cameraZoomScale);

	AABB2 screenBoundedBox = AABB2(m_worldCamera.GetOrthoBottomLeft(), m_worldCamera.GetOrthoTopRight());
	if (fabsf(screenBoundedBox.GetCenter().x - m_previousStarFieldCenter.x) > SCREEN_X || fabsf(screenBoundedBox.GetCenter().x - m_previousStarFieldCenter.y))
	{
		if (!m_isTakingOffFromPlanet && !m_isLandingOnPlanet && !m_playerShip->m_isOnPlanet)
		{
			DeleteStarsOffScreen(screenBoundedBox);
			GenerateBackgroundStarPositions();
		}
	}

	if (deltaSeconds > 0.f)
	{
		m_worldCamera.Translate2D
		(
			g_RNG->RollRandomVec2InRange(
				-MAX_CAMERA_SHAKE_X		*	m_trauma * m_trauma,
				MAX_CAMERA_SHAKE_X		*	m_trauma * m_trauma,
				-MAX_CAMERA_SHAKE_Y		*	m_trauma * m_trauma,
				MAX_CAMERA_SHAKE_Y		*	m_trauma * m_trauma
			)
		);

		if (m_trauma > 0.f)
		{
			m_trauma -= m_traumaDecay * deltaSeconds;
		}
	}
}

void Game::Render() const
{
	RenderCameras();
}

void Game::RenderEntities() const
{
	RenderEntitiesInList			(MAX_DEBRIS,			m_debris);
	RenderEntitiesInList			(MAX_ASTEROIDS,			m_asteroids);
	RenderEntitiesInList			(MAX_BEETLES,			m_beetles);
	RenderEntitiesInList			(MAX_BULLETS,			m_bullets);
	RenderEntitiesInList			(MAX_BULLETS,			m_enemyBullets);
	RenderEntitiesInList			(MAX_WASPS,				m_wasps);
	RenderEntitiesInList			(MAX_DUAL_SHOOTERS,		m_dualShooters);
	RenderEntitiesInList			(MAX_PICKUPS,			m_pickups);
}

void Game::RenderEntitiesInList(int numEntities, Entity* const* entities) const
{
	for (int entityIndex = 0; entityIndex < numEntities; entityIndex++)
	{
		if (IsEntityAlive(entities[entityIndex]))
		{
			if (m_drawDebug)
			{
				DebugDrawLine(m_playerShip->m_position, entities[entityIndex]->m_position, 0.2f, Rgba8(50, 50, 50, 255));
			}

			entities[entityIndex]->Render();
		}
	}
}

void Game::RenderLives() const
{	
	for (int lifeIndex = 0; lifeIndex < m_playerShip->m_numExtraLivesRemaining; lifeIndex++)
	{
		Vec2 lifePosition = Vec2(LIVES_X_OFFSET + (lifeIndex * LIVES_STRIDE), SCREEN_Y - LIVES_Y_OFFSET);
		Vertex_PCU lifeVerts[PLAYERSHIP_NUM_VERTEXES];
		for (int lifeVertexIndex = 0; lifeVertexIndex < PLAYERSHIP_NUM_VERTEXES; lifeVertexIndex++)
		{
			lifeVerts[lifeVertexIndex] = PLAYERSHIP_VERTEXES[lifeVertexIndex];
		}

		TransformVertexArrayXY3D(PLAYERSHIP_NUM_VERTEXES, lifeVerts, LIVES_SCALE, 90.f, lifePosition);
		g_renderer->BindTexture(nullptr);
		g_renderer->DrawVertexArray(PLAYERSHIP_NUM_VERTEXES, lifeVerts);
	}
}

void Game::RenderCameras() const
{
	if (!m_isShowingMenu && !m_isShowingXboxControllerVisualTest && !m_isShowingAttractScreen)
	{
		g_renderer->BeginCamera(m_worldCamera);

		if (m_cameraZoomScale < 1.5f && !m_isTakingOffFromPlanet)
		{
			RenderBackground();
		}
		RenderCelestialObjects();
		RenderEntities();
		m_playerShip->Render();

		g_renderer->EndCamera(m_worldCamera);
	}

	g_renderer->BeginCamera(m_screenCamera);

	if (m_isShowingMenu)
	{
		RenderMenu();
		return;
	}
	if (m_isShowingAttractScreen)
	{
		RenderAttractScreen();
		return;
	}
	if (m_isShowingXboxControllerVisualTest)
	{
		RenderXboxControllerVisualTest();
		return;
	}

	RenderLives();
	RenderHUD();
	if (m_isLandingOnPlanet || !m_playerShip->m_isOnPlanet)
	{
		m_questManager.ShowPlanetInteractionText(m_interactablePlanetName, Vec2(SCREEN_X - 400.f, 80.f));
	}
	m_playerShip->RenderLowHealthIndicator();

	g_renderer->EndCamera(m_screenCamera);
}

void Game::RenderBackground() const
{
	std::vector<Vertex_PCU> starVerts;
	starVerts.reserve(1000);

	for (int starIndex = 0; starIndex < MAX_STARS_IN_BACKGROUND; starIndex++)
	{
		Vertex_PCU* const& star = m_backgroundStars[starIndex];
		if (star)
		{
			starVerts.push_back(Vertex_PCU(star->m_position + Vec3(STAR_RADIUS, 0.f, 0.f), star->m_color, star->m_uvTexCoords));
			starVerts.push_back(Vertex_PCU(star->m_position + Vec3(0.f, STAR_RADIUS, 0.f), star->m_color, star->m_uvTexCoords));
			starVerts.push_back(Vertex_PCU(star->m_position - Vec3(0.f, STAR_RADIUS, 0.f), star->m_color, star->m_uvTexCoords));
			starVerts.push_back(Vertex_PCU(star->m_position - Vec3(STAR_RADIUS, 0.f, 0.f), star->m_color, star->m_uvTexCoords));
			starVerts.push_back(Vertex_PCU(star->m_position + Vec3(0.f, STAR_RADIUS, 0.f), star->m_color, star->m_uvTexCoords));
			starVerts.push_back(Vertex_PCU(star->m_position - Vec3(0.f, STAR_RADIUS, 0.f), star->m_color, star->m_uvTexCoords));
		}
	}
	g_renderer->BindTexture(nullptr);
	g_renderer->DrawVertexArray(starVerts);
}


Bullet* Game::SpawnNewBullet(Vec2 const& bulletPosition, float bulletDirection, Rgba8 const& bulletTailColor, int shooterID)
{
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (shooterID == m_playerShip->GetEntityID())
		{
			if (m_bullets[i] == nullptr)
			{
				m_bullets[i] = new Bullet(this, bulletPosition, bulletDirection, bulletTailColor, shooterID);
				return (Bullet*)m_bullets[i];
			}
		}
		else
		{
			if (m_enemyBullets[i] == nullptr)
			{
				m_enemyBullets[i] = new Bullet(this, bulletPosition, bulletDirection, bulletTailColor, shooterID);
				return (Bullet*)m_enemyBullets[i];
			}
		}
	}

	ERROR_RECOVERABLE("Could not fire bullet. All slots are full!");
	return nullptr;
}

Asteroid* Game::SpawnNewAsteroid(Vec2 const& asteroidPosition)
{
	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++)
	{
		if (m_asteroids[asteroidIndex] == nullptr)
		{
			m_asteroids[asteroidIndex] = new Asteroid(this, asteroidPosition, asteroidIndex);
			m_numSpawnedAsteroids++;
			return (Asteroid*)m_asteroids[asteroidIndex];
		}
	}

	ERROR_RECOVERABLE("Could not spawn asteroid. All slots are full!");
	return nullptr;
}

Beetle* Game::SpawnNewBeetle(Vec2 const& beetlePosition)
{
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++)
	{
		if (m_beetles[beetleIndex] == nullptr)
		{
			m_beetles[beetleIndex] = new Beetle(this, beetlePosition, beetleIndex);
			m_numSpawnedBeetles++;
			return (Beetle*)m_beetles[beetleIndex];
		}
	}

	ERROR_RECOVERABLE("Could not spawn beetle. All slots are full!");
	return nullptr;
}

Wasp* Game::SpawnNewWasp(Vec2 const& waspPosition)
{
	for (int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++)
	{
		if (m_wasps[waspIndex] == nullptr)
		{
			m_wasps[waspIndex] = new Wasp(this, waspPosition, waspIndex);
			m_numSpawnedWasps++;
			return (Wasp*)m_wasps[waspIndex];
		}
	}

	ERROR_RECOVERABLE("Could not spawn wasp. All slots are full!");
	return nullptr;
}

DualShooter* Game::SpawnNewDualShooter(Vec2 const& dualShooterPosition)
{
	for (int dualShooterIndex = 0; dualShooterIndex < MAX_DUAL_SHOOTERS; dualShooterIndex++)
	{
		if (m_dualShooters[dualShooterIndex] == nullptr)
		{
			m_dualShooters[dualShooterIndex] = new DualShooter(this, dualShooterPosition, dualShooterIndex);
			m_numSpawnedDualShooters++;
			return (DualShooter*)m_dualShooters[dualShooterIndex];
		}
	}

	ERROR_RECOVERABLE("Could not spawn dual shooter. All slots are full!");
	return nullptr;
}

void Game::SpawnDebris(Vec2 const& debrisPosition, Vec2 const& debrisVelocity, Rgba8 const& debrisColor, float debrisScale, float debrisLifespan)
{
	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++)
	{
		if (m_debris[debrisIndex] == nullptr)
		{
			m_debris[debrisIndex] = new Debris(this, debrisPosition, debrisVelocity, debrisColor, debrisScale, debrisLifespan);
			return;
		}
	}

	ERROR_RECOVERABLE("Error spawning debris. All slots are full!");
}

void Game::SpawnPickup(Vec2 const& spawnPosition, PickupType type)
{
	for (int pickupIndex = 0; pickupIndex < MAX_PICKUPS; pickupIndex++)
	{
		if (m_pickups[pickupIndex] == nullptr)
		{
			m_pickups[pickupIndex] = new Pickup(this, spawnPosition, type);
			return;
		}
	}
}

void Game::DeleteGarbageEntities()
{
	DeleteGarbageEntitiesInList				(MAX_ASTEROIDS,			m_asteroids);
	DeleteGarbageEntitiesInList				(MAX_BEETLES,			m_beetles);
	DeleteGarbageEntitiesInList				(MAX_BULLETS,			m_bullets);
	DeleteGarbageEntitiesInList				(MAX_BULLETS,			m_enemyBullets);
	DeleteGarbageEntitiesInList				(MAX_DEBRIS,			m_debris);
	DeleteGarbageEntitiesInList				(MAX_WASPS,				m_wasps);
	DeleteGarbageEntitiesInList				(MAX_DUAL_SHOOTERS,		m_dualShooters);

	if (m_numSpawnedBeetles == 0 && m_numSpawnedWasps == 0)
	{
		if (m_questManager.GetGameState() == GAMESTATE_COMBAT)
		{
   			m_questManager.StartNextWave();
		}
	}
}

void Game::DeleteGarbageEntitiesInList(int numEntities, Entity** entities)
{
	for (int entityIndex = 0; entityIndex < numEntities; entityIndex++)
	{
		if (entities[entityIndex] != nullptr && entities[entityIndex]->m_isGarbage)
		{
			
			// check entity type and decrement enemies of that type if the entity is an enemy
			// used to determine if next level/wave should be started
			EntityType& entityType = entities[entityIndex]->m_entityType;
			if (entityType == EntityType::ASTEROID)
			{
				m_numSpawnedAsteroids--;
			}
			else if (entityType == EntityType::BEETLE)
			{
				m_numSpawnedBeetles--;
			}
			else if (entityType == EntityType::WASP)
			{
    				m_numSpawnedWasps--;
			}
			delete entities[entityIndex];
			entities[entityIndex] = nullptr;
		}
	}
}

bool Game::IsEntityAlive(Entity* entity) const
{
	if (entity == nullptr)
	{
		return false;
	}

	return !(entity->m_isDead);
}


void Game::StartQuest()
{
	m_questManager.HandlePlanetInteraction(m_interactablePlanetName);
}

void Game::LandOnPlanet()
{
	if (m_interactablePlanetName != PLANET_INVALID)
	{
		m_timeTillLanding = 5.f;
		m_playerShip->m_velocity = Vec2::ZERO;
		m_isLandingOnPlanet = true;
		g_audio->StartSound(m_landingSound, false, 1.f, 0.f, 0.5f);
	}
}

void Game::UpdatePlanetLanding(float deltaSeconds)
{
	static Vec2 s_playerShipPositionAtLandingStart = m_playerShip->m_position;

	m_playerShip->UpdateLandingOnPlanet(m_timeTillLanding);

	Entity*& planet = m_celestialObjects[(int)m_interactablePlanetName];
	Vec2 directionPlayerShipToPlanet = (planet->m_position - m_playerShip->m_position).GetNormalized();
	float orientationPlayerShipToPlanet = directionPlayerShipToPlanet.GetOrientationDegrees();

	m_playerShip->m_orientationDegrees = orientationPlayerShipToPlanet;
	m_playerShip->m_position.x = Interpolate(s_playerShipPositionAtLandingStart.x, planet->m_position.x, 1.f - m_timeTillLanding / 5.f);
	m_playerShip->m_position.y = Interpolate(s_playerShipPositionAtLandingStart.y, planet->m_position.y, 1.f - m_timeTillLanding / 5.f);
	
	m_cameraZoomScale = Interpolate(1.f, 20.f, 1.f - m_timeTillLanding / 5.f);
	m_playerShip->m_scale = Interpolate(1.f, 0.05f, 1.f - m_timeTillLanding / 5.f);
	m_playerShip->m_physicsRadius = Interpolate(PLAYERSHIP_PHYSICS_RADIUS, PLAYERSHIP_PHYSICS_RADIUS * 0.05f, 1.f - m_timeTillLanding / 5.f);
	m_playerShip->m_cosmeticRadius = Interpolate(PLAYERSHIP_COSMETIC_RADIUS, PLAYERSHIP_COSMETIC_RADIUS * 0.05f, 1.f - m_timeTillLanding / 5.f);
	
	m_timeTillLanding -= deltaSeconds;

	if (m_timeTillLanding <= 0.f)
	{
		m_playerShip->UpdateLandingOnPlanet(0.f);
		m_playerShip->m_maxThrust = 1.f / 150.f;
		m_isLandingOnPlanet = false;
		g_audio->StartSound(m_landedSound);
		m_playerShip->m_isOnPlanet = true;
		m_playerShip->m_landedPlanet = (Planet*)planet;
		SpawnPickups(planet->m_position, planet->m_cosmeticRadius);
	}
}

void Game::TakeOffFromPlanet()
{
	if (m_playerShip->m_isOnPlanet)
	{
		m_timeUntilTakenOff = 5.f;
		m_playerShip->m_velocity = Vec2::ZERO;
		m_isTakingOffFromPlanet = true;
		m_playerShip->m_isOnPlanet = false;
		g_audio->StartSound(m_landingSound, false, 1.f, 0.f, 0.5f);
	}
}

void Game::UpdatePlanetTakeOff(float deltaSeconds)
{
	static Vec2 s_playerShipPositionAtTakeOffStart = m_playerShip->m_position;

	m_playerShip->UpdateTakingOffFromPlanet(m_timeUntilTakenOff);

	Planet*& planet = m_playerShip->m_landedPlanet;
	Vec2 directionToSpacePosition = (m_playerShip->m_position - planet->m_position).GetNormalized();
	Vec2 spacePosition = planet->m_position + directionToSpacePosition * planet->m_physicsRadius;
	float orientationToSpacePosition = directionToSpacePosition.GetOrientationDegrees();

	m_playerShip->m_orientationDegrees = orientationToSpacePosition;
	m_playerShip->m_position.x = Interpolate(s_playerShipPositionAtTakeOffStart.x, spacePosition.x, 1.f - m_timeUntilTakenOff / 5.f);
	m_playerShip->m_position.y = Interpolate(s_playerShipPositionAtTakeOffStart.y, spacePosition.y, 1.f - m_timeUntilTakenOff / 5.f);

	m_cameraZoomScale = Interpolate(20.f, 1.f, 1.f - m_timeUntilTakenOff / 5.f);
	m_playerShip->m_scale = Interpolate(0.05f, 1.f, 1.f - m_timeUntilTakenOff / 5.f);
	m_playerShip->m_physicsRadius = Interpolate(PLAYERSHIP_PHYSICS_RADIUS * 0.05f, PLAYERSHIP_PHYSICS_RADIUS, 1.f - m_timeUntilTakenOff / 5.f);
	m_playerShip->m_cosmeticRadius = Interpolate(PLAYERSHIP_COSMETIC_RADIUS * 0.05f, PLAYERSHIP_COSMETIC_RADIUS, 1.f - m_timeUntilTakenOff / 5.f);

	m_timeUntilTakenOff -= deltaSeconds;

	if (m_timeUntilTakenOff <= 0.f)
	{
		m_isTakingOffFromPlanet = false;
		m_playerShip->m_maxThrust = 1.f;
		m_playerShip->m_landedPlanet = nullptr;
		DeleteAllEntitiesInList(MAX_PICKUPS, m_pickups);
	}
}

void Game::SetTetherPlanet(PlanetName planetName)
{
	m_combatTetherPlanet = planetName;
}


void Game::SpawnCurrentLevel()
{
	for (int asteroidIndex = 0; asteroidIndex < ASTEROIDS_IN_LEVEL[m_currentLevel]; asteroidIndex++)
	{
		Vec2 const spawnPoint = GetRandomPointOffScreenByOffset(ASTEROID_COSMETIC_RADIUS);
		SpawnNewAsteroid(spawnPoint);
	}

	for (int beetleIndex = 0; beetleIndex < BEETLES_IN_LEVEL[m_currentLevel]; beetleIndex++)
	{
		Vec2 const spawnPoint = GetRandomPointOffScreenByOffset(BEETLE_COSMETIC_RADIUS);
		SpawnNewBeetle(spawnPoint);
	}

	for (int waspIndex = 0; waspIndex < WASPS_IN_LEVEL[m_currentLevel]; waspIndex++)
	{
		Vec2 const spawnPoint = GetRandomPointOffScreenByOffset(WASP_COSMETIC_RADIUS);
		SpawnNewWasp(spawnPoint);
	}
}

void Game::StartGame()
{
	if (!m_playerShip)
	{
		m_playerShip = new PlayerShip(this, Vec2(WORLD_X, WORLD_Y) * 0.5f);
	}
	else
	{
		m_playerShip->ResetAndRespawn(Vec2(WORLD_X, WORLD_Y) * 0.5f);
		m_playerShip->m_numExtraLivesRemaining = PLAYERSHIP_LIVES;
	}

	DeleteAllEntities();
	DeleteAllStars();
	m_questManager = QuestManager(this);

	m_worldCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(WORLD_X, WORLD_Y));
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_X, SCREEN_Y));
	
	m_worldCamera.Translate2D(m_playerShip->m_position - 0.5f * Vec2(WORLD_X, WORLD_Y));
	m_worldCamera.Zoom(1.f);

	GenerateBackgroundStarPositions(true);
	GenerateCelestialObjects();

	m_drawDebug = false;
}

void Game::GenerateBackgroundStarPositions(bool canDrawOnScreen)
{
	AABB2 screenBoundedBox(m_worldCamera.GetOrthoBottomLeft(), m_worldCamera.GetOrthoTopRight());
	AABB2 starFieldBox = AABB2(screenBoundedBox);
	starFieldBox.SetDimensions(screenBoundedBox.GetDimensions() * 2.f);

	int numStars = g_RNG->RollRandomIntInRange(MIN_STARS_IN_BACKGROUND * (canDrawOnScreen ? 4 : 1), MAX_STARS_IN_BACKGROUND * (canDrawOnScreen ? 4 : 1));

	for (int starIndex = 0; starIndex < numStars; starIndex++)
	{
		
		Vec2 starPosition = screenBoundedBox.GetCenter();
		do 
		{
			starPosition = g_RNG->RollRandomVec2InRange(starFieldBox.m_mins.x, starFieldBox.m_maxs.x, starFieldBox.m_mins.y, starFieldBox.m_maxs.y);
		} while (screenBoundedBox.IsPointInside(starPosition) && !canDrawOnScreen);

		unsigned char starOpacity = (unsigned char)g_RNG->RollRandomIntInRange((int)MIN_STAR_OPACITY, (int)MAX_STAR_OPACITY);

		Rgba8 starColor(STAR_COLOR.r, STAR_COLOR.g, STAR_COLOR.b, starOpacity);

		for (int star = 0; star < MAX_STARS_IN_BACKGROUND; star++)
		{
			if (!m_backgroundStars[star])
			{
				m_backgroundStars[star] = new Vertex_PCU(Vec3(starPosition.x, starPosition.y, 0.f), starColor, Vec2(0.f, 0.f));
				break;
			}
		}
	}

	m_previousStarFieldCenter = screenBoundedBox.GetCenter();
}

void Game::UpdateAttractScreen(float deltaSeconds)
{
	XboxController xboxController = g_input->GetController(0);

	if (g_input->WasKeyJustPressed('X'))
	{
		m_isShowingAttractScreen = false;
		m_isShowingXboxControllerVisualTest = true;
	}

	if (g_input->WasKeyJustPressed('Y'))
	{
		SoundID testSound = g_audio->CreateOrGetSound("Data/Audio/TestSound.mp3");
		g_audio->StartSound(testSound);
	}

	bool wasAnyStartKeyPressed =	g_input->WasKeyJustPressed(' ') ||
									g_input->WasKeyJustPressed('N') ||
									xboxController.WasButtonJustPressed(XBOX_BUTTON_A) ||
									xboxController.WasButtonJustPressed(XBOX_BUTTON_START);

	if (wasAnyStartKeyPressed)
	{
		m_isShowingAttractScreen = false;
		StartGame();
	}
	if (g_input->WasKeyJustPressed(KEYCODE_ESC) || xboxController.WasButtonJustPressed(XBOX_BUTTON_BACK))
	{
		g_app->HandleQuitRequested();
	}

	if (m_attractScreenPlayButtonOpacity <= 127)
	{
		m_attractScreenPlayButtonOpacityIsIncreasing = true;
	}
	else if (m_attractScreenPlayButtonOpacity >= 255)
	{
		m_attractScreenPlayButtonOpacityIsIncreasing = false;
	}

	if (m_attractScreenPlayButtonOpacityIsIncreasing)
	{
		m_attractScreenPlayButtonOpacity = (unsigned char)GetClamped((float)m_attractScreenPlayButtonOpacity + 255 * deltaSeconds, 127, 255);
	}
	else
	{
		m_attractScreenPlayButtonOpacity = (unsigned char)GetClamped((float)m_attractScreenPlayButtonOpacity - 255 * deltaSeconds, 127, 255);
	}

	if (m_attractScreenShipDistanceFromScreenCenter <= ATTRACT_SCREEN_SHIP_MIN_OFFSET)
	{
		m_isAttractScreenShipMovingTowardsCenter = false;
	}
	else if (m_attractScreenShipDistanceFromScreenCenter >= ATTRACT_SCREEN_SHIP_MAX_OFFSET)
	{
		m_isAttractScreenShipMovingTowardsCenter = true;
	}

	if (m_isAttractScreenShipMovingTowardsCenter)
	{
		m_attractScreenShipDistanceFromScreenCenter -= ATTRACT_SCREEN_SHIP_SPEED * deltaSeconds;
	}
	else
	{
		m_attractScreenShipDistanceFromScreenCenter += ATTRACT_SCREEN_SHIP_SPEED * deltaSeconds;
	}

	UpdateCameras(deltaSeconds);
}

void Game::RenderAttractScreen() const
{
	Vec2 attractScreenShip1Pos = m_screenCamera.GetOrthoTopRight() * 0.5f - Vec2(m_attractScreenShipDistanceFromScreenCenter , 0);
	Vec2 attractScreenShip2Pos = m_screenCamera.GetOrthoTopRight() * 0.5f + Vec2(m_attractScreenShipDistanceFromScreenCenter, 0);

	Vertex_PCU attractScreenShip1Verts[PLAYERSHIP_NUM_VERTEXES];
	for (int shipVertIndex = 0; shipVertIndex < PLAYERSHIP_NUM_VERTEXES; shipVertIndex++)
	{
		attractScreenShip1Verts[shipVertIndex] = PLAYERSHIP_VERTEXES[shipVertIndex];
	}

	TransformVertexArrayXY3D(PLAYERSHIP_NUM_VERTEXES, attractScreenShip1Verts, ATTRACT_SCREEN_PLAYERSHIPS_SCALE, 0.f, attractScreenShip1Pos);
	g_renderer->BindTexture(nullptr);
	g_renderer->DrawVertexArray(PLAYERSHIP_NUM_VERTEXES, attractScreenShip1Verts);

	Vertex_PCU attractScreenShip2Verts[PLAYERSHIP_NUM_VERTEXES];
	for (int shipVertIndex = 0; shipVertIndex < PLAYERSHIP_NUM_VERTEXES; shipVertIndex++)
	{
		attractScreenShip2Verts[shipVertIndex] = PLAYERSHIP_VERTEXES[shipVertIndex];
	}

	TransformVertexArrayXY3D(PLAYERSHIP_NUM_VERTEXES, attractScreenShip2Verts, ATTRACT_SCREEN_PLAYERSHIPS_SCALE, 180.f, attractScreenShip2Pos);
	g_renderer->BindTexture(nullptr);
	g_renderer->DrawVertexArray(PLAYERSHIP_NUM_VERTEXES, attractScreenShip2Verts);


	Vertex_PCU attractScreenPlayButtonVerts[ATTRACT_SCREEN_PLAY_BUTTON_VERTEXES];
	for (int vertexIndex = 0; vertexIndex < ATTRACT_SCREEN_PLAY_BUTTON_VERTEXES; vertexIndex++)
	{
		attractScreenPlayButtonVerts[vertexIndex] = m_playButtonVerts[vertexIndex];
		attractScreenPlayButtonVerts[vertexIndex].m_color.a = m_attractScreenPlayButtonOpacity;
	}

	TransformVertexArrayXY3D(ATTRACT_SCREEN_PLAY_BUTTON_VERTEXES, attractScreenPlayButtonVerts, ATTRACT_SCREEN_PLAY_BUTTON_SCALE, 0.f, m_screenCamera.GetOrthoTopRight() * 0.5f);
	g_renderer->BindTexture(nullptr);
	g_renderer->DrawVertexArray(ATTRACT_SCREEN_PLAY_BUTTON_VERTEXES, attractScreenPlayButtonVerts);
}

Vec2 const Game::GetRandomPointOffScreenByOffset(float offset) const
{
	int edgeToSpawnAt = g_RNG->RollRandomIntInRange(0, 3);

	Vec2 const& BL = m_worldCamera.GetOrthoBottomLeft();
	Vec2 const& TR = m_worldCamera.GetOrthoTopRight();

	Vec2 spawnPosition;

	if (edgeToSpawnAt == 0)
	{
		// Left edge
		spawnPosition = Vec2(BL.x - offset, g_RNG->RollRandomFloatInRange(BL.y, TR.y));
	}
	else if (edgeToSpawnAt == 1 || edgeToSpawnAt == 2)
	{
		// Right Edge
		spawnPosition = Vec2(TR.x + offset, g_RNG->RollRandomFloatInRange(BL.y, TR.y));
	}
	else
	{
		// Bottom Edge
		spawnPosition = Vec2(g_RNG->RollRandomFloatInRange(BL.x, TR.x), BL.y - offset);
	}

	return spawnPosition;
}

void Game::UpdateXboxControllerVisualTest(float deltaSeconds)
{
	UNUSED(deltaSeconds);

	XboxController controller = g_input->GetController(0);
	if (g_input->WasKeyJustPressed(KEYCODE_ESC))
	{
		m_isShowingXboxControllerVisualTest = false;
		m_isShowingAttractScreen = true;
	}
}

void Game::RenderXboxControllerVisualTest() const
{
	XboxController controller = g_input->GetController(0);
	RenderAnalogJoystick(controller.GetLeftStick(), Vec2(SCREEN_X * 0.25f, SCREEN_Y * 0.55f));
	RenderAnalogJoystick(controller.GetRightStick(), Vec2(SCREEN_X * 0.75f, SCREEN_Y * 0.55f));
	RenderXboxButtons();
	RenderXboxTrigger(controller.GetLeftTrigger(), Vec2(5.f, SCREEN_Y * 0.9f), 5.f);
	RenderXboxTrigger(controller.GetRightTrigger(), Vec2(SCREEN_X - 10.f, SCREEN_Y * 0.9f), 5.f);
}

void Game::RenderAnalogJoystick(AnalogJoystick const& joystick, Vec2 const& center) const
{
	float joystickRadius = SCREEN_Y * 0.4f;
	float innerDeadZoneRadius = joystickRadius * joystick.GetInnerDeadZoneFraction();
	float outerDeadZoneRadius = joystickRadius * joystick.GetOuterDeadZoneFraction();
	Rgba8 joystickRingColor(50, 50, 50, 255);
	Rgba8 deadzoneRingColor(255, 100, 100, 255);
	Rgba8 uncorrectedPositionColor(255, 0, 0, 255);
	Rgba8 deadZoneCorrectedPositionColor(0, 255, 0, 255);
	float lineThickness = 2.f;

	DebugDrawRing(center, joystickRadius, lineThickness, joystickRingColor);
	DebugDrawRing(center, innerDeadZoneRadius, lineThickness, deadzoneRingColor);
	DebugDrawRing(center, outerDeadZoneRadius, lineThickness, deadzoneRingColor);
	DebugDrawDisc(center + joystickRadius * joystick.GetRawUncorrectedPosition(), 10.f, uncorrectedPositionColor);
	DebugDrawDisc(center + joystickRadius * joystick.GetPosition(), 10.f, deadZoneCorrectedPositionColor);
	DebugDrawLine(Vec2(center.x - joystickRadius, center.y), Vec2(center.x + joystickRadius, center.y), lineThickness, joystickRingColor);
	DebugDrawLine(Vec2(center.x, center.y - joystickRadius), Vec2(center.x, center.y + joystickRadius), lineThickness, joystickRingColor);
}

void Game::RenderXboxButtons() const
{
	float buttonStride = SCREEN_X / (float)((int)XBOX_BUTTON_NUM + 1);

	for (int buttonIndex = 0; buttonIndex < (int)XBOX_BUTTON_NUM; buttonIndex++)
	{
		RenderXboxButton(buttonIndex, Vec2(buttonStride * (float)(buttonIndex + 1), 80.f));
	}
}

void Game::RenderXboxButton(int buttonIndex, Vec2 const& center) const
{
	XboxController controller = g_input->GetController(0);

	float circleRadius = 16.f;
	float lineThickness = 1.f;
	Rgba8 pressedColor(100, 255, 100, 255);
	Rgba8 justPressedColor(255, 255, 255, 255);
	Rgba8 unpressedColor(255, 255, 100, 255);

	if (controller.WasButtonJustPressed(XboxButtonID(buttonIndex)))
	{
		DebugDrawDisc(center, circleRadius, justPressedColor);
	}
	else if (controller.IsButtonDown(XboxButtonID(buttonIndex)))
	{
		DebugDrawDisc(center, circleRadius, pressedColor);
	}
	else
	{
		DebugDrawRing(center, circleRadius, lineThickness, unpressedColor);
	}
}

void Game::RenderXboxTrigger(float triggerValue, Vec2 const& startPosition, float maxLength) const
{
	float lineThickness = 10.f;
	Rgba8 triggerColor(255, 0, 255, 255);
	DebugDrawLine(startPosition, startPosition + Vec2(triggerValue * maxLength, 0), lineThickness, triggerColor);
}

void Game::RenderRadar() const
{
	float radarRadius = 120.f;
	Rgba8 radarColor(50, 155, 50, 155);
	Vec2 radarPosition(150.f, 150.f);
	
	float radarOuterRingRadius = 80.f;
	float radarInnerRingRadius = 40.f;
	float lineThickness = 1.f;
	Rgba8 lineColor(20, 50, 20, 155);

	DebugDrawDisc(radarPosition, radarRadius, radarColor);
	DebugDrawRing(radarPosition, radarOuterRingRadius, lineThickness, lineColor);
	DebugDrawRing(radarPosition, radarInnerRingRadius, lineThickness, lineColor);

	DebugDrawLine(Vec2(radarPosition.x - radarRadius, radarPosition.y), Vec2(radarPosition.x + radarRadius, radarPosition.y), lineThickness, lineColor);
	DebugDrawLine(Vec2(radarPosition.x, radarPosition.y - radarRadius), Vec2(radarPosition.x, radarPosition.y + radarRadius), lineThickness, lineColor);

	RenderEntitiesOnRadar(radarPosition, radarRadius);
}

void Game::RenderHUD() const
{
	if (!m_isLandingOnPlanet && !m_isTakingOffFromPlanet && !m_playerShip->m_isOnPlanet)
	{
		RenderRadar();
	}

	RenderBulletsRemaining();
}

void Game::RenderBulletsRemaining() const
{
	constexpr int NUM_BULLET_HUD_VERTEXES = 9;
	Vertex_PCU bulletHUDVerts[NUM_BULLET_HUD_VERTEXES] =
	{
		Vertex_PCU(Vec3(2.f, 0.f, 0.f), Rgba8(255, 255, 0, 255), Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(0.f, 1.f, 0.f), Rgba8(255, 255, 0, 255), Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(0.f, -1.f, 0.f), Rgba8(255, 255, 0, 255), Vec2(0.f, 0.f)),
		/**/
		Vertex_PCU(Vec3(0.f, 1.f, 0.f), Rgba8(255, 255, 0, 255), Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-4.f, 1.f, 0.f), Rgba8(255, 255, 0, 255), Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-4.f, -1.f, 0.f), Rgba8(255, 255, 0, 255), Vec2(0.f, 0.f)),
		/**/
		Vertex_PCU(Vec3(0.f, 1.f, 0.f), Rgba8(255, 255, 0, 255), Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(-4.f, -1.f, 0.f), Rgba8(255, 255, 0, 255), Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(0.f, -1.f, 0.f), Rgba8(255, 255, 0, 255), Vec2(0.f, 0.f)),
		/**/
	};
	TransformVertexArrayXY3D(NUM_BULLET_HUD_VERTEXES, bulletHUDVerts, SCREEN_X / WORLD_X, 90.f, Vec2(SCREEN_X - 240.f, SCREEN_Y - 60.f));
	g_renderer->BindTexture(nullptr);
	g_renderer->DrawVertexArray(NUM_BULLET_HUD_VERTEXES, bulletHUDVerts);

	std::vector<Vertex_PCU> numBulletsRemainingVerts;
	AddVertsForTextTriangles2D(numBulletsRemainingVerts, std::to_string(m_playerShip->m_numBulletsRemaining), Vec2(SCREEN_X - 200.f, SCREEN_Y - 100.f), 50.f, Rgba8(255, 255, 255, 255));
	g_renderer->BindTexture(nullptr);
	g_renderer->DrawVertexArray((int)numBulletsRemainingVerts.size(), numBulletsRemainingVerts.data());
}

void Game::RenderEntitiesOnRadar(Vec2 const& radarPosition, float radarRadius) const
{
	RenderAsteroidsOnRadar(radarPosition, radarRadius);
	RenderPlanetsOnRadar(radarPosition, radarRadius);

	RenderEnemiesOnRadar	(MAX_BEETLES,				m_beetles,			radarPosition,		radarRadius);
	RenderEnemiesOnRadar	(MAX_WASPS,					m_wasps,			radarPosition,		radarRadius);
	RenderEnemiesOnRadar	(MAX_DUAL_SHOOTERS,			m_dualShooters,		radarPosition,		radarRadius);
}

void Game::RenderAsteroidsOnRadar(Vec2 const& radarPosition, float radarRadius) const
{
	Rgba8 asteroidRadarColor(50, 50, 50, 155);
	float asteroidRadiusOnRadar = 5.f;

	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++)
	{
		if (m_asteroids[asteroidIndex])
		{
			Entity* const& asteroid = m_asteroids[asteroidIndex];
			float asteroidDistance = GetDistance2D(asteroid->m_position, m_playerShip->m_position); // in world units
			// radar is in screen units
			if (asteroidDistance > radarRadius * 10.f)
			{
				continue;
			}

			float asteroidOrientationDegrees = (m_playerShip->m_position - asteroid->m_position).GetOrientationDegrees();

			float asteroidDistanceOnRadar = RangeMap(asteroidDistance, 0.f, radarRadius * 10.f, 0.f, radarRadius);
			Vec2 asteroidPositionOnRadar = Vec2::MakeFromPolarDegrees(asteroidOrientationDegrees, asteroidDistanceOnRadar);

			DebugDrawDisc(radarPosition - asteroidPositionOnRadar, asteroidRadiusOnRadar, asteroidRadarColor);
		}
	}
}

void Game::RenderPlanetsOnRadar(Vec2 const& radarPosition, float radarRadius) const
{
	Rgba8 sunRadarColor(255, 127, 40, 155);
	float sunRadiusOnRadar = 8.f;
	Rgba8 planetRadarColor(50, 50, 250, 155);
	float planetRadiusOnRadar = 5.f;

	for (int planetIndex = 0; planetIndex < NUM_PLANETS; planetIndex++)
	{
		Entity* const& planet = m_celestialObjects[planetIndex];
		float planetDistance = GetDistance2D(planet->m_position, m_playerShip->m_position); // in world units
		// radar is in screen units
		if (planetDistance > radarRadius * 10.f)
		{
			continue;
		}

		float planetOrientationDegrees = (m_playerShip->m_position - planet->m_position).GetOrientationDegrees();

		float planetDistanceOnRadar = RangeMap(planetDistance, 0.f, radarRadius * 10.f, 0.f, radarRadius);
		Vec2 planetPositionOnRadar = Vec2::MakeFromPolarDegrees(planetOrientationDegrees, planetDistanceOnRadar);

		DebugDrawDisc(radarPosition - planetPositionOnRadar, planetRadiusOnRadar, planetRadarColor);
	}

	// Render Sun on radar
	Entity* const& sun = m_celestialObjects[NUM_PLANETS];
	float sunDistance = GetDistance2D(sun->m_position, m_playerShip->m_position); // in world units
	// radar is in screen units
	if (sunDistance < radarRadius * 10.f)
	{
		float sunOrientationDegrees = (m_playerShip->m_position - sun->m_position).GetOrientationDegrees();

		float sunDistanceOnRadar = RangeMap(sunDistance, 0.f, radarRadius * 10.f, 0.f, radarRadius);
		Vec2 sunPositionOnRadar = Vec2::MakeFromPolarDegrees(sunOrientationDegrees, sunDistanceOnRadar);

		DebugDrawDisc(radarPosition - sunPositionOnRadar, sunRadiusOnRadar, sunRadarColor);
	}

}

void Game::RenderEnemiesOnRadar(int numEnemies, Entity* const* enemies, Vec2 const& radarPosition, float radarRadius) const
{
	Rgba8 enemyRadarColor(155, 50, 50, 155);
	float enemyRadiusOnRadar = 5.f;

	for (int enemyIndex = 0; enemyIndex < numEnemies; enemyIndex++)
	{
		if (enemies[enemyIndex])
		{
			Entity* const& enemy = enemies[enemyIndex];
			float enemyDistance = GetDistance2D(enemy->m_position, m_playerShip->m_position); // in world units
			// radar is in screen units
			if (enemyDistance > radarRadius * 10.f)
			{
				continue;
			}

			float enemyOrientationDegrees = (m_playerShip->m_position - enemy->m_position).GetOrientationDegrees();

			float enemyDistanceOnRadar = RangeMap(enemyDistance, 0.f, radarRadius * 10.f, 0.f, radarRadius);
			Vec2 enemyPositionOnRadar = Vec2::MakeFromPolarDegrees(enemyOrientationDegrees, enemyDistanceOnRadar);

			DebugDrawDisc(radarPosition - enemyPositionOnRadar, enemyRadiusOnRadar, enemyRadarColor);
		}
	}
}

void Game::GenerateCelestialObjects()
{
	m_celestialObjects[NUM_PLANETS] = new Sun(this, SUN_POSITION);
	Entity*& sun = m_celestialObjects[NUM_PLANETS];

	for (int planetIndex = 0; planetIndex < NUM_PLANETS; planetIndex++)
	{
		float planetInitialOrientationRelativeToSunDegrees = g_RNG->RollRandomFloatInRange(0.f, 360.f);
		Vec2 planetDirFromSun = Vec2::MakeFromPolarDegrees(planetInitialOrientationRelativeToSunDegrees);
		float planetCoordsPolarR = (SUN_POSITION + (planetDirFromSun * (sun->m_cosmeticRadius + PLANET_DISTANCE_FROM_SUN[planetIndex]))).GetLength();
		Vec2 planetPosition = Vec2::MakeFromPolarDegrees(planetInitialOrientationRelativeToSunDegrees, planetCoordsPolarR);
		m_celestialObjects[planetIndex] = new Planet(this, planetPosition, (PlanetName)planetIndex);
	}
	
	LightSource sunLightSource = LightSource(Rgba8(255, 255, 255, 255), SUN_POSITION, 1.f);
	m_sunLightSource = sunLightSource;
}

void Game::RenderCelestialObjects() const
{
	RenderEntitiesInList(NUM_PLANETS + 1, m_celestialObjects);
}

void Game::ShowMenu()
{
	 if (!m_isSavedGame)
	 {
		m_selectedMenuItem = 1;
	 }

	 m_isShowingMenu = true;
}

void Game::RenderMenu() const
{
	std::vector<Vertex_PCU> titleTextVerts;
	AddVertsForTextTriangles2D(titleTextVerts, "Starship Gold", Vec2(200.f, SCREEN_Y - 200.f), 80.f, Rgba8(255, 215, 0, 255));
	g_renderer->BindTexture(nullptr);
	g_renderer->DrawVertexArray((int)titleTextVerts.size(), titleTextVerts.data());


	for (int menuItemIndex = 0; menuItemIndex < NUM_MENU_ITEMS; menuItemIndex++)
	{
		std::vector<Vertex_PCU> menuItemVerts;
		Rgba8 menuItemColor(255, 255, 255, 255);
		if (menuItemIndex == 0 && !m_isSavedGame)
		{
			menuItemColor = Rgba8(255, 255, 255, 55);
		}
		if (menuItemIndex == m_selectedMenuItem)
		{
			menuItemColor = Rgba8(102, 153, 204, 255);
		}

		AddVertsForTextTriangles2D(menuItemVerts, m_menuList[menuItemIndex], Vec2(200.f, SCREEN_Y - 500.f - (100.f * menuItemIndex)), 50.f, menuItemColor);
		g_renderer->BindTexture(nullptr);
		g_renderer->DrawVertexArray((int)menuItemVerts.size(), menuItemVerts.data());
	}
}

void Game::UpdateMenu(float deltaSeconds)
{
	UNUSED(deltaSeconds);

	if (g_input->WasKeyJustPressed(KEYCODE_DOWNARROW))
	{
		if (m_selectedMenuItem < NUM_MENU_ITEMS - 1)
		{
			m_selectedMenuItem++;
		}
	}
	if (g_input->WasKeyJustPressed(KEYCODE_UPARROW))
	{
		if (m_selectedMenuItem > 1)
		{
			m_selectedMenuItem--;
		}
		if (m_selectedMenuItem == 1 && m_isSavedGame)
		{
			m_selectedMenuItem--;
		}
	}

	if (g_input->WasKeyJustPressed(KEYCODE_ENTER) || g_input->WasKeyJustPressed(KEYCODE_SPACE))
	{
		switch(m_selectedMenuItem)
		{
			case 0:
			{
				break;
			}
			case 1:
			{
				m_isShowingMenu = false;
				StartGame();
				break;
			}
			case 2:
			{
				g_app->HandleQuitRequested();
				break;
			}
		}
	}
}

void Game::SpawnPickups(Vec2 const& planetCenter, float planetRadius)
{
	int numPickups = g_RNG->RollRandomIntInRange(5, 10);
	for (int pickupIndex = 0; pickupIndex < numPickups; pickupIndex++)
	{
		Vec2 spawnPosition = g_RNG->RollRandomVec2InRadius(planetCenter, planetRadius);
		PickupType pickupType = PickupType(g_RNG->RollRandomIntInRange(0, 1));
		SpawnPickup(spawnPosition, pickupType);
	}
}
