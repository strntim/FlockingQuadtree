#include <SFML/Graphics.hpp>
#include <random>
#include <vector>
#include <chrono>
#include <stdio.h>
#include "Quadtree.hpp"
#include "Boids.h"

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

# define PI           3.14159f

int NUM_BOIDS = 200;

const int MAX_BOIDS = 2000;



int main() {
	int WIDTH = 1600;
	int HEIGHT = 900;

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "FlockingQuadtree");
	ImGui::SFML::Init(window);

	std::vector<Boid> boids;
	Quadtree quadtree(sf::FloatRect(0, 0, WIDTH, HEIGHT), MAX_BOIDS);

	float ALIGNMENT_FACTOR = 0.01f;
	float COHESION_FACTOR = 0.01f;
	float SEPARATION_FACTOR = 0.1f;

	bool useTriangle = true;

	int spawnCount = 1;

	// Variables for measuring FPS
	float fps = 0.0f;
	int fpsCap = 60; //fps aslo determines simulation speed
	int frames = 0;
	sf::Clock fpsClock;
	sf::Time previousTime = fpsClock.getElapsedTime();
	sf::Time currentTime;

	window.setFramerateLimit(fpsCap);

	// Initialize random number generator
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> posX(0.0f, WIDTH);
	std::uniform_real_distribution<float> posY(0.0f, HEIGHT);
	std::uniform_real_distribution<float> velX(-MAX_VELOCITY, MAX_VELOCITY);
	std::uniform_real_distribution<float> velY(-MAX_VELOCITY, MAX_VELOCITY);
	//std::uniform_real_distribution<float> randomPos(0.0f, WIDTH);
	//std::uniform_real_distribution<float> randomVel(-MAX_VELOCITY, MAX_VELOCITY);

	// Create boids
	for (int i = 0; i < NUM_BOIDS; ++i) {
		sf::Vector2f position(posX(gen), posY(gen));
		sf::Vector2f velocity(velX(gen), velY(gen));
		Boid boid(position, velocity);
		boids.push_back(boid);
	}

	sf::Vector2f clickPosition;
	bool isMousePressed = false;

	// Function to spawn a new boid at the given position with a velocity
	auto spawnBoid = [&](const sf::Vector2f& position, const sf::Vector2f& velocity) {
		Boid newBoid(position, velocity);
		boids.push_back(newBoid);
	};

	// Game loop
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed) {
				window.close();
			}
			else if (event.type == sf::Event::Resized)
			{
				// Update the width and height
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(visibleArea));
				WIDTH = event.size.width;
				HEIGHT = event.size.height;

				quadtree.resize(sf::FloatRect(0, 0, WIDTH, HEIGHT), MAX_BOIDS);
			}
			else if (event.type == sf::Event::MouseButtonPressed) {
				// Start tracking mouse position when the left button is pressed
				if (event.mouseButton.button == sf::Mouse::Left) {
					clickPosition = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
					isMousePressed = true;
				}
			}
			else if (event.type == sf::Event::MouseButtonReleased) {
				// Spawn a new boid with the dragged direction as velocity when the left button is released
				if (event.mouseButton.button == sf::Mouse::Left && isMousePressed) {
					sf::Vector2f releasePosition(event.mouseButton.x, event.mouseButton.y);
					sf::Vector2f velocity = releasePosition - clickPosition;
					for (int i = 0; i < spawnCount; i++)
					{
						spawnBoid(clickPosition + sf::Vector2f(velX(gen), velY(gen)) * 10.0f, velocity + sf::Vector2f(velX(gen), velY(gen)) * 0.3f);
					}
					isMousePressed = false;
				}
			}
		}

		// UI window for adjusting factors and boid representation
		ImGui::SFML::Update(window, sf::seconds(1.0f / 60.0f));

		ImGui::Begin("Flocking Factors");

		ImGui::Text("Number of boids: %d", NUM_BOIDS);

		ImGui::Separator();

		ImGui::SliderFloat("Alignment Factor", &ALIGNMENT_FACTOR, 0.0f, 1.0f);
		ImGui::SliderFloat("Cohesion Factor", &COHESION_FACTOR, 0.0f, 1.0f);
		ImGui::SliderFloat("Separation Factor", &SEPARATION_FACTOR, 0.0f, 1.0f);

		ImGui::Separator();

		ImGui::Checkbox("Use Triangle", &useTriangle);

		ImGui::Separator();

		// Display the FPS specifically for the boids simulation
		ImGui::Text("Boids FPS: %.1f", fps);
		ImGui::SliderInt("Framerate limit", &fpsCap, 0, 1000);

		ImGui::Separator();

		ImGui::InputInt("Spawn on click", &spawnCount);

		ImGui::End();

		window.clear();

		// Clear the quadtree
		quadtree.clear();
		NUM_BOIDS = 0;

		// Update the quadtree with boids
		for (Boid& boid : boids) {
			quadtree.insert(boid.position, &boid);
			NUM_BOIDS++;
		}

		// Update and draw boids
		for (Boid& boid : boids) {
			//quadtree.queryRange(boid.position, perceptionRadius, perceivedNeighbours);
			boid.update(&quadtree, WIDTH, HEIGHT, ALIGNMENT_FACTOR, COHESION_FACTOR, SEPARATION_FACTOR);

			// Create the appropriate shape based on the useTriangle variable
			if (useTriangle) {
				// Create a triangle shape
				sf::ConvexShape triangle(3);
				triangle.setFillColor(sf::Color::White);
				triangle.setPoint(0, sf::Vector2f(0.0f, -8.0f));
				triangle.setPoint(1, sf::Vector2f(-5.0f, 2.0f));
				triangle.setPoint(2, sf::Vector2f(5.0f, 2.0f));

				// Set the position and rotation based on the boid's values
				triangle.setPosition(boid.position);
				triangle.setOrigin(0.0f, -1.0f);  // Set the origin to the center of the base

				float angle = (std::atan2(-boid.velocity.y, boid.velocity.x) / PI * 180.0f) * -1 + 90.0f;
				triangle.setRotation(angle);

				window.draw(triangle);
			}
			else {
				sf::CircleShape circle(5.0f);
				circle.setFillColor(sf::Color::White);
				circle.setPosition(boid.position);
				window.draw(circle);
			}
		}

		ImGui::SFML::Render(window);

		window.display();

		//fps stuff
		frames++;
		currentTime = fpsClock.getElapsedTime();
		//fps += 1.0f / (currentTime.asSeconds() - previousTime.asSeconds());
		if (currentTime.asSeconds() > 1.0f)
		{
			fps = frames / currentTime.asSeconds();
			previousTime = currentTime;
			frames = 0;
			fpsClock.restart();

			window.setFramerateLimit(fpsCap);
		}
	}

	ImGui::SFML::Shutdown();

	return 0;
}