#include "Game/PlayerShip.hpp"

#include <math.h>

#include "Engine/Input/XboxController.hpp"

#include "Game/App.hpp"
#include "Game/GameCommon.hpp"

PlayerShip::PlayerShip(Game* game, Vec2 const& startPosition) : Entity(game, startPosition)
{
	m_entityType = EntityType::PLAYERSHIP;
	m_health = PLAYERSHIP_MAX_HEALTH;
	m_physicsRadius = PLAYERSHIP_PHYSICS_RADIUS;
	m_cosmeticRadius = PLAYERSHIP_COSMETIC_RADIUS;
	m_color = PLAYERSHIP_COLOR;
	InitializeVertexes();
	m_entityID = 1; // PlayerShip is always entityID 1

	m_shootSound = g_audio->CreateOrGetSound("Data/Audio/Shoot_Bullet.wav");
	m_dieSound = g_audio->CreateOrGetSound("Data/Audio/PlayerShip_Die.wav");
}

void PlayerShip::InitializeVertexes()
{
	for (int vertexIndex = 0; vertexIndex < PLAYERSHIP_NUM_VERTEXES; vertexIndex++)
	{
		m_vertexes[vertexIndex] = PLAYERSHIP_VERTEXES[vertexIndex];
	}
}

void PlayerShip::Update(float deltaSeconds)
{
	if (deltaSeconds > 0.f && !m_game->m_isLandingOnPlanet)
	{
		UpdateFromController(deltaSeconds);
		if (m_game->m_isAIEnabled)
		{
			UpdateFromAI(deltaSeconds);
		}
		UpdateFromKeyboard(deltaSeconds);
	}

	if (!m_isDead)
	{
		Vec2 acceleration = Vec2(0.f, 0.f);

		if (!m_isOnPlanet)
		{
			acceleration = m_thrustFraction * PLAYERSHIP_THRUST * GetForwardNormal();
			m_velocity += acceleration * deltaSeconds;
			m_velocity.SetLength(GetClamped(m_velocity.GetLength(), 0.f, PLAYERSHIP_MAX_SPEED));
			m_position += m_velocity * deltaSeconds;
		}
		else
		{
			m_velocity = Vec2::MakeFromPolarDegrees(m_orientationDegrees, m_thrustFraction * 2.f);
			m_position += m_velocity * deltaSeconds;
			PushDiscIntoFixedDisc2D(m_position, m_cosmeticRadius, m_landedPlanet->m_position, m_landedPlanet->m_cosmeticRadius);
		}

		if (m_health == 0)
		{
			Die();
		}

		if (m_thrustFraction > 0.f && deltaSeconds > 0.f)
		{
			SpawnExhaustDebris();
			m_tailLength = 5.f * g_RNG->RollRandomFloatInRange(0.f, m_thrustFraction);
		}

		if (m_isHyperSpeedModeOn)
		{
			m_tailLength = 5.f * g_RNG->RollRandomFloatInRange(0.f, 1.f);
			HyperSpeedTowardsEarth(deltaSeconds);
		}
	}
	else if (m_game->m_isAIEnabled)
	{
		m_aiRespawnDelay -= deltaSeconds;
		if (m_aiRespawnDelay <= 0.f)
		{
			m_aiRespawnDelay = AI_RESPAWN_DELAY;
			ResetAndRespawn(Vec2(m_game->WORLD_X, m_game->WORLD_Y) * 0.5f);
		}
	}
}


