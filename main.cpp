#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>
#include <cmath>
#include <fstream>
#include <algorithm>

enum class gameStateClass {

	gameStateDefault,
	mainMenu,
	play,
	levels,
	exit
};

enum class levelsClass {

	levelsDefault,
	level1,
	level2,
	level3,
	level4,
	level5,
	exit 
};

bool detectCollision(sf::CircleShape& entity1, sf::CircleShape& entity2) {

	sf::FloatRect bounds1 = entity1.getGlobalBounds();
	sf::FloatRect bounds2 = entity2.getGlobalBounds();

	sf::Vector2f center1(bounds1.getCenter());
	sf::Vector2f center2(bounds2.getCenter());

	float distance = static_cast<float>(std::sqrt(std::pow(center1.x - center2.x, 2) + std::pow(center1.y - center2.y, 2)));

	float radius1 = entity1.getRadius();
	float radius2 = entity2.getRadius();

	if (distance <= radius1 + radius2) {

		return true;
	}
	else {

		return false;
	}
}

sf::Vector2f setRandomPosition(float xMin, float xMax, float yMin, float yMax) {

	static std::random_device random;
	static std::mt19937 mt(random());
	std::uniform_real_distribution<float> positionX(xMin, xMax);
	std::uniform_real_distribution<float> positionY(yMin, yMax);
	
	return sf::Vector2f(positionX(mt), positionY(mt));
}

sf::Vector2f setRandomDirection(float xMin, float xMax, float yMin, float yMax) {

	static std::random_device random;
	static std::mt19937 mt(random());
	std::uniform_real_distribution<float> directionX(xMin, xMax);
	std::uniform_real_distribution<float> directionY(yMin, yMax);

	return sf::Vector2f(directionX(mt), directionY(mt));
}

void updateDirection(sf::Vector2f& currentDirection, sf::CircleShape& smartEnemy, sf::RenderWindow& window) {

	const sf::Vector2u windowSize = window.getSize();
	const float radius = smartEnemy.getRadius();

	currentDirection = setRandomDirection(radius, (windowSize.x - radius), radius, (windowSize.y - radius));
}

void windowBorders(sf::CircleShape& entity, sf::RenderWindow& window) {

	sf::Vector2f entityPosition = entity.getPosition();
	const sf::Vector2u windowSize = window.getSize();
	const float radius = entity.getRadius();

	if (entityPosition.x > (windowSize.x - radius)) {

		entity.setPosition({ -radius, entityPosition.y });
	}
	else if (entityPosition.x < -radius) {

		entity.setPosition({ (windowSize.x - radius), entityPosition.y});
	}
	else if (entityPosition.y > (windowSize.y - radius)) {

		entity.setPosition({ entityPosition.x, -radius });
	}
	else if (entityPosition.y < -radius) {

		entity.setPosition({ entityPosition.x, (windowSize.y - radius) });
	}
}

void foodCode(sf::CircleShape& food, sf::CircleShape& player, int& score) {

	if (detectCollision(player, food)) {

		sf::Vector2f newPosition = setRandomPosition(40, 1160, 40, 760);
		food.setPosition(newPosition);
		score++;
	}
}

sf::Vector2f playerMovement(float deltaTime, float playerSpeed) {

	sf::Vector2f movement({ 0.f, 0.f });

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		movement.x -= playerSpeed * deltaTime;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		movement.x += playerSpeed * deltaTime;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
		movement.y -= playerSpeed * deltaTime;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
		movement.y += playerSpeed * deltaTime;
	}

	return movement;
}

sf::Vector2f enemyMovement(sf::CircleShape& enemy, const sf::CircleShape& player, float deltaTime, float enemySpeed) {

	sf::Vector2f direction = player.getPosition() - enemy.getPosition();
	float magnitude = std::sqrt(direction.x * direction.x + direction.y * direction.y);
	if (magnitude != 0) {

		direction /= magnitude;
	}

	sf::Vector2f movement = direction * enemySpeed * deltaTime;

	return movement;
}

sf::Vector2f targetDirectionMovement(sf::CircleShape& entity, sf::Vector2f direction, float deltaTime, float entitySpeed) {

	sf::Vector2f entityPosition = entity.getPosition();
	sf::Vector2f movementDirection = direction - entityPosition;

	float magnitude = std::sqrt(movementDirection.x * movementDirection.x + movementDirection.y * movementDirection.y);
	if (magnitude > 0) {

		movementDirection /= magnitude;
	}
	else {

		movementDirection = sf::Vector2f( 0.f, 0.f );
		// std::cout << "MovementDirection is set to (0.f, 0.f)" << std::endl;
	}

	sf::Vector2f movement = movementDirection * entitySpeed * deltaTime;

	return movement;
}

