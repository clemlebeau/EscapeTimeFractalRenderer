#include <SDL2/SDL.h>
#include "Complex.hpp"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

#define DIVERGENCE_THRESHOLD 3.0
#define MAX_ITERATIONS 10

#define ZOOM_FACTOR 0.9

int escapeIterationFromMandelbrotSet(Complex complex)
{
	Complex current = Complex();
	for (unsigned char i = 0; i < MAX_ITERATIONS; i++)
	{
		current = current * current + complex;
		if (current.modulusSquared() > DIVERGENCE_THRESHOLD * DIVERGENCE_THRESHOLD)
			return i;
	}
	return MAX_ITERATIONS;
}

double lerp(double a, double b, double t)
{
	return (b - a) * t + a;
}

SDL_Color lerpColor(const SDL_Color &from, const SDL_Color &to, double t)
{
	return {(unsigned char)lerp(from.r, to.r, t), (unsigned char)lerp(from.g, to.g, t), (unsigned char)lerp(from.b, to.b, t), (unsigned char)lerp(from.a, to.a, t)};
}

SDL_Color getColorFromEscapeIteration(int escapeIteration, SDL_Color palette[], int colorCount)
{
	//return escapeIteration == MAX_ITERATIONS ? palette[colorCount - 1] : palette[0];
	if (escapeIteration == MAX_ITERATIONS)
		return {0, 0, 0, 255};
	double perc = (double)escapeIteration / MAX_ITERATIONS;
	double fractionnalColorIndex = perc * colorCount;
	int firstColorIndex = int(floor(fractionnalColorIndex));

	double colorT = fractionnalColorIndex - firstColorIndex;
	if (firstColorIndex >= colorCount - 1)
		return palette[colorCount - 1];
	if (colorT <= 0.01)
		return palette[firstColorIndex];
	return lerpColor(palette[firstColorIndex], palette[firstColorIndex + 1], colorT);
}

void zoom(double zoomFactor, const Complex &zoomTarget, double *minX, double *maxX, double *minY, double *maxY)
{
	*minX = (*minX - zoomTarget.real) * zoomFactor + zoomTarget.real;
	*maxX = (*maxX - zoomTarget.real) * zoomFactor + zoomTarget.real;
	*minY = (*minY - zoomTarget.imag) * zoomFactor + zoomTarget.imag;
	*maxY = (*maxY - zoomTarget.imag) * zoomFactor + zoomTarget.imag;
}

Complex getComplexFromPixel(SDL_Point pixel, double minX, double maxX, double minY, double maxY)
{
	double real = lerp(minX, maxX, (double)pixel.x / WINDOW_WIDTH);
	double imag = lerp(minY, maxY, (double)pixel.y / WINDOW_HEIGHT);

	return Complex(real, imag);
}

int main(int argc, char **argv)
{
	SDL_Window *sdlWindow = SDL_CreateWindow("Fractal Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);

	SDL_Event sdlEvent;

	SDL_Color palette[] = {
		{0, 78, 200, 255},
		{255, 80, 10, 255},
		{255, 255, 255, 255},
		{0, 0, 0, 255}};
	int paletteColorCount = sizeof(palette) / sizeof(palette[0]);

	double graphMinX = -2.05;
	double graphMaxX = 0.6;
	double graphMinY = -1.2;
	double graphMaxY = 1.2;

	SDL_Point cameraPosition = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2}; // TODO: Properly center camera
	// cameraPosition.x = (int)((graphMaxX - graphMinX) / (abs(graphMaxX) + abs(graphMinX)) * WINDOW_WIDTH) + WINDOW_WIDTH / 2;
	// cameraPosition.y = (int)((graphMaxY - graphMinY) / (abs(graphMaxY) + abs(graphMinY)) * WINDOW_HEIGHT) + WINDOW_HEIGHT / 2;


	SDL_Point leftMouseStartPos = {0};
	SDL_Point leftMouseEndPos = {0};
	bool leftMouseDown = false;

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
				break;
			case SDL_MOUSEMOTION:
			{
				if (leftMouseDown)
				{
					// TODO: Fix camera movement, it's too fast and slippery
					leftMouseEndPos = {sdlEvent.motion.x, sdlEvent.motion.y};

					cameraPosition.x -= leftMouseEndPos.x - leftMouseStartPos.x;
					cameraPosition.y -= leftMouseEndPos.y - leftMouseStartPos.y;
				}
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
				if (sdlEvent.button.button == SDL_BUTTON_LEFT)
				{
					leftMouseDown = true;
					leftMouseStartPos = {sdlEvent.button.x, sdlEvent.button.y};
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (sdlEvent.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
					leftMouseDown = false;
					// TODO: don't zoom while moving the camera
					Complex zoomTarget = getComplexFromPixel({sdlEvent.button.x, sdlEvent.button.y}, graphMinX, graphMaxX, graphMinY, graphMaxY);
					zoom(ZOOM_FACTOR, zoomTarget, &graphMinX, &graphMaxX, &graphMinY, &graphMaxY);
					break;
				}
				case SDL_BUTTON_RIGHT:
				{
					Complex zoomTarget = getComplexFromPixel({sdlEvent.button.x, sdlEvent.button.y}, graphMinX, graphMaxX, graphMinY, graphMaxY);
					zoom(1 / ZOOM_FACTOR, zoomTarget, &graphMinX, &graphMaxX, &graphMinY, &graphMaxY);
					break;
				}
				}
				break;
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
				Complex complex = getComplexFromPixel({x, y}, graphMinX, graphMaxX, graphMinY, graphMaxY) + getComplexFromPixel(cameraPosition, graphMinX, graphMaxX, graphMinY, graphMaxY);
				int escapeIteration = escapeIterationFromMandelbrotSet(complex);
				SDL_Color pixelColor = getColorFromEscapeIteration(escapeIteration, palette, paletteColorCount);

				SDL_SetRenderDrawColor(renderer, pixelColor.r, pixelColor.g, pixelColor.b, pixelColor.a);
				SDL_RenderDrawPoint(renderer, x, y);
			}

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(sdlWindow);
	return 0;
}