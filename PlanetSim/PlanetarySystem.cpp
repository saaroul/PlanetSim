#include <cmath>
#include <iostream>
#include <future>
#include "PlanetarySystem.h"


void PlanetarySystem::simulate()
{
	std::vector<std::future<void>> futures;
	int chunk_count = 10; // Chunk count for async calls.
	int chunk_size = _planets.size() / chunk_count + (_planets.size() % chunk_count != 0);

	// Chunking vector in 10 equally sized portions to be processed asyncronously.
	for (int i = 0; i < _planets.size(); i += chunk_size)
	{
		futures.push_back(std::async(&PlanetarySystem::calculateNewPos, this, i, i + chunk_size));
	}

	// Join threads
	// Waits here until all chunks are processed.
	for (auto& fut : futures)
	{
		fut.get();
	}

	// Update new state
	updateAllPos();
}


void PlanetarySystem::calculateNewPos(int start, int end)
{
	for (; start < _planets.size() && start < end; start++)
	{
		calculatePositionFor(start);
	}
}


void PlanetarySystem::updateAllPos()
{
	for (Planet& p : _planets)
	{
		p.updatePos();
		p.updateVel();
	}
}


void PlanetarySystem::calculatePositionFor(int planet_index)
{
	Planet& current = _planets[planet_index];
	Vector2 cPos = current.getPos();
	Vector2 cVel = current.getVel();
	Vector2 acc = calculateAcceleration(cPos, planet_index);
	
	Vector2 new_vel = {
		cVel.x + acc.x * _dt,
		cVel.y + acc.y * _dt
	};
	Vector2 new_pos = {
		cPos.x + new_vel.x * _dt,
		cPos.y + new_vel.y * _dt
	};

	current.setPos(new_pos);
	current.setVel(new_vel);
}

Vector2 PlanetarySystem::calculateAcceleration(Vector2 pos, int planet_index)
{
	Vector2 acc = { 0.0f, 0.0f };
	for (int i = 0; i < _planets.size(); i++)
	{
		if (i == planet_index)
			continue;

		Planet &other = _planets[i];
		Vector2 d_pos = { other.getPos().x - pos.x, other.getPos().y - pos.y };
		double dist = sqrt(d_pos.x*d_pos.x + d_pos.y*d_pos.y);
		Vector2 dir = { d_pos.x / dist, d_pos.y / dist };

		double da = G * other.getMass() / (dist * dist);
		acc.x += da * dir.x;
		acc.y += da * dir.y;
	}
	return acc;
}
