#include "Boids.h"
#include <cmath>
#include <random>

int WIDTH = 1600;
int HEIGHT = 900;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> posX(0.0f, WIDTH);
std::uniform_real_distribution<float> posY(0.0f, HEIGHT);
std::uniform_real_distribution<float> velX(-MAX_VELOCITY, MAX_VELOCITY);
std::uniform_real_distribution<float> velY(-MAX_VELOCITY, MAX_VELOCITY);

Boid::Boid(sf::Vector2f position, sf::Vector2f velocity)
	: position(position), velocity(velocity) {}

sf::Vector2f Boid::align(const std::vector<Boid>& perceivedNeighbors, float ALIGNMENT_FACTOR) {
	//std::vector<Boid> perceivedNeighbors = getPerceivedNeighbors(boids);
	if (!perceivedNeighbors.empty()) {
		sf::Vector2f avgVelocity(0.0f, 0.0f);
		for (const Boid& neighbor : perceivedNeighbors) {
			avgVelocity += neighbor.velocity;
		}
		avgVelocity /= static_cast<float>(perceivedNeighbors.size());
		sf::Vector2f steering = (avgVelocity - velocity) * ALIGNMENT_FACTOR;
		return steering;
	}
	return sf::Vector2f(0.0f, 0.0f);
}

sf::Vector2f Boid::cohesion(const std::vector<Boid>& perceivedNeighbors, float COHESION_FACTOR) {
	//std::vector<Boid> perceivedNeighbors = getPerceivedNeighbors(boids);
	if (!perceivedNeighbors.empty()) {
		sf::Vector2f avgPosition(0.0f, 0.0f);
		for (const Boid& neighbor : perceivedNeighbors) {
			avgPosition += neighbor.position;
		}
		avgPosition /= static_cast<float>(perceivedNeighbors.size());
		sf::Vector2f desiredVelocity = (avgPosition - position) * 0.1f;
		sf::Vector2f steering = (desiredVelocity - velocity) * COHESION_FACTOR;
		return steering;
	}
	return sf::Vector2f(0.0f, 0.0f);
}

sf::Vector2f Boid::separation(const std::vector<Boid>& perceivedNeighbors, float SEPARATION_FACTOR) {
	//std::vector<Boid> perceivedNeighbors = getPerceivedNeighbors(boids);
	if (!perceivedNeighbors.empty()) {
		sf::Vector2f steering(0.0f, 0.0f);
		for (const Boid& neighbor : perceivedNeighbors) {
			float distance = std::sqrt(std::pow(neighbor.position.x - position.x, 2) +
				std::pow(neighbor.position.y - position.y, 2));
			if (distance < 0.05f) {
				distance = 0.05f;
			}
			if (distance < PERCEPTION_RADIUS / 2.0f) {
				steering -= (neighbor.position - position) / distance;
			}
		}
		return steering * SEPARATION_FACTOR;
	}
	return sf::Vector2f(0.0f, 0.0f);
}

std::vector<Boid> Boid::getPerceivedNeighbors(std::vector<void*> boids) {
	std::vector<Boid> newPerceivedNeighbors;

	for (void* object : boids) {
		const Boid other = *static_cast<Boid*>(object);
		if (&other != this) {
			float distance = (other.position.x - position.x) * (other.position.x - position.x) +
				(other.position.y - position.y) * (other.position.y - position.y);
			if (distance < PERCEPTION_RADIUS * PERCEPTION_RADIUS) {
				newPerceivedNeighbors.push_back(other);
			}
		}
	}
	return newPerceivedNeighbors;
}

void Boid::update(const Quadtree* quadtree, int width, int height, float ALIGNMENT_FACTOR, float COHESION_FACTOR, float SEPARATION_FACTOR) {
	std::vector<void*> querryNeighbors;
	quadtree->queryRange(position, PERCEPTION_RADIUS, querryNeighbors);

	/*std::vector<Boid> convertedNeighbors;
	convertedNeighbors.reserve(querryNeighbors.size());
	for (void* object : querryNeighbors) {
		convertedNeighbors.push_back(*static_cast<Boid*>(object));
	}*/

	std::vector<Boid> perceivedNeighbors = getPerceivedNeighbors(querryNeighbors);

	sf::Vector2f alignment = align(perceivedNeighbors, COHESION_FACTOR);
	sf::Vector2f cohesion_ = cohesion(perceivedNeighbors, COHESION_FACTOR);
	sf::Vector2f separation_ = separation(perceivedNeighbors, SEPARATION_FACTOR);
	velocity += alignment + cohesion_ + separation_;

	// Limit the velocity
	float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
	if (speed > MAX_VELOCITY) {
		velocity = (velocity / speed) * MAX_VELOCITY;
	}

	if (speed < MIN_VELOCITY) {
		velocity = (velocity / speed) * MIN_VELOCITY;
	}

	position += velocity;

	// Wrap around the screen
	if (position.x < 0) {
		position.x = width;
	}
	if (position.x > width) {
		position.x = 0;
	}
	if (position.y < 0) {
		position.y = height;
	}
	if (position.y > height) {
		position.y = 0;
	}
}