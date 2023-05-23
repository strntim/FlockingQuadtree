#include "Quadtree.hpp"

QuadtreeNode::QuadtreeNode(const sf::FloatRect& bounds, int capacity)
	: bounds(bounds), capacity(capacity) {
	for (int i = 0; i < 4; ++i) {
		children[i] = nullptr;
	}
}

QuadtreeNode::~QuadtreeNode() {
	clear();
}

void QuadtreeNode::insert(sf::Vector2f position, void* data) {
	if (!bounds.contains(position))
		return;

	if (isLeaf()) {
		if (objects.size() < capacity) {
			objects.emplace_back(position, data);
		}
		else {
			if (!children[0]) {
				float halfWidth = bounds.width / 2.0f;
				float halfHeight = bounds.height / 2.0f;
				children[0] = new QuadtreeNode(sf::FloatRect(bounds.left, bounds.top, halfWidth, halfHeight), capacity);
				children[1] = new QuadtreeNode(sf::FloatRect(bounds.left + halfWidth, bounds.top, halfWidth, halfHeight), capacity);
				children[2] = new QuadtreeNode(sf::FloatRect(bounds.left, bounds.top + halfHeight, halfWidth, halfHeight), capacity);
				children[3] = new QuadtreeNode(sf::FloatRect(bounds.left + halfWidth, bounds.top + halfHeight, halfWidth, halfHeight), capacity);
			}

			for (const auto& object : objects) {
				for (int i = 0; i < 4; ++i) {
					children[i]->insert(object.first, object.second);
				}
			}

			objects.clear();

			for (int i = 0; i < 4; ++i) {
				children[i]->insert(position, data);
			}
		}
	}
	else {
		for (int i = 0; i < 4; ++i) {
			children[i]->insert(position, data);
		}
	}
}

void QuadtreeNode::queryRange(const sf::FloatRect& range, std::vector<void*>& foundObjects) const {
	if (!bounds.intersects(range))
		return;

	for (const auto& object : objects) {
		if (range.contains(object.first))
			foundObjects.push_back(object.second);
	}

	if (!isLeaf()) {
		for (int i = 0; i < 4; ++i) {
			children[i]->queryRange(range, foundObjects);
		}
	}
}

void QuadtreeNode::clear() {
	objects.clear();
	if (!isLeaf()) {
		for (int i = 0; i < 4; ++i) {
			children[i]->clear();
			delete children[i];
			children[i] = nullptr;
		}
	}
}

void Quadtree::resize(const sf::FloatRect& newBounds, int capacity) {
	clear(); // Clear the existing quadtree

	// Rebuild the quadtree with the new bounds
	root = new QuadtreeNode(newBounds, capacity);
}

bool QuadtreeNode::isLeaf() const {
	return children[0] == nullptr;
}

Quadtree::Quadtree(const sf::FloatRect& bounds, int capacity)
	: root(new QuadtreeNode(bounds, capacity)) {}

Quadtree::~Quadtree() {
	delete root;
}

void Quadtree::insert(sf::Vector2f position, void* data) {
	root->insert(position, data);
}

void Quadtree::queryRange(sf::Vector2f position, float range, std::vector<void*>& foundObjects) const {
	sf::FloatRect rangeRect(position.x - range, position.y - range, range * 2, range * 2);
	root->queryRange(rangeRect, foundObjects);
}

void Quadtree::clear() {
	root->clear();
}