#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>
#include <cmath>
#include <fstream>

enum class gameStateClass {

	mainMenu,
	play,
	exit
};

sf::Vector2f setRandomPosition(float xMin, float xMax, float yMin, float yMax) {

	static std::random_device random;
	static std::mt19937 mt(random());
	std::uniform_real_distribution<float> positionX(xMin, xMax);
	std::uniform_real_distribution<float> positionY(yMin, yMax);
	
	return sf::Vector2f(positionX(mt), positionY(mt));
}



int main() {

	sf::Clock clock;
	sf::Font font("arial.ttf");
	gameStateClass gameState = gameStateClass::mainMenu;

	//Window
	sf::RenderWindow window(sf::VideoMode({ 1200, 800 }), "SMPG");
	window.setFramerateLimit(60);
	window.setKeyRepeatEnabled(false);

	//Player
	sf::CircleShape player(40.f);
	player.setFillColor(sf::Color::Blue);
	player.setPosition({ 600.f, 700.f });
	const float playerSpeed = 400.f;

	//Points
	sf::CircleShape food(20.f);
	food.setFillColor(sf::Color::Red);
	food.setPosition({ 600.f, 700.f });

	//Enemy
	sf::CircleShape enemy(60.f);
	enemy.setFillColor(sf::Color::Green);
	enemy.setPosition({ 600.f, 100.f });
	const float enemySpeed = 150.f;

	//Score
	int score = -1;
	sf::Text scoreText(font);
	scoreText.setCharacterSize(40);
	scoreText.setFillColor(sf::Color::White);

	int highScore = 0;
	sf::Text highScoreText(font);
	highScoreText.setCharacterSize(70);
	highScoreText.setFillColor(sf::Color::White);
	highScoreText.setPosition({ 460.f, 100.f });

	//Menu text variables
	sf::Text playText(font);
	playText.setCharacterSize(80);
	playText.setString("Play");
	playText.setPosition({ 560.f, 340.f });

	sf::Text exitText(font);
	exitText.setCharacterSize(80);
	exitText.setString("Exit");
	exitText.setPosition({ 560.f, 420.f });

	int selectedOption = 0; // mainMenu = 0, play = 1, exit = 2
	int totalOptions = 2;


	//highScore code for writing
	std::ifstream inFileHighScore("highScore.txt");
	if (inFileHighScore.is_open()) {

		if (!(inFileHighScore >> highScore)) {

			highScore = 0;
		}

		inFileHighScore.close();
	}
	else {

		std::cout << "Failed to open highScore.txt for writing." << std::endl;
	}



	while (window.isOpen()) {

		//Variable that need to be updated while playing
		sf::Vector2f playerPosition = player.getPosition();
		sf::Vector2f enemyPosition = enemy.getPosition();
		
		while (std::optional event = window.pollEvent()) {

			if (event->is<sf::Event::Closed>()) {

				window.close();
			}

			//Switching between options in the menu
			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
				if (keyPressed->scancode == sf::Keyboard::Scan::Up) {
					if (selectedOption == 0) {
						selectedOption = 1;
					}
					else if (selectedOption == 1) {
						selectedOption = totalOptions;
					}
					else {
						selectedOption -= 1;
					}
				}
				else if (keyPressed->scancode == sf::Keyboard::Scan::Down) {
					if (selectedOption == 0) {
						selectedOption = 1;
					}
					else {
						selectedOption = (selectedOption % totalOptions) + 1;
					}
				}
				else if (keyPressed->scancode == sf::Keyboard::Scan::Enter) {
					if (selectedOption == 1) {
						gameState = gameStateClass::play; // Start the game
					}
					else if (selectedOption == 2) {
						gameState = gameStateClass::exit; // Exit the game
					}
				}
			}

		}


		if (gameState == gameStateClass::mainMenu) {

			std::cout << "Main menu state activated" << std::endl;
			std::cout << "Player position (" << playerPosition.x << ", " << playerPosition.y << ")" << std::endl;

			if (selectedOption == 1) {

				playText.setFillColor(sf::Color::Red);
			}
			else {

				playText.setFillColor(sf::Color::White);
			}

			if (selectedOption == 2) {

				exitText.setFillColor(sf::Color::Red);
			}
			else {

				exitText.setFillColor(sf::Color::White);
			}

			//highScore code
			highScoreText.setString("HighScore: " + std::to_string(highScore));

			window.clear();
			window.draw(playText);
			window.draw(exitText);
			window.draw(highScoreText);
			window.display();
		
		}
		else if (gameState == gameStateClass::play) {

			float deltaTime = clock.restart().asSeconds();
			if (deltaTime > 0.1f) {

				deltaTime = 0.1f;
			}

			std::cout << "Play state activated" << std::endl;
			std::cout << "Enemy position (" << enemyPosition.x << ", " << enemyPosition.y << ")" << std::endl;
			std::cout << "Player position (" << playerPosition.x << ", " << playerPosition.y << ")" << std::endl;
			std::cout << "Delta = " << deltaTime << std::endl;

			//Player movement
			sf::Vector2f playerMovement(0.f, 0.f);

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

			//Player borders code
			if (playerPosition.x > 1160.f) {

				player.setPosition({ -40.f, playerPosition.y });
			}
			else if (playerPosition.x < -40.f) {

				player.setPosition({ 1160.f, playerPosition.y });
			}
			else if (playerPosition.y > 760.f) {

				player.setPosition({ playerPosition.x, -40.f});
			}
			else if (playerPosition.y < -40.f) {

				player.setPosition({ playerPosition.x, 760.f });
			}


			//Food code
			if (food.getGlobalBounds().findIntersection(player.getGlobalBounds())) {

				sf::Vector2f newPosition = setRandomPosition(40, 1160, 40, 760);
				food.setPosition(newPosition);
				score++;
			}

			//Enemy code
			if (enemy.getGlobalBounds().findIntersection(player.getGlobalBounds())) {

				gameState = gameStateClass::mainMenu;
				player.setPosition({ 600.f, 700.f });
				food.setPosition({ 600.f, 700.f });
				enemy.setPosition({ 600.f, 100.f });
				score = -1;
			}

			sf::Vector2f direction = player.getPosition() - enemy.getPosition();
			float magnitude = std::sqrt(direction.x * direction.x + direction.y * direction.y);
			if (magnitude != 0) {

				direction /= magnitude;
			}

			sf::Vector2f enemyMovement = direction * enemySpeed * deltaTime;
			
			enemy.move(enemyMovement);
			

			//Score-highScore code
			scoreText.setString("Score: " + std::to_string(score));

			if (highScore < score) {

				highScore = score;
				std::ofstream outFileHighScore("highScore.txt");
				if (outFileHighScore.is_open()) {

					outFileHighScore << highScore;
					outFileHighScore.close();
					std::cout << "HighScore updated to: " << highScore << std::endl;
				}
				else {

					std::cout << "Failed to open highScore.txt for writing." << std::endl;
				}
			}
			else {

				std::cout << "Score isn't higher than highScore." << std::endl;
			}


			window.clear();
			window.draw(player);
			window.draw(food);
			window.draw(enemy);
			window.draw(scoreText);
			window.display();
		}
		else if (gameState == gameStateClass::exit) {

			std::cout << "Exit state activated" << std::endl;
			std::cout << "HighScore: " << highScore << std::endl;

			window.close();
		}

	}

	return 0;
}