#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class QuadtreeNode {
public:
	QuadtreeNode(const sf::FloatRect& bounds, int capacity);
	~QuadtreeNode();

	void insert(sf::Vector2f position, void* data);
	void queryRange(const sf::FloatRect& range, std::vector<void*>& foundObjects) const;
	void clear();

private:
	bool isLeaf() const;

	sf::FloatRect bounds;
	int capacity;
	std::vector<std::pair<sf::Vector2f, void*>> objects;
	QuadtreeNode* children[4];
};

class Quadtree {
public:
	Quadtree(const sf::FloatRect& bounds, int capacity);
	~Quadtree();

	void resize(const sf::FloatRect& newBounds, int capacity);

	void insert(sf::Vector2f position, void* data);
	void queryRange(sf::Vector2f position, float range, std::vector<void*>& foundObjects) const;
	void clear();

private:
	QuadtreeNode* root;
};