void PlayerShip::UpdateFromKeyboard(float& deltaSeconds)
{
	if (!m_isDead)
	{
		if (g_input->WasKeyJustPressed('Q') && !m_isOnPlanet)
		{
			m_game->StartQuest();
		}
		if (g_input->WasKeyJustPressed('R'))
		{
			if (!m_isOnPlanet && m_game->m_questManager.GetGameState() == GAMESTATE_EXPLORATION)
			{
				m_game->LandOnPlanet();
			}
			else
			{
				m_game->TakeOffFromPlanet();
			}
		}

		if (g_input->IsKeyDown('E'))
		{
			m_thrustFraction = 1.f;
			m_game->m_isAIEnabled = false;
		}
		if (g_input->WasKeyJustReleased('E'))
		{
			m_thrustFraction = 0.f;
			m_game->m_isAIEnabled = false;
		}

		if (g_input->IsKeyDown('D'))
		{
			m_velocity -= PLAYERSHIP_THRUST * deltaSeconds * m_velocity.GetNormalized();
			m_game->m_isAIEnabled = false;
		}

		if (g_input->IsKeyDown('S'))
		{
			m_orientationDegrees += PLAYERSHIP_ANGULAR_SPEED * deltaSeconds;
			m_game->m_isAIEnabled = false;
		}
		if (g_input->IsKeyDown('F'))
		{
			m_orientationDegrees += -PLAYERSHIP_ANGULAR_SPEED * deltaSeconds;
			m_game->m_isAIEnabled = false;
		}
		if (g_input->WasKeyJustPressed(' ') && m_numBulletsRemaining > 0 && !m_isOnPlanet)
		{
			float bulletInaccuracy = g_RNG->RollRandomFloatInRange(-m_bulletInaccuracy, m_bulletInaccuracy);
			m_game->SpawnNewBullet(m_position + GetForwardNormal() * 8.f, m_orientationDegrees + GetClamped(bulletInaccuracy, -2.f, 2.f), m_color, m_entityID);
			m_numBulletsRemaining--;
			m_game->m_isAIEnabled = false;
			m_bulletInaccuracy += 2.f * deltaSeconds;

			g_audio->StartSound(m_shootSound);
		}
		else
		{
			m_bulletInaccuracy -= 1.f * deltaSeconds;
		}
		if (g_input->WasKeyJustPressed('H') && m_game->m_questManager.GetGameState() == GAMESTATE_EXPLORATION)
		{
			m_thrustFraction = 0;
			m_velocity = Vec2(0.f, 0.f);
			m_isHyperSpeedModeOn = true;
		}
	}
	else
	{
		if (g_input->WasKeyJustPressed('N'))
		{
			if (m_numExtraLivesRemaining > 0)
			{
				ResetAndRespawn(Vec2(m_game->WORLD_X, m_game->WORLD_Y) * 0.5f);
			}
		}
	}
}

void PlayerShip::UpdateLandingOnPlanet(float timeUntilLanding)
{
	static Vertex_PCU s_tempVertexes[NUM_VERTEXES];
	for (int vertexIndex = 0; vertexIndex < NUM_VERTEXES; vertexIndex++)
	{
		s_tempVertexes[vertexIndex] = m_vertexes[vertexIndex];
	}

	if (timeUntilLanding > 1.f)
	{
		return;
	}

	for (int vertexIndex = 0; vertexIndex < NUM_VERTEXES; vertexIndex++)
	{
		m_vertexes[vertexIndex].m_position.x = Interpolate(s_tempVertexes[vertexIndex].m_position.x, PLAYERSHIP_LAND_VERTEXES[vertexIndex].m_position.x, 1.f - timeUntilLanding);
		m_vertexes[vertexIndex].m_position.y = Interpolate(s_tempVertexes[vertexIndex].m_position.y, PLAYERSHIP_LAND_VERTEXES[vertexIndex].m_position.y, 1.f - timeUntilLanding);
		m_vertexes[vertexIndex].m_color = Interpolate(s_tempVertexes[vertexIndex].m_color, PLAYERSHIP_LAND_VERTEXES[vertexIndex].m_color, 1.f - timeUntilLanding);
	}
}

void PlayerShip::UpdateTakingOffFromPlanet(float timeUntilLanding)
{
	static Vertex_PCU s_tempVertexes[NUM_VERTEXES];
	for (int vertexIndex = 0; vertexIndex < NUM_VERTEXES; vertexIndex++)
	{
		s_tempVertexes[vertexIndex] = m_vertexes[vertexIndex];
	}

	if (timeUntilLanding > 1.f)
	{
		return;
	}

	for (int vertexIndex = 0; vertexIndex < NUM_VERTEXES; vertexIndex++)
	{
		m_vertexes[vertexIndex].m_position.x = Interpolate(s_tempVertexes[vertexIndex].m_position.x, PLAYERSHIP_VERTEXES[vertexIndex].m_position.x, 1.f - timeUntilLanding);
		m_vertexes[vertexIndex].m_position.y = Interpolate(s_tempVertexes[vertexIndex].m_position.y, PLAYERSHIP_VERTEXES[vertexIndex].m_position.y, 1.f - timeUntilLanding);
		m_vertexes[vertexIndex].m_color = Interpolate(s_tempVertexes[vertexIndex].m_color, PLAYERSHIP_VERTEXES[vertexIndex].m_color, 1.f - timeUntilLanding);
	}
}

