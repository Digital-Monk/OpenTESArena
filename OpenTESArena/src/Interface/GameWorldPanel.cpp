#include <cassert>
#include <iostream>

#include "SDL2\SDL.h"

#include "GameWorldPanel.h"

#include "Button.h"
#include "CharacterPanel.h"
#include "PauseMenuPanel.h"
#include "../Game/GameData.h"
#include "../Game/GameState.h"
#include "../Math/Int2.h"
#include "../Media/Color.h"
#include "../Media/MusicName.h"
#include "../Media/TextureFile.h"
#include "../Media/TextureManager.h"
#include "../Media/TextureName.h"

GameWorldPanel::GameWorldPanel(GameState *gameState)
	: Panel(gameState)
{
	this->characterSheetButton = nullptr;
	this->pauseButton = nullptr;

	this->characterSheetButton = [gameState]()
	{
		auto function = [gameState]()
		{
			auto sheetPanel = std::unique_ptr<Panel>(new CharacterPanel(gameState));
			gameState->setPanel(std::move(sheetPanel));
		};
		return std::unique_ptr<Button>(new Button(function));
	}();

	this->pauseButton = [gameState]()
	{
		auto function = [gameState]()
		{
			auto pausePanel = std::unique_ptr<Panel>(new PauseMenuPanel(gameState));
			gameState->setPanel(std::move(pausePanel));
		};
		return std::unique_ptr<Button>(new Button(function));
	}();

	assert(this->characterSheetButton.get() != nullptr);
	assert(this->pauseButton.get() != nullptr);
}

GameWorldPanel::~GameWorldPanel()
{

}

void GameWorldPanel::handleEvents(bool &running)
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		bool applicationExit = (e.type == SDL_QUIT);
		bool resized = (e.type == SDL_WINDOWEVENT) &&
			(e.window.event == SDL_WINDOWEVENT_RESIZED);
		bool escapePressed = (e.type == SDL_KEYDOWN) &&
			(e.key.keysym.sym == SDLK_ESCAPE);

		if (applicationExit)
		{
			running = false;
		}
		if (resized)
		{
			int width = e.window.data1;
			int height = e.window.data2;
			this->getGameState()->resizeWindow(width, height);
		}
		if (escapePressed)
		{
			this->pauseButton->click();
		}
	
		bool leftClick = (e.type == SDL_MOUSEBUTTONDOWN) &&
			(e.button.button == SDL_BUTTON_LEFT);
		bool activateHotkeyPressed = (e.type == SDL_KEYDOWN) && 
			(e.key.keysym.sym == SDLK_e);
		bool sheetHotkeyPressed = (e.type == SDL_KEYDOWN) &&
			(e.key.keysym.sym == SDLK_TAB);

		if (leftClick)
		{
			// Attack...
		}
		if (activateHotkeyPressed) 
		{
			// "Activate" whatever is looked at.
		}
		if (sheetHotkeyPressed)
		{
			// Go to character screen.
			this->characterSheetButton->click();
		}
	}
}

void GameWorldPanel::handleMouse(double dt)
{
	static_cast<void>(dt);
	// Make camera look around.
	// The code for this is already in another project. I just need to bring it over
	// and make a couple changes for having the window grab the mouse.
}

void GameWorldPanel::handleKeyboard(double dt)
{
	static_cast<void>(dt);
	// Listen for WASD, jump, crouch...
}

void GameWorldPanel::tick(double dt, bool &running)
{
	static_cast<void>(dt);
	// Animate the game world by "dt" seconds...
	
	this->handleEvents(running);
}

void GameWorldPanel::render(SDL_Surface *dst, const SDL_Rect *letterbox)
{
	// Clear full screen.
	this->clearScreen(dst);

	const int originalWidth = 320;
	const int originalHeight = 200;

	// Temporary background. The game world doesn't use the letterbox for rendering;
	// just interface objects.
	SDL_FillRect(dst, nullptr, SDL_MapRGB(dst->format, 24, 24, 48));

	// Draw game world (OpenCL rendering, kernel stored in GameData)...

	// Interface objects (stat bars, compass, ...) should snap to the edges of the native
	// screen, not just the letterbox, because right now, when the screen is tall, the 
	// compass is near the middle of the screen (in the way), and the stat bars are much 
	// higher than they should be. I haven't figured out yet what the equation is. I
	// think it requires using the original height and the draw scale somehow.

	// Draw stat bars.
	auto statBarSurface = Surface(5, 35);
	
	statBarSurface.fill(Color(0, 255, 0));
	this->drawScaledToNative(statBarSurface, 
		5, 
		160, 
		statBarSurface.getWidth(), 
		statBarSurface.getHeight(),
		dst);

	statBarSurface.fill(Color(255, 0, 0));
	this->drawScaledToNative(statBarSurface, 
		13, 
		160, 
		statBarSurface.getWidth(),
		statBarSurface.getHeight(), 
		dst);

	statBarSurface.fill(Color(0, 0, 255));
	this->drawScaledToNative(statBarSurface,
		21,
		160,
		statBarSurface.getWidth(),
		statBarSurface.getHeight(),
		dst);
	
	// Draw compass.
	const auto &compassFrame = this->getGameState()->getTextureManager()
		.getSurface(TextureFile::fromName(TextureName::CompassFrame));
	SDL_SetColorKey(compassFrame.getSurface(), SDL_TRUE, this->getMagenta(dst->format));
	
	this->drawScaledToNative(compassFrame,
		(originalWidth / 2) - (compassFrame.getWidth() / 2),
		0,
		compassFrame.getWidth(),
		compassFrame.getHeight(),
		dst);

	// Draw cursor for now. It won't be drawn once the game world is developed enough.
	const auto &cursor = this->getGameState()->getTextureManager()
		.getSurface(TextureFile::fromName(TextureName::SwordCursor));
	this->drawCursor(cursor, dst);
}