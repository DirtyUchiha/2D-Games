#include "Game.h"

//Private functions
void Game::initVariables()
{
	this->window = nullptr;

	//Game logic
	this->endGame = false;
	this->points = 0;
	this->health = 20;
	this->enemySpawnTimerMax = 20.f;
	this->enemySpawnTimer = this->enemySpawnTimerMax;
	this->maxEnemies = 5;
	this->mouseHeld = false;
	this->shotsFired = 0;
	this->shotsHit = 0;
	this->accuracy = 0.f;
	this->bestAccuracy = 0.f;
	this->highScore = 0;

	this->accuracyBarBack.setSize(sf::Vector2f(200.f, 20.f));
	this->accuracyBarBack.setFillColor(sf::Color(50, 50, 50, 200));
	this->accuracyBarBack.setPosition(0.f, 160.f);

	this->accuracyBar.setSize(sf::Vector2f(200.f, 20.f));
	this->accuracyBar.setFillColor(sf::Color::Green);
	this->accuracyBar.setPosition(0.f, 160.f);

	//Game objects
	this->enemy.setSize(sf::Vector2f(100.f, 100.f));
	this->enemy.setFillColor(sf::Color::Cyan);
	this->enemy.setPosition(0.f, 0.f);
	//this->enemy.setOutlineColor(sf::Color::Green);
	//this->enemy.setOutlineThickness(1.f);

		// Game state
	this->gameState = MENU;

	// Start Button Setup
	this->startButton.setSize(sf::Vector2f(200.f, 60.f));
	this->startButton.setFillColor(sf::Color(100, 100, 255));
	this->startButton.setPosition(
		this->videoMode.width / 2.f - this->startButton.getSize().x / 2.f,
		this->videoMode.height / 2.f - this->startButton.getSize().y / 2.f
	);

	this->startButtonText.setFont(this->font);
	this->startButtonText.setString("START");
	this->startButtonText.setCharacterSize(28);
	this->startButtonText.setFillColor(sf::Color::White);

	// Center the text on the button
	sf::FloatRect textBounds = this->startButtonText.getLocalBounds();
	this->startButtonText.setOrigin(textBounds.width / 2, textBounds.height / 2);
	this->startButtonText.setPosition(
		this->startButton.getPosition().x + this->startButton.getSize().x / 2.f,
		this->startButton.getPosition().y + this->startButton.getSize().y / 2.f - 5.f
	);

	//Load high score on startup
	std::ifstream inFile("highscore.txt");
	if (inFile.is_open())
	{
		if (!(inFile >> this->highScore >> this->bestAccuracy))
		{
			// File exists but data is invalid
			this->highScore = 0;
			this->bestAccuracy = 0.f;
		}
		inFile.close();
	}
	else
	{
		// File doesn't exist yet
		this->highScore = 0;
		this->bestAccuracy = 0.f;
	}
}

void Game::initWindow()
{
	this->videoMode.height = 920;
	this->videoMode.width = 1080;
	//this->videoMode.getDesktopMode; // optional if you dont want a auto custom window
	this->window = new sf::RenderWindow(this->videoMode, "Aim trainer", sf::Style::Titlebar | sf::Style::Close);
	this->window->setFramerateLimit(60);
	this->window->setVerticalSyncEnabled(true); // Optional, but can help with performance
	this->window->setMouseCursorGrabbed(true);
}

void Game::initFonts()
{
	if (!this->font.loadFromFile("Fonts/Feelin Teachy TTF.ttf"))
	{
		std::cout << "ERROR::GAME::INITFONTS::Failed to load font!" << "\n";
	}
}

void Game::initText()
{
	this->uiText.setFont(this->font);
	this->uiText.setCharacterSize(24);
	this->uiText.setFillColor(sf::Color::White);
	this->uiText.setString("NONE");
}

void Game::initEnemies()
{
	this->enemy.setPosition(10.f, 10.f);
	this->enemy.setSize(sf::Vector2f(100.f, 100.f));
	this->enemy.setFillColor(sf::Color::Cyan);
	//this->enemy.setOutlineColor(sf::Color::Green);
	//this->enemy.setOutlineThickness(1.f);
}

//Constructors / Destructors
Game::Game()
{
	this->initVariables();
	this->initWindow();
	this->initFonts();
	this->initText();
	this->initEnemies();
	this->gameState = MENU;
}

Game::~Game()
{
	delete this->window;
}

//Accessors
bool Game::getWindowIsOpen() const
{
	return this->window->isOpen();
}

bool Game::getEndGame() const
{
	return this ->endGame;
}

