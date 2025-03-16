#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>

sf::Vector2f setRandomPosition(float xMin, float xMax, float yMin, float yMax) {

	static std::random_device random;
	static std::mt19937 mt(random());
	std::uniform_real_distribution<float> positionX(xMin, xMax);
	std::uniform_real_distribution<float> positionY(yMin, yMax);
	
	return sf::Vector2f(positionX(mt), positionY(mt));
}


int main() {

	//Window
	sf::RenderWindow window(sf::VideoMode({ 1200, 800 }), "SMPG");
	window.setFramerateLimit(60);
	//Player
	sf::CircleShape player(40.f);
	player.setFillColor(sf::Color::Blue);
	player.setPosition({ 600.f, 200.f });
	const float speed = 8.f;
	//Points
	sf::CircleShape food(20.f);
	food.setFillColor(sf::Color::Red);
	food.setPosition({600.f, 600.f});
	//Score
	int score = 0;

	while (window.isOpen()) {

		while (std::optional event = window.pollEvent()) {

			if (event->is<sf::Event::Closed>()) {

				window.close();
			}

		}

		//Player movement
		sf::Vector2f movement(0.f, 0.f);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
			movement.x -= speed;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
			movement.x += speed;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
			movement.y -= speed;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
			movement.y += speed;
		}

		//Food code
		if (food.getGlobalBounds().findIntersection(player.getGlobalBounds())) {
			  
			sf::Vector2f newPosition = setRandomPosition(40, 1160, 40, 760);
			food.setPosition(newPosition);
			score++;
		}

		player.move(movement);

		window.clear();
		window.draw(player);
		window.draw(food);
		window.display();

	}

	std::cout << "Score: " << score << std::endl;
	return 0;
}