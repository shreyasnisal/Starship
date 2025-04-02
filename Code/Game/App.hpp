#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Camera.hpp"

#include "Game/Game.hpp"

class App
{
public:
						App							();
						~App						();
	void				Startup						();
	void				Shutdown					();
	void				Run							();
	void				RunFrame					();

	bool				IsQuitting					() const		{ return m_isQuitting; }
	bool				HandleQuitRequested			();
	static bool			HandleQuitRequested(EventArgs& args);
	static bool			DisplayControls(EventArgs& args);

public:
	Game*				m_game;

private:
	void				BeginFrame					();
	void				Update						();
	void				Render						() const;
	void				EndFrame					();

private:
	bool				m_isQuitting				= false;
	double				m_previousFrameTime			= 0;
};
