#pragma once
#ifndef BOIDS_H
#define BOIDS_H
#include <SFML/Graphics.hpp>
#include "Quadtree.hpp"
#include <vector>
#include <cmath>

constexpr float PERCEPTION_RADIUS = 50.0f;
constexpr float MAX_VELOCITY = 5.0f;
constexpr float MIN_VELOCITY = 2.0f;


class Boid {
public:
	sf::Vector2f position;
	sf::Vector2f velocity;

	Boid(sf::Vector2f position, sf::Vector2f velocity);

	sf::Vector2f align(const std::vector<Boid>& perceivedNeighbors, float ALIGNMENT_FACTOR);
	sf::Vector2f cohesion(const std::vector<Boid>& perceivedNeighbors, float COHESION_FACTOR);
	sf::Vector2f separation(const std::vector<Boid>& perceivedNeighbors, float SEPARATION_FACTOR);

	std::vector<Boid> getPerceivedNeighbors(std::vector<void*> boids);

	void update(const Quadtree* quadtree, int width, int height, float ALIGNMENT_FACTOR, float COHESION_FACTOR, float SEPARATION_FACTOR);
	//void update(const std::vector<Boid>& boids, int width, int height, float ALIGNMENT_FACTOR, float COHESION_FACTOR, float SEPARATION_FACTOR);
};

#endif // BOID_H