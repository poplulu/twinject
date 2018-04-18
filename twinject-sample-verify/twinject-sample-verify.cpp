// twinject-sample-verify.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SDL.h"

#include <iostream>
#include <fstream>
#include <vector>

union th_kbd_state
{
	struct {
		bool shot;		// 0
		bool bomb;		// 1
		bool slow;		// 2
		bool skip;		// 3
		bool up;		// 4
		bool left;		// 5
		bool down;		// 6
		bool right;		// 7
	};
	bool keys[8];
};

struct vec2
{
	float x, y;
};

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;
const int SAMPLE_SIZE = 64;

struct sample
{
	// relative locations to players of critical points
	vec2 critpt[SAMPLE_SIZE];
	// velocities of critical points
	vec2 ptvel[SAMPLE_SIZE];
	// key presses
	th_kbd_state keys;
};

SDL_Window* SDLWindow = NULL;
SDL_Surface* SDLSurface = NULL;
SDL_Renderer* SDLRenderer = NULL;

bool CSDL_Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	SDLWindow = SDL_CreateWindow("twinject-sample-verify", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (SDLWindow == NULL)
	{
		printf("SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	SDLSurface = SDL_GetWindowSurface(SDLWindow);
	if (SDLSurface == NULL)
	{
		printf("SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	SDLRenderer = SDL_CreateRenderer(SDLWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (SDLRenderer == NULL)
	{
		printf("SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	return true;
}

void CSDL_Destroy()
{
	SDL_DestroyRenderer(SDLRenderer);
	SDL_FreeSurface(SDLSurface);
	SDL_DestroyWindow(SDLWindow);
	SDL_Quit();
}

int main(int argc, char* argv[])
{
	if (!CSDL_Init())
		return 1;
	std::cout << "twinject-sample-verify" << std::endl;

	std::ifstream ifs("D:\\Programming\\Multi\\th15\\nn_response.samp", std::ios::in | std::ios::binary);
	if (ifs.fail())
	{
		std::cerr << "file not found" << std::endl;
		return 1;
	}

	std::vector<sample> samples;
	sample samp;
	while (ifs.read((char*)&samp, sizeof(sample)))	// this is also disgusting
	{
		samples.push_back(samp);
	}
	std::cout << "loaded " << samples.size() << " samples" << std::endl;

	std::vector<sample>::const_iterator s = samples.cbegin();

	SDL_Rect rects[SAMPLE_SIZE] = { 0 };
	SDL_Rect center = { WINDOW_WIDTH / 2 - 2, WINDOW_HEIGHT / 2 - 2,4,4 };

	bool quit = false;
	SDL_Event e;
	while (!quit && s != samples.cend())
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
		}
		SDL_SetRenderDrawColor(SDLRenderer, 0, 0, 0, 0xFF);
		SDL_RenderClear(SDLRenderer);

		SDL_SetRenderDrawColor(SDLRenderer, 0, 255, 0, 0xFF);
		SDL_RenderFillRect(SDLRenderer, &center);

		SDL_SetRenderDrawColor(SDLRenderer, 0, 0, 255, 0xFF);
		for (int i = 0; i < SAMPLE_SIZE; ++i)
		{
			int x = (int)s->critpt[i].x;
			int y = (int)s->critpt[i].y;
				if (x == 0 && y == 0)
					continue;
			rects[i].x = WINDOW_WIDTH / 2 + x - 2;
			rects[i].y = WINDOW_HEIGHT / 2 + y - 2;
			rects[i].w = 4;
			rects[i].h = 4;
			SDL_RenderDrawLine(SDLRenderer, 
				WINDOW_WIDTH / 2 + x, WINDOW_HEIGHT / 2 + y,
				WINDOW_WIDTH / 2 + x + (int)(s->ptvel[i].x * 10), 
				WINDOW_HEIGHT / 2 + y + (int)(s->ptvel[i].y * 10));
		}
		SDL_SetRenderDrawColor(SDLRenderer, 255, 0, 0, 0xFF);
		SDL_RenderFillRects(SDLRenderer, rects, SAMPLE_SIZE);

		SDL_RenderPresent(SDLRenderer);
		++s;
	}

	CSDL_Destroy();
	return 0;
}