//Functions
void Game::spawnEnemy()
{
	//Randomize enemy type
	int type = rand() % 6; // increase range to 6 to include health block
	if (type == 5)
	{
		// Health block
		float blockSize = 30.f;
		float x = static_cast<float>(rand() % static_cast<int>(this->window->getSize().x - static_cast<int>(blockSize)));

		sf::RectangleShape healthBlock;
		healthBlock.setSize(sf::Vector2f(30.f, 30.f));
		healthBlock.setFillColor(sf::Color::White);
		healthBlock.setPosition(x, 0.f);

		this->healthBlocks.push_back(healthBlock);
		return; // Don't spawn a regular enemy this time
	}

	//set enemy type
	switch (type)
	{
	case 0:
		this->enemy.setSize(sf::Vector2f(18.f, 18.f));
		this->enemy.setFillColor(sf::Color::Magenta);
		break;
	case 1:
		this->enemy.setSize(sf::Vector2f(30.f, 30.f));
		this->enemy.setFillColor(sf::Color::Blue);
		break;
	case 2:
		this->enemy.setSize(sf::Vector2f(50.f, 50.f));
		this->enemy.setFillColor(sf::Color::Cyan);
		break;
	case 3:
		this->enemy.setSize(sf::Vector2f(70.f, 70.f));
		this->enemy.setFillColor(sf::Color::Red);
		break;
	case 4:
		this->enemy.setSize(sf::Vector2f(100.f, 100.f));
		this->enemy.setFillColor(sf::Color::Green);
		break;
	}

	//Position the enemy within window bounds
	float enemyWidth = this->enemy.getSize().x;
	float x = static_cast<float>(rand() % static_cast<int>(this->window->getSize().x - static_cast<int>(enemyWidth)));
	this->enemy.setPosition(x, 0.f);

	this->enemies.push_back(this->enemy);
}

void Game::pollEvents()
{
	while (this->window->pollEvent(this->ev))
	{
		switch (this->ev.type)
		{
		case sf::Event::Closed:
			this->window->close();
			break;

		case sf::Event::KeyPressed:
			if (ev.key.code == sf::Keyboard::Escape)
				this->window->close();
			break;

		case sf::Event::MouseButtonPressed:
			if (this->ev.mouseButton.button == sf::Mouse::Left)
			{
				// Only check for start button clicks here, NOT shooting
				if (this->gameState == MENU &&
					this->startButton.getGlobalBounds().contains(this->mousePosView))
				{
					this->gameState = GAME;
				}
			}
			break;

		case sf::Event::MouseButtonReleased:
			if (this->ev.mouseButton.button == sf::Mouse::Left)
				this->mouseHeld = false;  // Allow re-clicking next frame
			break;
		}
	}
}

void Game::updateMousePositions()
{
	/**
	@ return void

	Updates the mouse positions:
		- Mouse position realtive to window (Vector2i)
	*/

	this->mousePosWindow = sf::Mouse::getPosition(*this->window);
	this->mousePosView = this->window->mapPixelToCoords(this->mousePosWindow);
}

void Game::updateText()
{
	std::stringstream ss;

	this->accuracy = (this->shotsFired > 0) ?
		static_cast<float>(this->shotsHit) / this->shotsFired * 100.f : 0.f;

	ss << "Points: " << this->points << "\n"
		<< "Health: " << this->health << "\n"
		<< "Accuracy: " << static_cast<int>(this->accuracy) << "%\n"
		<< "Best Accuracy: " << static_cast<int>(this->bestAccuracy) << "%\n"
		<< "High Score: " << this->highScore << "\n";

	float percent = (this->accuracy / 100.f);
	this->accuracyBar.setSize(sf::Vector2f(200.f * percent, 20.f));

	if (this->accuracy > 80.f)
	{
		this->accuracyBar.setFillColor(sf::Color::Green);
		this->uiText.setFillColor(sf::Color::Green);
	}
	else if (this->accuracy > 50.f)
	{
		this->accuracyBar.setFillColor(sf::Color::Yellow);
		this->uiText.setFillColor(sf::Color::Yellow);
	}
	else
	{
		this->accuracyBar.setFillColor(sf::Color::Red);
		this->uiText.setFillColor(sf::Color::Red);
	}

	this->uiText.setString(ss.str());
}

