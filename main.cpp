#include <SFML/Graphics.hpp>

int main() {

	sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "SMPG");
	window.setFramerateLimit(60);
	sf::CircleShape player(40.f);
	player.setFillColor(sf::Color::Blue);
	player.setPosition({ 400.f, 300.f });

	const float speed = 4.f;

	while (window.isOpen()) {

		while (std::optional event = window.pollEvent()) {

			if (event->is<sf::Event::Closed>()) {

				window.close();
			}

		}

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


		player.move(movement);

		window.clear();
		window.draw(player);
		window.display();

	}

}