#include <SDL2/SDL.h>
#include "Complex.hpp"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

#define DIVERGENCE_THRESHOLD 10.0
#define MAX_ITERATIONS 25

#define GRAPH_MIN_X -2.5
#define GRAPH_MAX_X 2.5

#define GRAPH_MIN_Y -2.5
#define GRAPH_MAX_Y 2.5

bool isPartOfMandelbrotSet(Complex complex)
{
	Complex current = Complex();
	for (unsigned char i = 0; i < MAX_ITERATIONS; i++)
	{
		current = current * current + complex;
		if (abs(current.real) > DIVERGENCE_THRESHOLD || abs(current.imag) > DIVERGENCE_THRESHOLD)
			return false;
	}
	return true;
}

int main(int argc, char **argv)
{
	SDL_Window *sdlWindow = SDL_CreateWindow("Fractal Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);

	SDL_Event sdlEvent;

	bool isRunning = true;
	while (isRunning)
	{
		// Handle event
		while (SDL_PollEvent(&sdlEvent))
		{
			switch (sdlEvent.type)
			{
			case SDL_QUIT:
				isRunning = false;
			}
		}

		// Update

		// Draw
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		for (int y = 0; y < WINDOW_HEIGHT; y++)
			for (int x = 0; x < WINDOW_WIDTH; x++)
			{
				double real = ((double)x / (double)WINDOW_WIDTH) * (GRAPH_MAX_X - GRAPH_MIN_X) + GRAPH_MIN_X;
				double imag = ((double)y / (double)WINDOW_HEIGHT) * (GRAPH_MAX_Y - GRAPH_MIN_Y) + GRAPH_MIN_Y;
				Complex complex(real, imag);
				if (isPartOfMandelbrotSet(complex))
					SDL_RenderDrawPoint(renderer, x, y);
			}

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(sdlWindow);
	return 0;
}