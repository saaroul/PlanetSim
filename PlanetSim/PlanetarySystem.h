#pragma once
#include <vector>
#include "Planet.h"

/**
 * \brief Describes a planetary system.
 * Describes a planetary system. Provides ways to add planets and simulate their movement
 * based on initial values and gravity.
 */
class PlanetarySystem
{
public:
	PlanetarySystem(double timestep) : _dt(timestep) {};
	~PlanetarySystem() = default;
	/**
	 * @brief Simulate next timestep.
	 * Updates all the bodies in the system according to their velocities and gravity.
	 * Bodies are chunked into 10 chunks and given to 10 async function calls to be
	 * processed asyncronously.
	 */
	void simulate();
	void setTimestep(double timestep) { _dt = timestep; }
	Vector2 getPlanetPosition(int i) { return _planets[i].getPos(); };
	int getPlanetCount() {	return _planets.size();	}
	void addPlanet(Planet p) { _planets.push_back(p); };
private:
	/**
	 * \brief Calculate new pos and velo.
	 * Calculate new position and velocity based on acceleration from other bodies.
	 * @see calculateAcceleration()
	 *
	 * \param planet_index index of the planet
	 */
	void calculatePositionFor(int planet_index);

	/**
	 * \brief Process a chunk.
	 * Process a chunk of planets between start and given end point or end of list.
	 *
	 * \param start start index
	 * \param end end index
	 */
	void calculateNewPos(int i, int end);

	/**
	 * \brief Calculate acceleration.
	 * Calculate acceleration from gravity from other bodies in the system at a specified position.
	 *
	 * \param pos Position
	 * \param planet_index Planet to ignore
	 * \return Acceleration at position
	 */
	Vector2 calculateAcceleration(Vector2 pos, int planet_index);

	/**
	 * \brief Apply new positions and velocity for all bodies.
	 * Applies new positions and velocities for all bodies in this system.
	 * Called after calculating new positions and velocities. Updating is separated from
	 * calculation to ensure each body has new positions and velocities calculated in the same timestep.
	 *
	 */
	void updateAllPos();
	std::vector<Planet> _planets;
	double _dt;
	double G = 6.6743E-11; // Gravitational constant
};