sf::Vector2f bulletTargetDirectionMovement(sf::CircleShape& bullet, sf::Vector2f target, float deltaTime, float speedMultiplier, float minSpeed, float maxSpeed) {

	sf::Vector2f bulletPosition = bullet.getPosition();

	sf::Vector2f difference = target - bulletPosition;

	float magnitude = std::sqrt(difference.x * difference.x + difference.y * difference.y);

	sf::Vector2f movementDirection = (magnitude > 0.5f) ? difference / magnitude : sf::Vector2f(0.f, 0.f);

	float rawSpeed = speedMultiplier * magnitude;

	float computedSpeed = std::clamp(rawSpeed, minSpeed, maxSpeed);

	sf::Vector2f movement = movementDirection * computedSpeed * deltaTime;

	/* std::cout << "DeltaTime: " << deltaTime << std::endl;
	std::cout << "MovementDirection: (" << movementDirection.x << ", " << movementDirection.y << ")" << std::endl;
	std::cout << "Magnitude: " << magnitude << std::endl;
	std::cout << "RawSpeed: " << rawSpeed << std::endl;
	std::cout << "ComputedSpeed: " << computedSpeed << std::endl;
	std::cout << "Movement: (" << movement.x << ", " << movement.y << ")" << std::endl; */

	return movement;
}

bool distanceDifference(sf::CircleShape& enemy, sf::CircleShape player, float distance) {

	sf::FloatRect enemyBounds = enemy.getGlobalBounds();
	sf::FloatRect playerBounds = player.getGlobalBounds();

	sf::Vector2f enemyCenter = enemyBounds.getCenter();
	sf::Vector2f playerCenter = playerBounds.getCenter();

	float realDistance = static_cast<float>(std::sqrt(std::pow(playerCenter.x - enemyCenter.x, 2) + std::pow(playerCenter.y - enemyCenter.y, 2)));

	if (realDistance <= distance) {

		return true;
	}
	else {

		return false;
	}
}

