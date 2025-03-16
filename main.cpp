#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>
#include <cmath>

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
	const float playerSpeed = 400.f;
	//Points
	sf::CircleShape food(20.f);
	food.setFillColor(sf::Color::Red);
	food.setPosition({600.f, 200.f});
	//Enemy
	sf::CircleShape enemy(60.f);
	enemy.setFillColor(sf::Color::Green);
	enemy.setPosition({ 600.f, 600.f });
	const float enemySpeed = 150.f;
	//Score
	int score = -1;

	sf::Clock clock;

	while (window.isOpen()) {

		while (std::optional event = window.pollEvent()) {

			if (event->is<sf::Event::Closed>()) {

				window.close();
			}

		}

		//Player movement
		sf::Vector2f playerMovement(0.f, 0.f);
		float deltaTime = clock.restart().asSeconds();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
			playerMovement.x -= playerSpeed * deltaTime;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
			playerMovement.x += playerSpeed * deltaTime;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
			playerMovement.y -= playerSpeed * deltaTime;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
			playerMovement.y += playerSpeed * deltaTime;
		}

		player.move(playerMovement);

		//Food code
		if (food.getGlobalBounds().findIntersection(player.getGlobalBounds())) {
			  
			sf::Vector2f newPosition = setRandomPosition(40, 1160, 40, 760);
			food.setPosition(newPosition);
			score++;
		}

		//Enemy code
		if (enemy.getGlobalBounds().findIntersection(player.getGlobalBounds())) {

			window.close();
			std::cout << "You lost!" << std::endl;
		}

		sf::Vector2f direction = player.getPosition() - enemy.getPosition();
		float magnitude = std::sqrt(direction.x * direction.x + direction.y * direction.y);
		if (magnitude != 0) {

			direction /= magnitude;
		}
		sf::Vector2f enemyMovement = direction * enemySpeed * deltaTime;
		enemy.move(enemyMovement);


		window.clear();
		window.draw(player);
		window.draw(food);
		window.draw(enemy);
		window.display();

	}

	std::cout << "Score: " << score << std::endl;
	return 0;
}