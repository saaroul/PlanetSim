#pragma once
#include <string>
#include "Vector2.h"

/**
 * @brief Describes a basic celestial body.
 * 
 * Describes a celestial body in a N-Body system. Body is a point mass with defined initial
 * coordinates and velocity. Regardless of name Planet body may describe any point mass
 *  celestial body like planets and stars.
 */

class Planet
{
public:
	/**
	 * Constructor for planet.
	 * 
	 * \param name Name for planet
	 * \param mass Mass in kilos
	 * \param pos Initial position in meters on x, y coordinate system
	 * \param v Initial elocity in meters per second
	 */
	Planet(std::string name, double mass, Vector2 pos, Vector2 v) :
		_name(name), _mass(mass), _pos(pos), _v(v), _new_pos(_pos), _new_v(_v) {};
	~Planet() = default;
	
	/**
	 * \brief Get planet name.
	 * 
	 * \return string name of the planet
	 */
	std::string getName() { return _name; }

	/**
	 * \brief Get planet position.
	 * 
	 * \return Planet position
	 */
	Vector2 getPos() { return _pos; }

	/**
	 * \brief Set new position for planet.
	 * Sets a new position for a planet. New position is not applied until updatePos() is
	 *  called.
	 * @see updatePos()
	 * @see _new_pos
	 * 
	 * \param new_pos New position
	 */
	void setPos(Vector2 new_pos) { _new_pos = new_pos; }
	/**
	 * \brief Update position.
	 * Update planets position to value in _new_position. Must be called after using setPos()
	 * to change actual positon of the planet.
	 * 
	 */
	void updatePos() { _pos = _new_pos; }
	/**
	 * \brief Get planet velocity.
	 * Returns planets current velocity as 2d vector
	 * 
	 * \return Planet velocity
	 */
	Vector2 getVel() { return _v; }
	/**
	 * \brief Set new velocity.
	 * Set new velocity for planet. New velocity is not applied until updateVel() is called.
	 * 
	 * \param new_v New velocity
	 */
	void setVel(Vector2 new_v) { _new_v = new_v; }
	/**
	 * \brief Update position.
	 * Update planets velocity to value in _new_velocity. Must be called after using setVel()
	 * to change actual velocity of the planet.
	 *
	 */
	void updateVel() { _v = _new_v; }

	/**
	 * \brief Get planet mass.
	 * Returns the mass of the planet in kilograms.
	 * 
	 * \return Planet mass
	 */
	double getMass() { return _mass; }

private:
	double _mass; 
	Vector2 _pos;
	Vector2 _new_pos;
	Vector2 _v;
	Vector2 _new_v;
	std::string _name;
};