void PlayerShip::HyperSpeedTowardsEarth(float deltaSeconds)
{
	Entity* planetEarth = m_game->m_celestialObjects[(int)PLANET_EARTH];

	if (GetDistance2D(m_position, planetEarth->m_position) > 2.f * planetEarth->m_cosmeticRadius)
	{
		m_orientationDegrees = (planetEarth->m_position - m_position).GetOrientationDegrees();
		m_position.x = Interpolate(m_position.x, planetEarth->m_position.x, 0.99f * deltaSeconds);
		m_position.y = Interpolate(m_position.y, planetEarth->m_position.y, 0.99f * deltaSeconds);
	}
	else
	{
		m_isHyperSpeedModeOn = false;
	}
}


void PlayerShip::UpdateFromController(float& deltaSeconds)
{
	UNUSED(deltaSeconds);

	XboxController const& controller = g_input->GetController(0);

	if (!m_isDead)
	{

		if (controller.WasButtonJustPressed(XBOX_BUTTON_A))
		{
			m_game->SpawnNewBullet(m_position + GetForwardNormal(), m_orientationDegrees, m_color, m_entityID);
		}

		float leftStickMagnitude = controller.GetLeftStick().GetMagnitude();
		if (leftStickMagnitude > 0.f)
		{
			m_thrustFraction = leftStickMagnitude;
			m_orientationDegrees = controller.GetLeftStick().GetOrientationDegrees();
		}
		else
		{
			m_thrustFraction = 0.f;
		}
	}
	else
	{
		if (controller.WasButtonJustPressed(XBOX_BUTTON_START))
		{
			if (m_numExtraLivesRemaining > 0)
			{
				ResetAndRespawn(Vec2(m_game->WORLD_X, m_game->WORLD_Y) * 0.5f);
			}
		}
	}
}


// AI Control for PlayerShip- not great
// Shoots well, doesn't move very well
void PlayerShip::UpdateFromAI(float deltaSeconds)
{
	if (!m_isDead)
	{
		if (m_velocity.GetLength() >= AI_MAX_SPEED)
		{
			m_orientationDegrees -= GetTurnedTowardDegrees(m_orientationDegrees, m_velocity.GetOrientationDegrees(), PLAYERSHIP_ANGULAR_SPEED * deltaSeconds);
			m_thrustFraction = 1.f;
			return;
		}

		Entity* nearestEnemy = GetNearestEnemy();

		if (!nearestEnemy)
		{
			return;
		}

		float distanceToNearestEnemy = GetDistance2D(m_position, nearestEnemy->m_position);

		if (distanceToNearestEnemy < AI_ENEMY_DISTANCE_FOR_RUN)
		{
			float orientationDegreesToNearestEnemy = (nearestEnemy->m_position - m_position).GetOrientationDegrees();
			float rotationDegreesAwayFromNearestEnemy = fabsf(GetShortestAngularDispDegrees(m_orientationDegrees, 180.f - orientationDegreesToNearestEnemy));

			if (rotationDegreesAwayFromNearestEnemy > AI_MIN_AWAY_FROM_ENEMY_ROTATION_DEGREES_FOR_RUN)
			{
				m_orientationDegrees += PLAYERSHIP_ANGULAR_SPEED * deltaSeconds;
			}
			else
			{
				m_thrustFraction = 1.f;
			}
			return;
		}

		float bulletTime = distanceToNearestEnemy / BULLET_SPEED;

		float orientationDegreesToNearestEnemy = (nearestEnemy->m_position + nearestEnemy->m_velocity * bulletTime - m_position).GetOrientationDegrees();
		float rotationDegreesToNearestEnemy = GetShortestAngularDispDegrees(m_orientationDegrees, orientationDegreesToNearestEnemy);

		if (rotationDegreesToNearestEnemy > AI_SHOOT_MIN_ANGE_DEGREES)
		{
			m_orientationDegrees += PLAYERSHIP_ANGULAR_SPEED * deltaSeconds;
		}
		else if (rotationDegreesToNearestEnemy < -AI_SHOOT_MIN_ANGE_DEGREES)
		{
			m_orientationDegrees += -PLAYERSHIP_ANGULAR_SPEED * deltaSeconds;
		}
		else
		{
			if (bulletTime > BULLET_LIFESPAN)
			{
				m_thrustFraction = 1.f;
			}

			m_aiShootDelay -= deltaSeconds;
			if (m_aiShootDelay < 0.f)
			{
				m_game->SpawnNewBullet(m_position + GetForwardNormal(), m_orientationDegrees, m_color, m_entityID);
				m_aiShootDelay = AI_SHOOT_DELAY;
			}
		}
	}
}

