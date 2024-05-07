#include <iostream>
#include <random>
#include <string>
#include <string.h>
#include <chrono>
#include <ctime>
#include "SDL_filesystem.h"
#include "SDL.h"
#include "SDL_rwops.h"
#include "SDL_ttf.h"
#include "planets.h"
#include "PlanetarySystem.h"

/*
https://iq.direct/blog/315-simple-program-to-explain-sdl-rendering.html
https://stackoverflow.com/questions/38334081/how-to-draw-circles-arcs-and-vector-graphics-in-sdl
https://codereview.stackexchange.com/questions/234331/modelling-solar-system-runge-kutta-4-n-body-problem
*/


const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int HALF_SCREEN_WIDTH = SCREEN_WIDTH/2;
const int HALF_SCREEN_HEIGHT = SCREEN_HEIGHT/2;
long long meter_to_pix = 100'000'000;
double timestep = 1.0f;

void DrawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius);
void get_text_and_rect(SDL_Renderer* renderer, int x, int y, const char* text,
	TTF_Font* font, SDL_Texture** texture, SDL_Rect* rect);
void RenderPlanets(SDL_Renderer *rend, PlanetarySystem* system);


const int direction_count = 360;
Vector2 directions[direction_count];

int main(int argc, char** argv)
{
	// Exit
	auto at_exit = []() {
		SDL_Quit();
		TTF_Quit();
		};
	const int AT_EXIT_FAILURE = std::atexit(at_exit);
	if (AT_EXIT_FAILURE)
	{
		std::cout << "At exit failed to register";
		return EXIT_FAILURE;
	}
	// Create lookup table for 360 deg unit vectors
	for (int i = 0; i < direction_count; i++)
	{
		directions[i] = { 
			std::cos(2 * (double)std::_Pi_val * i / direction_count),
			std::sin(2 * (double)std::_Pi_val * i / direction_count) };
	}

	// Initialize SDL and SDL_TTF
	// retutns zero on success else non-zero 
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	SDL_Window* win = SDL_CreateWindow("Planet Sim", // creates a window 
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (TTF_Init() != 0)
	{
		printf("error initializing TTF");
		return EXIT_FAILURE;
	}

	// triggers the program that controls 
	// your graphics hardware and sets flags 
	Uint32 render_flags = SDL_RENDERER_ACCELERATED;

	// creates a renderer to render our images 
	SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

	// Load a font
	// Looks for Consolas.ttf in the build folder.
	// Change extension to match font you want to use and add it to build folder.
	char path_name[128];
	char* root_path = SDL_GetBasePath();
	const char* extension = "Consolas.ttf";

	if (sizeof(path_name) < strlen(root_path) + 1) { /* +1 is for null character */
		fprintf(stderr, "Name '%s' is too long\n", root_path);
		return EXIT_FAILURE;
	}

	strncpy_s(path_name, root_path, sizeof(path_name));
	if (sizeof(path_name) < (strlen(path_name) + strlen(extension) + 1)) {
		fprintf(stderr, "Final size of filename is too long!\n");
		return EXIT_FAILURE;
	}

	strncat_s(path_name, extension, (sizeof(path_name) - strlen(path_name)));
	printf("Filename is %s\n", path_name);
	TTF_Font* font = TTF_OpenFont(path_name, 24);
	if (font == NULL) {
		fprintf(stderr, "error: font not found\n");
		exit(EXIT_FAILURE);
	}


	// Create planetary system
	PlanetarySystem system(timestep);

	
	// Add planets to system
	// Use comments to control what are included or make your own planets
	// Units are in SI units (meters, seconds, kilos) although should probably be in 
	// astronomical unit scales in the future.

	// Normal solar system

	system.addPlanet(Planet("Sun", SolarMass, SolarPos, SolarVel));
	system.addPlanet(Planet("Mercury", MercuryMass, MercuryPos, MercuryVel));
	system.addPlanet(Planet("Venus", VenusMass, VenusPos, VenusVel));
	system.addPlanet(Planet("Earth", EarthMass, EarthPos, EarthVel));
	system.addPlanet(Planet("Mars", MarsMass, MarsPos, MarsVel));
	system.addPlanet(Planet("Jupiter", JupiterMass, JupiterPos, JupiterVel));
	system.addPlanet(Planet("Saturn", SaturnMass, SaturnPos, SaturnVel));
	system.addPlanet(Planet("Uranus", UranusMass, UranusPos, UranusVel));
	system.addPlanet(Planet("Neptune", NeptuneMass, NeptunePos, NeptuneVel));


	// Solar system with everything having suns mass

	//system.addPlanet(Planet("Sun", SolarMass, SolarPos, SolarVel));
	//system.addPlanet(Planet("Mercury", SolarMass, MercuryPos, MercuryVel));
	//system.addPlanet(Planet("Venus", SolarMass, VenusPos, VenusVel));
	//system.addPlanet(Planet("Earth", SolarMass, EarthPos, EarthVel));
	//system.addPlanet(Planet("Mars", SolarMass, MarsPos, MarsVel));
	//system.addPlanet(Planet("Jupiter", SolarMass, JupiterPos, JupiterVel));
	//system.addPlanet(Planet("Saturn", SolarMass, SaturnPos, SaturnVel));
	//system.addPlanet(Planet("Uranus", SolarMass, UranusPos, UranusVel));
	//system.addPlanet(Planet("Neptune", SolarMass, NeptunePos, NeptuneVel));


	// Random distribution of planets
	
	//std::srand(std::time(nullptr));
	//for (int i = 1; i <= 8000; i++)
	//{
	//	Vector2 dir = directions[rand() % direction_count];
	//	long long dist = std::rand() * 2000000000;
	//	Vector2 pos = { dir.x * dist, dir.y * dist };
	//	int speed = rand() % 100 * 1000;
	//	Vector2 vel = { dir.y * speed, dir.x * -speed };
	//	system.addPlanet(Planet("p" + i, MercuryMass, pos, vel));
	//}


	// Text boxes for fps and sim speed
	SDL_Rect sim_speed_text_rect;
	SDL_Texture* sim_speed_text_texture;
	SDL_Rect fps_text_rect;
	SDL_Texture* fps_text_texture;

	// Main loop
	bool keep_window_open = true;
	while (keep_window_open)
	{

		auto frame_start = std::chrono::high_resolution_clock::now();

		// SDL even handling
		SDL_Event e;
		while (SDL_PollEvent(&e) > 0)
		{
			switch (e.type)
			{
			case SDL_QUIT: // EXIT
				keep_window_open = false;
				break;
			case SDL_KEYDOWN:
				// keyboard API for key pressed 
				switch (e.key.keysym.scancode) {
				case SDL_SCANCODE_UP:
					meter_to_pix /= 1.25; // Change meters to pixel scaling up or down
					break;
				case SDL_SCANCODE_DOWN:
					meter_to_pix *= 1.25;
					break;
				case SDL_SCANCODE_LEFT:
					timestep /= 2;
					system.setTimestep(timestep); // Speed up or slow down simulation
					break;
				case SDL_SCANCODE_RIGHT:
					timestep *= 2;
					system.setTimestep(timestep);
					break;
				}
			}
		}

		auto sim_start = std::chrono::high_resolution_clock::now();

		system.simulate(); // Simulate one timestep

		auto sim_end = std::chrono::high_resolution_clock::now();

		// Rendering
		auto rend_start = std::chrono::high_resolution_clock::now();
		// clears the screen 
		SDL_RenderClear(rend);

		// Render the planets
		RenderPlanets(rend, &system);
		auto rend_end = std::chrono::high_resolution_clock::now();

		auto sim_duration = std::chrono::duration_cast<std::chrono::microseconds>(sim_end - sim_start);
		auto rend_duration = std::chrono::duration_cast<std::chrono::microseconds>(rend_end - rend_start);


		//std::cout << "Simulation time: " << sim_duration.count() << std::endl;
		//std::cout << "Render time: " << rend_duration.count() << std::endl;

		// wait until approx 16ms has passed for 60Hz refresh rate
		int delay = (16000 - rend_duration.count() - sim_duration.count()) / 1000;
		if (!(delay < 0))
			SDL_Delay(delay);
		auto frame_end = std::chrono::high_resolution_clock::now();
		auto frame_time = std::chrono::duration_cast<std::chrono::microseconds>(frame_end - frame_start);
		//std::cout << "Frame Time: " << frame_time.count() << "microseconds" << std::endl;


		// Sim speed text box update
		std::string s = "timestep: " + std::to_string(timestep) + " seconds";
		get_text_and_rect(rend, 0, 0, s.c_str(), font, &sim_speed_text_texture, &sim_speed_text_rect);
		SDL_RenderCopy(rend, sim_speed_text_texture, NULL, &sim_speed_text_rect);

		// fps counter update
		std::string fps = "fps " + std::to_string(1000000/frame_time.count());
		get_text_and_rect(rend, 0, sim_speed_text_rect.y + sim_speed_text_rect.h, fps.c_str(), font, &fps_text_texture, &fps_text_rect);
		SDL_RenderCopy(rend, fps_text_texture, NULL, &fps_text_rect);

		// Update screen
		SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
		SDL_RenderPresent(rend);

	}

	return 0;
}



void RenderPlanets(SDL_Renderer *rend,PlanetarySystem* system)
{
	int n = system->getPlanetCount();
	for (int i = 0, r = 0, g = 0, b = 0; i < n; i++)
	{
		SDL_SetRenderDrawColor(rend, 100+(4*i)%155, 100 + (2 * i) % 155, 100 + (3 * i) % 155, 0);
		//SDL_SetRenderDrawColor(rend, 200, 200, 200, 0);
		Vector2 pos = system->getPlanetPosition(i);
		DrawCircle(rend, pos.x/meter_to_pix+HALF_SCREEN_WIDTH, pos.y/meter_to_pix+HALF_SCREEN_HEIGHT, 10);
	}
}

void get_text_and_rect(SDL_Renderer* renderer, int x, int y, const char* text,
	TTF_Font* font, SDL_Texture** texture, SDL_Rect* rect) {
	int text_width;
	int text_height;
	SDL_Surface* surface;
	SDL_Color textColor = { 255, 255, 255, 0 };

	surface = TTF_RenderText_Solid(font, text, textColor);
	*texture = SDL_CreateTextureFromSurface(renderer, surface);
	text_width = surface->w;
	text_height = surface->h;
	SDL_FreeSurface(surface);
	rect->x = x;
	rect->y = y;
	rect->w = text_width;
	rect->h = text_height;
}

void DrawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius)
{
	const int32_t diameter = (radius * 2);

	int32_t x = (radius - 1);
	int32_t y = 0;
	int32_t tx = 1;
	int32_t ty = 1;
	int32_t error = (tx - diameter);

	while (x >= y)
	{
		//  Each of the following renders an octant of the circle
		SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
		SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
		SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
		SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
		SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
		SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
		SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
		SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

		if (error <= 0)
		{
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0)
		{
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
}