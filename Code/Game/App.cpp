#include "Game/App.hpp"

#include "Game/GameCommon.hpp"

#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Window.hpp"


App* g_app = nullptr;
AudioSystem* g_audio = nullptr;
RandomNumberGenerator* g_RNG = nullptr;
Renderer* g_renderer = nullptr;
Window* g_window = nullptr;


bool App::HandleQuitRequested(EventArgs& args)
{
	bool help = args.GetValue("help", false);
	if (help)
	{
		g_console->AddLine("Quits the application. No parameters.", false);
		return true;
	}

	g_app->HandleQuitRequested();
	return true;
}

bool App::DisplayControls(EventArgs& args)
{
	UNUSED(args);

	// Add controls to DevConsole
	g_console->AddLine(Rgba8::STEEL_BLUE, "App-level controls", false);
	g_console->AddLine(Rgba8::MAGENTA, Stringf("%-20s : Deletes the game and creates a new one", "F8"), false);
	g_console->AddLine(Rgba8::MAGENTA, Stringf("%-20s : Opens the console", "`"), false);
	g_console->AddLine(Rgba8::STEEL_BLUE, "Attract Screen Controls", false);
	g_console->AddLine(Rgba8::MAGENTA, Stringf("%-20s : Play a test sound", "Y"), false);
	g_console->AddLine(Rgba8::MAGENTA, Stringf("%-20s : Go to game screen", "Space"), false);
	g_console->AddLine(Rgba8::MAGENTA, Stringf("%-20s : Exits the application", "Escape"), false);
	g_console->AddLine(Rgba8::STEEL_BLUE, "Game Screen Controls", false);
	g_console->AddLine(Rgba8::MAGENTA, Stringf("%-20s : Returns to attract screen", "Escape"), false);
	g_console->AddLine(Rgba8::MAGENTA, Stringf("%-20s : Pauses/Resumes the game", "P"), false);
	g_console->AddLine(Rgba8::MAGENTA, Stringf("%-20s : Runs the game at timeScale = 0.1f", "T (hold)"), false);
	g_console->AddLine(Rgba8::MAGENTA, Stringf("%-20s : Runs a single frame (and pauses if not already paused)", "O (hold)"), false);
	g_console->AddLine(Rgba8::MAGENTA, Stringf("%-20s : Toggles debug draw", "F1"), false);
	g_console->AddLine(Rgba8::MAGENTA, Stringf("%-20s : Accelerates ship in the ship's forward direction", "E"), false);
	g_console->AddLine(Rgba8::MAGENTA, Stringf("%-20s : Rotate ship clockwise/counter-clockwise", "F/S"), false);
	g_console->AddLine(Rgba8::MAGENTA, Stringf("%-20s : Deccelerates ship", "D"), false);
	g_console->AddLine(Rgba8::MAGENTA, Stringf("%-20s : Fires a bullet", "Spacebar"), false);
	g_console->AddLine(Rgba8::MAGENTA, Stringf("%-20s : Returns ship to planet Earth", "H"), false);
	g_console->AddLine(Rgba8::MAGENTA, Stringf("%-20s : Starts a quest (when prompted)", "Q"), false);
	g_console->AddLine(Rgba8::MAGENTA, Stringf("%-20s : Lands/Takes-off ships to/from planet (landing only possible when ship is near a planet)", "R"), false);
	g_console->AddLine(Rgba8::MAGENTA, Stringf("%-20s : Spawns an asteroid just off screen", "I"), false);
	g_console->AddLine(Rgba8::MAGENTA, Stringf("%-20s : Lands/Takes-off ships to/from planet (landing only possible when ship is near a planet)", "R"), false);

	return true;
}

App::App()
{
	
}

App::~App()
{
	delete g_renderer;
	g_renderer = nullptr;

	delete g_input;
	g_input = nullptr;

	delete g_audio;
	g_audio = nullptr;

	delete g_RNG;
	g_RNG = nullptr;

	delete m_game;
	m_game = nullptr;
}