int main() {

	sf::Clock clock;
	sf::Font font("OpenSans-Regular.ttf");
	const float tolerance = 10.f;
	
	// Window
	sf::RenderWindow window(sf::VideoMode({ 1200, 800 }), "SMPG");
	window.setFramerateLimit(60);
	window.setKeyRepeatEnabled(false);

	// Player
	sf::CircleShape player(40.f);
	player.setFillColor(sf::Color::Blue);
	player.setPosition({ 600.f, 700.f });
	const float playerSpeed = 400.f;

	// Points
	sf::CircleShape food(20.f);
	food.setFillColor(sf::Color::Red);
	food.setPosition({ 600.f, 700.f });

	// Enemy
	sf::CircleShape enemy(60.f);
	enemy.setFillColor(sf::Color::Green);
	enemy.setPosition({ 600.f, 100.f });

	// Smart Enemy
	sf::CircleShape smartEnemy(70.f);
	smartEnemy.setFillColor(sf::Color::Cyan);
	smartEnemy.setPosition({ 600.f, 200.f });
	sf::Vector2f currentDirection({ 600.f, 200.f });
	
	// Bullet
	sf::CircleShape bullet(10.f);
	bullet.setFillColor(sf::Color::Yellow);
	bool bulletIsActive = false;
	sf::Vector2f bulletTargetPosition;

	// Score
	int score = -1;
	sf::Text scoreText(font);
	scoreText.setCharacterSize(40);
	scoreText.setFillColor(sf::Color::White);

	int highScore = 0;
	sf::Text highScoreText(font);
	highScoreText.setCharacterSize(70);
	highScoreText.setFillColor(sf::Color::White);
	highScoreText.setPosition({ 460.f, 100.f });

	// Main menu variables
	sf::Text playText(font);
	playText.setCharacterSize(80);
	playText.setString("Play");
	playText.setPosition({ 560.f, 340.f });

	sf::Text levelsText(font);
	levelsText.setCharacterSize(80);
	levelsText.setString("Levels");
	levelsText.setPosition({ 560.f, 430.f });

	sf::Text exitText(font);
	exitText.setCharacterSize(80);
	exitText.setString("Exit");
	exitText.setPosition({ 560.f, 520.f });

	int selectedOptionMainMenu = 0; // mainMenu = 0, play = 1, levels = 2, exit = 3
	const int totalOptionsMainMenu = 3;
	gameStateClass gameState = gameStateClass::mainMenu;

	// Levels variables
	sf::Text level1Text(font);
	level1Text.setCharacterSize(60);
	level1Text.setString("Level 1");
	level1Text.setPosition({ 560.f, 100.f });

	sf::Text level2Text(font);
	level2Text.setCharacterSize(60);
	level2Text.setString("Level 2");
	level2Text.setPosition({ 560.f, 200.f });

	sf::Text level3Text(font);
	level3Text.setCharacterSize(60);
	level3Text.setString("Level 3");
	level3Text.setPosition({ 560.f, 300.f });

	sf::Text level4Text(font);
	level4Text.setCharacterSize(60);
	level4Text.setString("Level 4");
	level4Text.setPosition({ 560.f, 400.f });

	sf::Text level5Text(font);
	level5Text.setCharacterSize(60);
	level5Text.setString("Level 5");
	level5Text.setPosition({ 560.f, 500.f });

	sf::Text levelsExitText(font);
	levelsExitText.setCharacterSize(60);
	levelsExitText.setString("Exit");
	levelsExitText.setPosition({ 560.f, 600.f });

	int selectedOptionLevelsMenu = 0;
	const int totalOptionsLevelsMenu = 5;
	levelsClass level = levelsClass::levelsDefault;

	int levelsScore = -1;
	
	// HighScore code for writing
	std::ifstream inFileHighScore("highScore.txt");
	if (inFileHighScore.is_open()) {

		if (!(inFileHighScore >> highScore)) {

			highScore = 0;
		}

		inFileHighScore.close();
	}
	else {

		// std::cout << "Failed to open highScore.txt for writing." << std::endl;
	}


	while (window.isOpen()) {

		// Variable that need to be updated while playing
		sf::Vector2f playerPosition = player.getPosition();
		sf::Vector2f enemyPosition = enemy.getPosition();
		sf::Vector2f smartEnemyPosition = smartEnemy.getPosition();
		sf::Vector2f bulletPosition = bullet.getPosition();
		float deltaTime = clock.restart().asSeconds();
		if (deltaTime > 0.1f) {

			deltaTime = 0.1f;
		}
		
		while (std::optional event = window.pollEvent()) {

			if (event->is<sf::Event::Closed>()) {

				window.close();
			}

			// Switching between options in the menu
			if (gameState == gameStateClass::mainMenu) {

				if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
					if (keyPressed->scancode == sf::Keyboard::Scan::Up) {
						if (selectedOptionMainMenu == 0) {
							selectedOptionMainMenu = 1;
						}
						else if (selectedOptionMainMenu == 1) {
							selectedOptionMainMenu = totalOptionsMainMenu;
						}
						else {
							selectedOptionMainMenu -= 1;
						}
					}
					else if (keyPressed->scancode == sf::Keyboard::Scan::Down) {
						if (selectedOptionMainMenu == 0) {
							selectedOptionMainMenu = 1;
						}
						else {
							selectedOptionMainMenu = (selectedOptionMainMenu % totalOptionsMainMenu) + 1;
						}
					}
					else if (keyPressed->scancode == sf::Keyboard::Scan::Enter) {
						if (selectedOptionMainMenu == 1) {
							gameState = gameStateClass::play;
						}
						else if (selectedOptionMainMenu == 2) {
							gameState = gameStateClass::levels;
						}
						else if (selectedOptionMainMenu == 3) {
							gameState = gameStateClass::exit;
						}
					}
				}
			}
			else if (gameState == gameStateClass::levels) {

				if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
					if (keyPressed->scancode == sf::Keyboard::Scan::Up) {
						if (selectedOptionLevelsMenu <= 0) {
							selectedOptionLevelsMenu = totalOptionsLevelsMenu;
						}
						else {
							selectedOptionLevelsMenu -= 1;
						}
					}
					else if (keyPressed->scancode == sf::Keyboard::Scan::Down) {
						if (selectedOptionLevelsMenu == 5) {
							selectedOptionLevelsMenu = 0;
						}
						else {
							selectedOptionLevelsMenu = (selectedOptionLevelsMenu % totalOptionsLevelsMenu) + 1;
						}
					}
					else if (keyPressed->scancode == sf::Keyboard::Scan::Enter) {
						if (selectedOptionLevelsMenu == 1) {
							level = levelsClass::level1;
						}
						else if (selectedOptionLevelsMenu == 2) {
							level = levelsClass::level2;
						}
						else if (selectedOptionLevelsMenu == 3) {
							level = levelsClass::level3;
						}
						else if (selectedOptionLevelsMenu == 4) {
							level = levelsClass::level4;
						}
						else if (selectedOptionLevelsMenu == 5) {
							level = levelsClass::level5;
						}
						else if (selectedOptionLevelsMenu == 0) {
							level = levelsClass::exit;
						}
					}
				}
			}

		}

		if (gameState == gameStateClass::mainMenu) {

			// std::cout << "Main menu state activated" << std::endl;
			// std::cout << "Player position (" << playerPosition.x << ", " << playerPosition.y << ")" << std::endl;

			if (selectedOptionMainMenu == 1) {

				playText.setFillColor(sf::Color::Red);
			}
			else {

				playText.setFillColor(sf::Color::White);
			}

			if (selectedOptionMainMenu == 2) {

				levelsText.setFillColor(sf::Color::Red);
			}
			else {

				levelsText.setFillColor(sf::Color::White);
			}

			if (selectedOptionMainMenu == 3) {

				exitText.setFillColor(sf::Color::Red);
			}
			else {
				
				exitText.setFillColor(sf::Color::White);
			}

			// HighScore code
			highScoreText.setString("HighScore: " + std::to_string(highScore));

			window.clear();
			window.draw(playText);
			window.draw(levelsText);
			window.draw(exitText);
			window.draw(highScoreText);
			window.display();
		
		}
		else if (gameState == gameStateClass::play) {

			/* std::cout << "Play state activated" << std::endl;
			std::cout << "Enemy position (" << enemyPosition.x << ", " << enemyPosition.y << ")" << std::endl;
			std::cout << "Player position (" << playerPosition.x << ", " << playerPosition.y << ")" << std::endl;
			std::cout << "Delta = " << deltaTime << std::endl; */

			// Player movement
			player.move(playerMovement(deltaTime, playerSpeed));

			// Player borders code
			windowBorders(player, window);

			// Food code
			foodCode(food, player, score);

			// Enemy code
			if (detectCollision(player, enemy)) {

				gameState = gameStateClass::mainMenu;
				player.setPosition({ 600.f, 700.f });
				food.setPosition({ 600.f, 700.f });
				enemy.setPosition({ 600.f, 100.f });
				score = -1;
			}

			enemy.move(enemyMovement(enemy, player, deltaTime, 200.f));

			// Score-HighScore code
			scoreText.setString("Score: " + std::to_string(score));

			if (highScore < score) {

				highScore = score;
				std::ofstream outFileHighScore("highScore.txt");
				if (outFileHighScore.is_open()) {

					outFileHighScore << highScore;
					outFileHighScore.close();
					// std::cout << "HighScore updated to: " << highScore << std::endl;
				}
				else {

					// std::cout << "Failed to open highScore.txt for writing." << std::endl;
				}
			}
			else {

				// std::cout << "Score isn't higher than highScore." << std::endl;
			}

			window.clear();
			window.draw(food);
			window.draw(player);
			window.draw(enemy);
			window.draw(scoreText);
			window.display();
		}
		else if (gameState == gameStateClass::levels) {

			// std::cout << "Levels state activated." << std::endl;

			// Level menu buttons coloring
			if (selectedOptionLevelsMenu == 1) {
				level1Text.setFillColor(sf::Color::Red);
			}
			else {
				level1Text.setFillColor(sf::Color::White);
			}

			if (selectedOptionLevelsMenu == 2) {
				level2Text.setFillColor(sf::Color::Red);
			}
			else {
				level2Text.setFillColor(sf::Color::White);
			}

			if (selectedOptionLevelsMenu == 3) {
				level3Text.setFillColor(sf::Color::Red);
			}
			else {
				level3Text.setFillColor(sf::Color::White);
			}

			if (selectedOptionLevelsMenu == 4) {
				level4Text.setFillColor(sf::Color::Red);
			}
			else {
				level4Text.setFillColor(sf::Color::White);
			}

			if (selectedOptionLevelsMenu == 5) {
				level5Text.setFillColor(sf::Color::Red);
			}
			else {
				level5Text.setFillColor(sf::Color::White);
			}

			if (selectedOptionLevelsMenu == 0) {
				levelsExitText.setFillColor(sf::Color::Red);
			}
			else {
				levelsExitText.setFillColor(sf::Color::White);
			}

			if (level == levelsClass::exit) {

				// std::cout << "Levels exit activated." << std::endl;

				gameState = gameStateClass::mainMenu;
				level = levelsClass::levelsDefault;
			}

			window.clear();
			window.draw(level1Text);
			window.draw(level2Text);
			window.draw(level3Text);
			window.draw(level4Text);
			window.draw(level5Text);
			window.draw(levelsExitText);
			window.display();
		}
		else if (gameState == gameStateClass::exit) {

			// std::cout << "Exit state activated" << std::endl;
			// std::cout << "HighScore: " << highScore << std::endl;

			window.close();
		}

		if (level == levelsClass::level1) {

			gameState = gameStateClass::gameStateDefault;

			// Player movement
			player.move(playerMovement(deltaTime, playerSpeed));

			// Player borders code
			windowBorders(player, window);

			// Food code
			foodCode(food, player, levelsScore);

			// Score code
			scoreText.setString("Score: " + std::to_string(levelsScore) + " Remaining: " + std::to_string(10 - levelsScore));

			if (levelsScore == 10) {

				level = levelsClass::levelsDefault;
				gameState = gameStateClass::levels;
				level1Text.setString("Level 1: Completed!");
				player.setPosition({ 600.f, 700.f });
				food.setPosition({ 600.f, 700.f });
				enemy.setPosition({ 600.f, 100.f });
				levelsScore = -1;
			}
			
			// Enemy code
			if (detectCollision(player, enemy)) {

				level = levelsClass::levelsDefault;
				gameState = gameStateClass::levels;
				player.setPosition({ 600.f, 700.f });
				food.setPosition({ 600.f, 700.f });
				enemy.setPosition({ 600.f, 100.f });
				levelsScore = -1;
			}

			enemy.move(enemyMovement(enemy, player, deltaTime, 150.f));

			window.clear();
			window.draw(food);
			window.draw(player);
			window.draw(enemy);
			window.draw(scoreText);
			window.display();
		}
		else if (level == levelsClass::level2) {

			gameState = gameStateClass::gameStateDefault;

			// Player movement
			player.move(playerMovement(deltaTime, playerSpeed));

			// Player borders code
			windowBorders(player, window);

			// Food code
			foodCode(food, player, levelsScore);

			// Score code
			scoreText.setString("Score: " + std::to_string(levelsScore) + " Remaining: " + std::to_string(15 - levelsScore));

			if (levelsScore == 15) {

				level = levelsClass::levelsDefault;
				gameState = gameStateClass::levels;
				level2Text.setString("Level 2: Completed!");
				player.setPosition({ 600.f, 700.f });
				food.setPosition({ 600.f, 700.f });
				enemy.setPosition({ 600.f, 100.f });
				levelsScore = -1;
			}

			// Enemy code
			if (detectCollision(player, enemy)) {

				level = levelsClass::levelsDefault;
				gameState = gameStateClass::levels;
				player.setPosition({ 600.f, 700.f });
				food.setPosition({ 600.f, 700.f });
				enemy.setPosition({ 600.f, 100.f });
				levelsScore = -1;
			}

			enemy.move(enemyMovement(enemy, player, deltaTime, 300.f));

			window.clear();
			window.draw(food);
			window.draw(player);
			window.draw(enemy);
			window.draw(scoreText);
			window.display();
		}
		else if (level == levelsClass::level3) {

			gameState = gameStateClass::gameStateDefault;

			// Player movement
			player.move(playerMovement(deltaTime, playerSpeed));

			// Player borders code
			windowBorders(player, window);

			// Food code
			foodCode(food, player, levelsScore);

			// Score code
			scoreText.setString("Score: " + std::to_string(levelsScore) + " Remaining: " + std::to_string(15 - levelsScore));

			if (levelsScore == 15) {

				level = levelsClass::levelsDefault;
				gameState = gameStateClass::levels;
				level3Text.setString("Level 3: Completed!");
				player.setPosition({ 600.f, 700.f });
				food.setPosition({ 600.f, 700.f });
				enemy.setPosition({ 600.f, 100.f });
				smartEnemy.setPosition({ 600.f, 200.f });
				currentDirection = sf::Vector2f( 600.f, 200.f );
				levelsScore = -1;
			}

			// Enemy code
			if (detectCollision(player, enemy)) {

				level = levelsClass::levelsDefault;
				gameState = gameStateClass::levels;
				player.setPosition({ 600.f, 700.f });
				food.setPosition({ 600.f, 700.f });
				enemy.setPosition({ 600.f, 100.f });
				smartEnemy.setPosition({ 600.f, 200.f });
				currentDirection = sf::Vector2f( 600.f, 200.f );
				levelsScore = -1;
			}

			enemy.move(enemyMovement(enemy, player, deltaTime, 250.f));

			// Smart Enemy code
			if (std::abs(smartEnemyPosition.x - currentDirection.x) < tolerance && std::abs(smartEnemyPosition.y - currentDirection.y) < tolerance) {
				updateDirection(currentDirection, smartEnemy, window);
			}

			smartEnemy.move(targetDirectionMovement(smartEnemy, currentDirection, deltaTime, 300.f));

			if (detectCollision(player, smartEnemy)) {

				level = levelsClass::levelsDefault;
				gameState = gameStateClass::levels;
				player.setPosition({ 600.f, 700.f });
				food.setPosition({ 600.f, 700.f });
				enemy.setPosition({ 600.f, 100.f });
				smartEnemy.setPosition({ 600.f, 200.f });
				currentDirection = sf::Vector2f(600.f, 200.f);
				levelsScore = -1;
			}

			window.clear();
			window.draw(food);
			window.draw(player);
			window.draw(smartEnemy);
			window.draw(enemy);
			window.draw(scoreText);
			window.display();
		}
		else if (level == levelsClass::level4) {

			gameState = gameStateClass::gameStateDefault;

			// Player movement
			player.move(playerMovement(deltaTime, playerSpeed));

			// Player borders code
			windowBorders(player, window);

			// Food code
			foodCode(food, player, levelsScore);

			// Score code
			scoreText.setString("Score: " + std::to_string(levelsScore) + " Remaining: " + std::to_string(20 - levelsScore));

			if (levelsScore == 20) {

				level = levelsClass::levelsDefault;
				gameState = gameStateClass::levels;
				level4Text.setString("Level 4: Completed!");
				player.setPosition({ 600.f, 700.f });
				food.setPosition({ 600.f, 700.f });
				enemy.setPosition({ 600.f, 100.f });
				smartEnemy.setPosition({ 600.f, 200.f });
				currentDirection = sf::Vector2f(600.f, 200.f);
				levelsScore = -1;
			}

			// Enemy code
			if (detectCollision(player, enemy)) {

				level = levelsClass::levelsDefault;
				gameState = gameStateClass::levels;
				player.setPosition({ 600.f, 700.f });
				food.setPosition({ 600.f, 700.f });
				enemy.setPosition({ 600.f, 100.f });
				smartEnemy.setPosition({ 600.f, 200.f });
				currentDirection = sf::Vector2f(600.f, 200.f);
				levelsScore = -1;
			}

			enemy.move(enemyMovement(enemy, player, deltaTime, 275.f));

			// Smart Enemy code
			if (std::abs(smartEnemyPosition.x - currentDirection.x) < tolerance && std::abs(smartEnemyPosition.y - currentDirection.y) < tolerance) {
				updateDirection(currentDirection, smartEnemy, window);
			}

			smartEnemy.move(targetDirectionMovement(smartEnemy, currentDirection, deltaTime, 400.f));

			if (detectCollision(player, smartEnemy)) {

				level = levelsClass::levelsDefault;
				gameState = gameStateClass::levels;
				player.setPosition({ 600.f, 700.f });
				food.setPosition({ 600.f, 700.f });
				enemy.setPosition({ 600.f, 100.f });
				smartEnemy.setPosition({ 600.f, 200.f });
				currentDirection = sf::Vector2f(600.f, 200.f);
				levelsScore = -1;
			}

			window.clear();
			window.draw(food);
			window.draw(player);
			window.draw(smartEnemy);
			window.draw(enemy);
			window.draw(scoreText);
			window.display();
		}
		else if (level == levelsClass::level5) {

			gameState = gameStateClass::gameStateDefault;

			// Player movement
			player.move(playerMovement(deltaTime, playerSpeed));

			// Player borders code
			windowBorders(player, window);

			// Food code
			foodCode(food, player, levelsScore);

			// Score code
			scoreText.setString("Score: " + std::to_string(levelsScore) + " Remaining: " + std::to_string(25 - levelsScore));

			if (levelsScore == 20) {

				level = levelsClass::levelsDefault;
				gameState = gameStateClass::levels;
				level5Text.setString("Level 5: Completed!");
				player.setPosition({ 600.f, 700.f });
				food.setPosition({ 600.f, 700.f });
				enemy.setPosition({ 600.f, 100.f });
				smartEnemy.setPosition({ 600.f, 200.f });
				currentDirection = sf::Vector2f(600.f, 200.f);
				bullet.setPosition(smartEnemyPosition);
				levelsScore = -1;
			}

			// Enemy code
			if (detectCollision(player, enemy)) {

				level = levelsClass::levelsDefault;
				gameState = gameStateClass::levels;
				player.setPosition({ 600.f, 700.f });
				food.setPosition({ 600.f, 700.f });
				enemy.setPosition({ 600.f, 100.f });
				smartEnemy.setPosition({ 600.f, 200.f });
				currentDirection = sf::Vector2f(600.f, 200.f);
				bullet.setPosition(smartEnemyPosition);
				levelsScore = -1;
			}

			enemy.move(enemyMovement(enemy, player, deltaTime, 300.f));

			// Smart Enemy code
			if (std::abs(smartEnemyPosition.x - currentDirection.x) < tolerance && std::abs(smartEnemyPosition.y - currentDirection.y) < tolerance) {
				updateDirection(currentDirection, smartEnemy, window);
			}

			smartEnemy.move(targetDirectionMovement(smartEnemy, currentDirection, deltaTime, 300.f));

			if (detectCollision(player, smartEnemy)) {

				level = levelsClass::levelsDefault;
				gameState = gameStateClass::levels;
				player.setPosition({ 600.f, 700.f });
				food.setPosition({ 600.f, 700.f });
				enemy.setPosition({ 600.f, 100.f });
				smartEnemy.setPosition({ 600.f, 200.f });
				currentDirection = sf::Vector2f(600.f, 200.f);
				bullet.setPosition(smartEnemyPosition);
				levelsScore = -1;
			}

			// Bullet code
			if (distanceDifference(smartEnemy, player, 800.f)) {
				if (!bulletIsActive) {
					bulletIsActive = true;
					bulletTargetPosition = playerPosition;

					// Set the bullet position after target assignement
					bullet.setPosition(smartEnemyPosition);
					// std::cout << "Bullet has a target position." << std::endl;
				}

				if (bulletIsActive) {
					sf::Vector2f bulletMovement = bulletTargetDirectionMovement(bullet, bulletTargetPosition, deltaTime, 1.f, 400.f, 800.f);
					bullet.move(bulletMovement);
					bulletPosition = bullet.getPosition();
					// std::cout << "Bullet moved by (" << bulletMovement.x << ", " << bulletMovement.y << ")" << std::endl;
					// std::cout << "Bullet position: (" << bulletPosition.x << ", " << bulletPosition.y << ")" << std::endl;
				}

				// Deactivate the bullet after it reaches the target
				if (std::hypot(bulletPosition.x - bulletTargetPosition.x, bulletPosition.y - bulletTargetPosition.y) < tolerance) {
					bullet.setPosition({ -1000.f, -1000.f });
					bulletIsActive = false;
					// std::cout << "Bullet deactivated." << std::endl;
				}
			}
			else {
				bullet.setPosition({ -1000.f, -1000.f });
				bulletIsActive = false;
			}

			if (detectCollision(player, bullet)) {

				level = levelsClass::levelsDefault;
				gameState = gameStateClass::levels;
				player.setPosition({ 600.f, 700.f });
				food.setPosition({ 600.f, 700.f });
				enemy.setPosition({ 600.f, 100.f });
				smartEnemy.setPosition({ 600.f, 200.f });
				currentDirection = sf::Vector2f(600.f, 200.f);
				bullet.setPosition(smartEnemyPosition);
				levelsScore = -1;
			}
			
			window.clear();
			window.draw(food);
			window.draw(player);
			window.draw(smartEnemy);
			window.draw(enemy);
			window.draw(bullet);
			window.draw(scoreText);
			window.display();
		}

	}

	return 0;
}