void Game::updateEnemies()
{
	/**
		@return void

		Updates the enemy spawn timer and spawns enemies
		when the total enemies is smaller than the maximum.
		Moves the enemies downwards
		Removes the enemies at the edge of the screen.
	*/

	//Updating the timer for enemy spawning
	if (this->enemies.size() < this->maxEnemies)
	{
		if (this->enemySpawnTimer >= this->enemySpawnTimerMax)
		{
			//spawn the enemy and reset the timer
			this->spawnEnemy();
			this->enemySpawnTimer = 0.f;
		}
		else
			this->enemySpawnTimer += 1.f;
	}

	// Health blocks
	for (size_t i = 0; i < this->healthBlocks.size(); i++)
	{
		this->healthBlocks[i].move(0.f, 4.f);

		if (this->healthBlocks[i].getPosition().y > this->window->getSize().y)
		{
			this->healthBlocks.erase(this->healthBlocks.begin() + i);
			--i;
		}
	}

	// Move Enemies
	for (int i = 0; i < this->enemies.size(); i++)
	{
		this->enemies[i].move(0.f, 5.f);
		if (this->enemies[i].getPosition().y > this->window->getSize().y)
		{
			this->enemies.erase(this->enemies.begin() + i);
			this->health --;
			--i;
		}
	}
	//Check if clicked on
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !this->mouseHeld)
		{
			this->mouseHeld = true;
			this->shotsFired++;

			// Check health blocks
			for (size_t i = 0; i < this->healthBlocks.size(); i++)
			{
				if (this->healthBlocks[i].getGlobalBounds().contains(this->mousePosView))
				{
					this->health = std::min(this->health + 1, 20); // Heal max 20
					this->healthBlocks.erase(this->healthBlocks.begin() + i);
					std::cout << "Healed! Health: " << this->health << "\n";
					return; // skip checking other enemies if you hit a health block
				}
			}

			for (size_t i = 0; i < this->enemies.size(); i++)
			{
				if (this->enemies[i].getGlobalBounds().contains(this->mousePosView))
				{
					this->shotsHit++;
					//Gain points
					if(this->enemies[i].getFillColor() == sf::Color::Magenta)
					this->points += 10;
					else if (this->enemies[i].getFillColor() == sf::Color::Blue)
						this->points += 7;
					else if (this->enemies[i].getFillColor() == sf::Color::Cyan)
						this->points += 5;
					else if (this->enemies[i].getFillColor() == sf::Color::Red)
						this->points += 3;
					else if (this->enemies[i].getFillColor() == sf::Color::Green)
						this->points += 1;

					std::cout << "Points: " << this->points << "\n";

					//Delete the enemy
					this->enemies.erase(this->enemies.begin() + i);
					break; // Exit the loop once the enemy is hit
				}
			}
		}
	}

void Game::update()
{
	this->pollEvents();
	this->updateMousePositions(); // Always update mouse position for button click detection

	if (this->gameState == GAME && this->endGame == false)
	{
		this->updateText();
		this->updateEnemies();
	}

	if (this->gameState == GAME && this->health <= 0)
	{
		this->endGame = true;

		if (this->accuracy > this->bestAccuracy)
			this->bestAccuracy = this->accuracy;

		if (this->points > this->highScore)
		{
			this->highScore = this->points;
			std::ofstream outFile("highscore.txt");
			if (outFile.is_open())
			{
				outFile << this->highScore << " " << this->bestAccuracy;
				outFile.close();
			}
		}
	}
}

void Game::renderText(sf::RenderTarget& target)
{
	target.draw(this->uiText);
}


void Game::renderEnemies(sf::RenderTarget& target)
{
	//Rendering all the enemies
	for (auto &e : this->enemies)
	{
		target.draw(e);
	}

	// Draw health blocks
	for (auto& h : this->healthBlocks)
	{
		target.draw(h);

		// Create plus sign text
		sf::Text plusText;
		plusText.setFont(this->font);
		plusText.setString("+");
		plusText.setCharacterSize(40);
		plusText.setFillColor(sf::Color::Red);

		// Center the plus sign on the health block
		sf::FloatRect hBounds = h.getGlobalBounds();
		sf::FloatRect tBounds = plusText.getGlobalBounds();

		//Set origin of the text to its center
		plusText.setOrigin(tBounds.width / 2.f, tBounds.height / 2.f);

		//Set the position of the text to the center of the health block
		plusText.setPosition(
			hBounds.left + hBounds.width / 2.1f,   // Horizontal center
			hBounds.top + hBounds.height / 80.f     // Vertical center
		);

		//draw the plus sign text
		target.draw(plusText);
	}
}

void Game::renderMenu(sf::RenderTarget& target)
{
	// Draw the start button and its text
	target.draw(this->startButton);
	target.draw(this->startButtonText);
}

void Game::render()
{
	// Clear old frames
	this->window->clear(sf::Color(0, 0, 0, 255));

	// Check the game state
	if (this->gameState == MENU)
	{
		// Render the menu (including the start button)
		this->window->draw(this->startButton);
		this->window->draw(this->startButtonText);
	}
	else if (this->gameState == GAME)
	{
		// Render the game objects (enemies, health blocks, etc.)
		this->renderEnemies(*this->window);

		// Render text (score, accuracy, etc.)
		this->renderText(*this->window);

		// Render the accuracy bar
		this->window->draw(this->accuracyBarBack);
		this->window->draw(this->accuracyBar);
	}

	// Display the frame in the window
	this->window->display();
}