Entity* PlayerShip::GetNearestEnemy()
{
	Entity* nearestAsteroid =	GetNearestEnemyInList		(MAX_ASTEROIDS,		m_game->m_asteroids);
	Entity* nearestBeetle =		GetNearestEnemyInList		(MAX_BEETLES,		m_game->m_beetles);
	Entity* nearestWasp =		GetNearestEnemyInList		(MAX_WASPS,			m_game->m_wasps);

	float distanceToNearestAsteroid = (float)INT_MAX;
	float distanceToNearestBeetle = (float)INT_MAX;
	float distanceToNearestWasp = (float)INT_MAX;

	if (nearestAsteroid)
	{
		distanceToNearestAsteroid = GetDistance2D(m_position, nearestAsteroid->m_position);
	}
	if (nearestBeetle)
	{
		distanceToNearestBeetle = GetDistance2D(m_position, nearestBeetle->m_position);
	}
	if (nearestWasp)
	{
		distanceToNearestWasp = GetDistance2D(m_position, nearestWasp->m_position) - AI_WASP_DISTANCE_ADVNATAGE;
	}

	if (distanceToNearestAsteroid < distanceToNearestBeetle && distanceToNearestAsteroid < distanceToNearestWasp)
	{
		return nearestAsteroid;
	}
	else if (distanceToNearestBeetle < distanceToNearestWasp)
	{
		return nearestBeetle;
	}
	else
	{
		return nearestWasp;
	}
}

Entity* PlayerShip::GetNearestEnemyInList(int numEnemies, Entity** enemies)
{
	float minDistance = (float)INT_MAX;
	Entity* nearestEnemy = nullptr;
	for (int enemyIndex = 0; enemyIndex < numEnemies; enemyIndex++)
	{
		if (m_game->IsEntityAlive(enemies[enemyIndex]) && !enemies[enemyIndex]->IsOffScreen())
		{
			float enemyDistance = GetDistance2D(enemies[enemyIndex]->m_position + enemies[enemyIndex]->m_velocity, m_position);
			if (enemyDistance < minDistance)
			{
				nearestEnemy = enemies[enemyIndex];
				minDistance = enemyDistance;
			}
		}
	}

	return nearestEnemy;
}


void PlayerShip::CheckCollisionsWithWalls()
{
	// Left wall
	if (m_position.x < 0.f + m_physicsRadius)
	{
		m_position.x = m_physicsRadius;
		m_velocity.x = -m_velocity.x;
	}

	// Right Wall
	if (m_position.x > m_game->WORLD_X - m_physicsRadius)
	{
		m_position.x = m_game->WORLD_X - m_physicsRadius;
		m_velocity.x = -m_velocity.x;
	}

	// Bottom Wall
	if (m_position.y < 0.f + m_physicsRadius)
	{
		m_position.y = m_physicsRadius;
		m_velocity.y = -m_velocity.y;
	}

	// Top Wall
	if (m_position.y > m_game->WORLD_Y - m_physicsRadius)
	{
		m_position.y = m_game->WORLD_Y - m_physicsRadius;
		m_velocity.y = -m_velocity.y;
	}
}

void PlayerShip::Render() const
{
	if (!m_isDead)
	{
		Vertex_PCU shipVerts[NUM_VERTEXES];
		for (int vertexIndex = 0; vertexIndex < NUM_VERTEXES; vertexIndex++)
		{
			shipVerts[vertexIndex] = m_vertexes[vertexIndex];
		}

		TransformVertexArrayXY3D(NUM_VERTEXES, shipVerts, m_scale, m_orientationDegrees, m_position);
		g_renderer->BindTexture(nullptr);
		g_renderer->DrawVertexArray(NUM_VERTEXES, shipVerts);


		if (m_thrustFraction > 0.f || m_isHyperSpeedModeOn)
		{
			Vertex_PCU shipTailVerts[PLAYERSHIP_NUM_TAIL_VERTEXES];
			for (int vertexIndex = 0; vertexIndex < PLAYERSHIP_NUM_TAIL_VERTEXES; vertexIndex++)
			{
				shipTailVerts[vertexIndex] = PLAYERSHIP_TAIL_VERTEXES[vertexIndex];

				if (vertexIndex == PLAYERSHIP_NUM_TAIL_VERTEXES - 1)
				{
					shipTailVerts[vertexIndex].m_position.x -= m_tailLength;
					shipTailVerts[vertexIndex].m_color.a = 0;
				}

				if (m_isHyperSpeedModeOn)
				{
					shipTailVerts[vertexIndex].m_color = m_hyperSpeedTailColor;
				}
			}

			TransformVertexArrayXY3D(PLAYERSHIP_NUM_TAIL_VERTEXES, shipTailVerts, m_scale, m_orientationDegrees, m_position);
			g_renderer->BindTexture(nullptr);
			g_renderer->DrawVertexArray(PLAYERSHIP_NUM_TAIL_VERTEXES, shipTailVerts);
		}

		RenderHealthBar();
	}

	if (m_game->m_drawDebug)
	{
		RenderDebug();
	}
}