void App::Startup()
{
	EventSystemConfig eventSystemConfig;
	g_eventSystem = new EventSystem(eventSystemConfig);

	InputConfig inputConfig;
	g_input = new InputSystem(inputConfig);

	WindowConfig windowConfig;
	windowConfig.m_inputSystem = g_input;
	windowConfig.m_windowTitle = "SD1-A4: Starship Gold";
	windowConfig.m_clientAspect = 2.f;
	g_window = new Window(windowConfig);

	RenderConfig renderConfig;
	renderConfig.m_window = g_window;
	g_renderer = new Renderer(renderConfig);

	DevConsoleConfig devConsoleConfig;
	devConsoleConfig.m_renderer = g_renderer;
	Camera devConsoleCamera = Camera();
	devConsoleCamera.SetOrthoView(Vec2::ZERO, Vec2(2.f, 1.f));
	devConsoleConfig.m_camera = devConsoleCamera;
	devConsoleConfig.m_consoleFontFilePathWithNoExtension = "Data/Images/SquirrelFixedFont";
	g_console = new DevConsole(devConsoleConfig);

	AudioConfig audioConfig;
	g_audio = new AudioSystem(audioConfig);

	DebugRenderConfig debugRenderConfig;
	debugRenderConfig.m_bitmapFontFilePathWithNoExtension = "Data/Images/SquirrelFixedFont";
	debugRenderConfig.m_renderer = g_renderer;

	g_eventSystem->Startup();
	g_console->Startup();
	g_input->Startup();
	g_window->Startup();
	g_renderer->Startup();
	g_audio->Startup();
	DebugRenderSystemStartup(debugRenderConfig);

	m_game = new Game();

	SubscribeEventCallbackFunction("Quit", HandleQuitRequested, "Exits the application");
	SubscribeEventCallbackFunction("Controls", DisplayControls, "Displays game controls to the console");
	EventArgs emptyArgs;
	App::DisplayControls(emptyArgs);
}

void App::Run()
{
	while (!IsQuitting())
	{
		RunFrame();
	}
}

void App::RunFrame()
{
	BeginFrame();
	Update();
	Render();
	EndFrame();

}

bool App::HandleQuitRequested()
{
	m_isQuitting = true;

	return true;
}

void App::BeginFrame()
{
	g_eventSystem->BeginFrame();
	g_console->BeginFrame();
	g_input->BeginFrame();
	g_window->BeginFrame();
	g_renderer->BeginFrame();
	g_audio->BeginFrame();
	DebugRenderBeginFrame();

	Clock::TickSystemClock();
}

void App::Update()
{
	m_game->Update();

	if (g_input->WasKeyJustPressed(KEYCODE_F8))
	{
		delete m_game;
		m_game = new Game();
	}
	if (g_input->WasKeyJustPressed(KEYCODE_TILDE))
	{
		g_console->ToggleMode(DevConsoleMode::OPENFULL);
	}
}

void App::Render() const
{
	g_renderer->ClearScreen(Rgba8(0, 0, 0, 255));

	// Globally set renderer states so that there's no culling or depth
	g_renderer->SetModelConstants();
	g_renderer->SetBlendMode(BlendMode::ALPHA);
	g_renderer->SetDepthMode(DepthMode::DISABLED);
	g_renderer->SetRasterizerCullMode(RasterizerCullMode::CULL_NONE);
	g_renderer->SetRasterizerFillMode(RasterizerFillMode::SOLID);
	g_renderer->SetSamplerMode(SamplerMode::POINT_CLAMP);

	m_game->Render();

	g_console->Render(AABB2(Vec2::ZERO, Vec2(Game::SCREEN_X, Game::SCREEN_Y)));
}

void App::EndFrame()
{
	DebugRenderEndFrame();
	g_audio->EndFrame();
	g_renderer->EndFrame();
	g_window->EndFrame();
	g_input->EndFrame();
	g_console->EndFrame();
	g_eventSystem->EndFrame();
}

void App::Shutdown()
{
	DebugRenderSystemShutdown();
	g_audio->Shutdown();
	g_renderer->Shutdown();
	g_window->Shutdown();
	g_input->Shutdown();
	g_console->Shutdown();
	g_eventSystem->Shutdown();
}

