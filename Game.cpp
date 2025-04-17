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
	this->enemy.setPosition(0.f, 0.f);
	this->enemy.setSize(sf::Vector2f(100.f, 100.f));
	this->enemy.setFillColor(sf::Color::Cyan);
	//this->enemy.setOutlineColor(sf::Color::Green);
	//this->enemy.setOutlineThickness(1.f);

	//Load high score on startup
	std::ifstream inFile("highscore.txt");
	if (inFile.is_open())
	{
		inFile >> this->highScore >> this->bestAccuracy;
		inFile.close();
	}
	else
	{
		this->highScore = 0;
		this->bestAccuracy = 0.f;
	}
}

void Game::initWindow()
{
	this->videoMode.height = 920;
	this->videoMode.width = 1080;
	//this->videoMode.getDesktopMode; // optional if you dont want a auto custom window
	this->window = new sf::RenderWindow(this->videoMode, "Game 1", sf::Style::Titlebar | sf::Style::Close);

	this->window->setFramerateLimit(80);
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
}

Game::~Game()
{
	delete this->window;
}

//Accessors
const bool Game::getWindowIsOpen() const
{
	return this->window->isOpen();
}

const bool Game::getEndGame() const
{
	return this ->endGame;
}

//Functions
void Game::spawnEnemy()
{
	this->enemy.setPosition(
		static_cast<float>(rand() % static_cast<int>(this->window->getSize().x - this->enemy.getSize().x)),
		0.f
	);

	//Randomize enemy type
	int type = rand() % 6; // increase range to 6 to include health block
	if (type == 5)
	{
		// Health block
		sf::RectangleShape healthBlock;
		healthBlock.setSize(sf::Vector2f(30.f, 30.f));
		healthBlock.setFillColor(sf::Color::White);
		healthBlock.setPosition(
			static_cast<float>(rand() % static_cast<int>(this->window->getSize().x - 30)),
			0.f
		);
		this->healthBlocks.push_back(healthBlock);
		return; // Don't spawn a regular enemy this time
	}

	switch (type)
	{
	case 0:
		this->enemy.setSize(sf::Vector2f(10.f, 10.f));
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

	this->enemies.push_back(this->enemy);
}

void Game::pollEvents()
{
	//Event polling
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
			i--;
		}
	}

	// Enemies
	for (int i = 0; i < this->enemies.size(); i++)
	{
		this->enemies[i].move(0.f, 5.f);

		if (this->enemies[i].getPosition().y > this->window->getSize().y)
		{
			this->enemies.erase(this->enemies.begin() + i);
			this->health -= 1;
			std::cout << "Health: " << this->health << "\n";
		}
	}
	//Check if clicked on
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		if (this->mouseHeld == false)
		{
			this->mouseHeld = true;
			this->shotsFired++;

			// Check health blocks
			for (size_t i = 0; i < this->healthBlocks.size(); i++)
			{
				if (this->healthBlocks[i].getGlobalBounds().contains(this->mousePosView))
				{
					this->health = std::min(this->health + 1, 20); // Heal max 20
					std::cout << "Healed! Health: " << this->health << "\n";
					this->healthBlocks.erase(this->healthBlocks.begin() + i);
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
	else
	{
		this->mouseHeld = false;
	}
}

void Game::update()
{
	this->pollEvents();

	if (this->endGame == false)
	{
		this->updateMousePositions();

		this->updateText();

		this->updateEnemies();

	}

	//Endgame condition
	if (this->health <= 0)
	{
		this->endGame = true;

		// Update best accuracy
		if (this->accuracy > this->bestAccuracy)
			this->bestAccuracy = this->accuracy;

		// Check if the player beat the high score
		if (this->points > this->highScore)
		{
			this->highScore = this->points;

			// Save new high score to file
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

void Game::render()
{
	/*
		@return void
	
		 -clear old frames
		 -clear the game objects
		 -display frame in window

		 -Renders the game objects
	*/

	this->window->clear(sf::Color(0, 0, 0, 255));

	//draw game objects
	this->renderEnemies(*this->window);

	this->renderText(*this->window);

	//draw accuracy bar before display()
	this->window->draw(this->accuracyBarBack);
	this->window->draw(this->accuracyBar);

	this->window->display();
}