void PlayerShip::RenderHealthBar() const
{
	if (m_game->m_isLandingOnPlanet)
	{
		return;
	}

	Vec2 healthBarBottomLeft(m_position.x - m_scale * HEALTH_INDICATOR_BOX_WIDTH * 0.5f, m_position.y + m_scale * HEALTH_INDICATOR_Y_OFFSET);
	Vec2 healthBarTopRight(m_position.x + m_scale * HEALTH_INDICATOR_BOX_WIDTH * 0.5f, m_position.y + m_scale * HEALTH_INDICATOR_Y_OFFSET + m_scale * HEALTH_INDICATOR_BOX_HEIGHT);

	AABB2 healthBarOutline(healthBarBottomLeft, healthBarTopRight);
	DrawBoxOutline(healthBarOutline, m_scale * 0.2f, Rgba8(200, 200, 200, 255));

	DebugDrawLine
	(
		Vec2(healthBarBottomLeft.x, healthBarBottomLeft.y + healthBarOutline.GetDimensions().y * 0.5f),
		Vec2(healthBarTopRight.x, healthBarBottomLeft.y + healthBarOutline.GetDimensions().y * 0.5f),
		m_scale * HEALTH_INDICATOR_BOX_HEIGHT,
		Rgba8(250, 50, 50, 255)
	);

	DebugDrawLine
	(
		Vec2(healthBarBottomLeft.x, healthBarBottomLeft.y + healthBarOutline.GetDimensions().y * 0.5f),
		Vec2(healthBarBottomLeft.x + RangeMap((float)m_health, 0.f, (float)PLAYERSHIP_MAX_HEALTH, 0.f, healthBarOutline.GetDimensions().x), healthBarBottomLeft.y + healthBarOutline.GetDimensions().y * 0.5f),
		m_scale * HEALTH_INDICATOR_BOX_HEIGHT,
		Rgba8(50, 250, 50, 255)
	);
}

void PlayerShip::RenderLowHealthIndicator() const
{
	if (m_health < PLAYERSHIP_MAX_HEALTH / 10.f)
	{
		DebugDrawGlow(Vec2(0.f, 0.f), 1000.f, Rgba8(255, 0, 0, 40), Rgba8(255, 0, 0, 0));
		DebugDrawGlow(Vec2(m_game->SCREEN_X, 0.f), 1000.f, Rgba8(255, 0, 0, 40), Rgba8(255, 0, 0, 0));
		DebugDrawGlow(Vec2(m_game->SCREEN_X, m_game->SCREEN_Y), 1000.f, Rgba8(255, 0, 0, 40), Rgba8(255, 0, 0, 0));
		DebugDrawGlow(Vec2(0.f, m_game->SCREEN_Y), 1000.f, Rgba8(255, 0, 0, 40), Rgba8(255, 0, 0, 0));
	}
}

void PlayerShip::Die()
{
	g_audio->StartSound(m_dieSound);
	m_isDead = true;
	SpawnDebrisCluster(MIN_DEBRIS_PER_PLAYERSHIP, MAX_DEBRIS_PER_PLAYERSHIP);
	m_game->ShakeCamera(1.f);
}

void PlayerShip::ResetAndRespawn(Vec2 const& resetPosition)
{
	m_velocity = Vec2(0.f, 0.f);
	m_angularVelocity = 0.f;
	m_position = resetPosition;
	m_orientationDegrees = 0.f;
	m_thrustFraction = 0.f;
	m_health = PLAYERSHIP_MAX_HEALTH;
	m_isDead = false;
	m_numExtraLivesRemaining--;
	m_numBulletsRemaining = PLAYERSHIP_MAX_BULLETS;
}

void PlayerShip::SpawnExhaustDebris()
{
	int numDebris = g_RNG->RollRandomIntLessThan((int)(2 * m_thrustFraction));
	for (int debrisIndex = 0; debrisIndex < numDebris; debrisIndex++)
	{
		m_game->SpawnDebris(m_position - GetForwardNormal() * 2.5f * m_scale, -GetForwardNormal() * 100.f, PLAYERSHIP_TAIL_COLOR, 0.4f * m_scale, 0.08f * m_scale);
	}
}