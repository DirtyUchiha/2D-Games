#include "Game.h"

//Private functions
void Game::initVariables()
{
	// Other initializations...

	//Set initial game state
	this->gameState = MENU;  // Start in menu state
	this->isPaused = false;
	this->window = nullptr;

	// Initialize pause overlay
	this->pauseOverlay.setSize(sf::Vector2f(this->videoMode.width, this->videoMode.height));
	this->pauseOverlay.setFillColor(sf::Color(0, 0, 0, 150));  // Semi-transparent black

	// Initialize pause button (simple square in the top-right corner)
	this->pauseButton.setSize(sf::Vector2f(100.f, 50.f));
	this->pauseButton.setFillColor(sf::Color(100, 100, 255));
	this->pauseButton.setPosition(this->videoMode.width - 120.f, 20.f);  // Top-right corner

	// Initialize pause button text
	this->pauseButtonText.setFont(this->font);
	this->pauseButtonText.setString("Pause");
	this->pauseButtonText.setCharacterSize(30);
	this->pauseButtonText.setFillColor(sf::Color::White);

	// Center the text on the button
	sf::FloatRect pauseTextBounds = this->pauseButtonText.getLocalBounds();
	this->pauseButtonText.setOrigin(
		pauseTextBounds.left + pauseTextBounds.width / 2.f,
		pauseTextBounds.top + pauseTextBounds.height / 2.f
	);

	this->pauseButtonText.setPosition(
		this->pauseButton.getPosition().x + this->pauseButton.getSize().x / 2.f,
		this->pauseButton.getPosition().y + this->pauseButton.getSize().y / 2.f
	);

	// Initialize exit button (perfectly centered)
	this->exitButton.setSize(sf::Vector2f(200.f, 50.f));
	this->exitButton.setFillColor(sf::Color(255, 0, 0));  // Red button
	this->exitButton.setPosition(
		(1080.f - 200.f) / 2.f,  // X = 440 (centered horizontally)
		(920.f - 50.f) / 2.f     // Y = 435 (centered vertically)
	);

	// Exit button text (perfectly centered on button)
	this->exitButtonText.setFont(this->font);
	this->exitButtonText.setString("Exit");
	this->exitButtonText.setCharacterSize(32);
	this->exitButtonText.setFillColor(sf::Color::White);

	// Center the text inside the button
	sf::FloatRect exitButtonTextBounds = this->exitButtonText.getLocalBounds();
	this->exitButtonText.setOrigin(
		exitButtonTextBounds.width / 2.f,
		exitButtonTextBounds.height / 2.f
	);
	this->exitButtonText.setPosition(
		this->exitButton.getPosition().x + this->exitButton.getSize().x / 2.f,
		this->exitButton.getPosition().y + this->exitButton.getSize().y / 2.f
	);

	// Initialize resume button (centered above exit button)
	this->resumeButton.setSize(sf::Vector2f(200.f, 50.f));
	this->resumeButton.setFillColor(sf::Color(0, 255, 0));  // Green button
	this->resumeButton.setPosition(
		(1080.f - 200.f) / 2.f,      // Same X as exit button
		(920.f - 50.f) / 2.f - 70.f  // 70px above exit button (Y = 365)
	);

	// Resume button text (centered on button)
	this->resumeButtonText.setFont(this->font);
	this->resumeButtonText.setString("Resume");
	this->resumeButtonText.setCharacterSize(32);
	this->resumeButtonText.setFillColor(sf::Color::White);

	// Center the text inside the button
	sf::FloatRect resumeButtonTextBounds = this->resumeButtonText.getLocalBounds();
	this->resumeButtonText.setOrigin(
		resumeButtonTextBounds.width / 2.f,
		resumeButtonTextBounds.height / 2.f
	);
	this->resumeButtonText.setPosition(
		this->resumeButton.getPosition().x + this->resumeButton.getSize().x / 2.f,
		this->resumeButton.getPosition().y + this->resumeButton.getSize().y / 2.f
	);

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
	this->accuracyBarBack.setPosition(0.f, 180.f);

	this->accuracyBar.setSize(sf::Vector2f(200.f, 20.f));
	this->accuracyBar.setFillColor(sf::Color::Green);
	this->accuracyBar.setPosition(0.f, 180.f);

	//Game objects
	this->enemy.setSize(sf::Vector2f(100.f, 100.f));
	this->enemy.setFillColor(sf::Color::Cyan);
	this->enemy.setPosition(0.f, 0.f);
	//this->enemy.setOutlineColor(sf::Color::Green);
	//this->enemy.setOutlineThickness(1.f);

	// Start Button Setup
	this->startButton.setSize(sf::Vector2f(200.f, 60.f));
	this->startButton.setFillColor(sf::Color(100, 100, 255));
	this->startButton.setPosition(
		this->videoMode.width / 920.f - this->startButton.getSize().x / 2.f,
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

	//Enhanced Load high score on startup
	std::ifstream inFile("highscore.txt");
	if (inFile.is_open())
	{
		std::string line;
		if (std::getline(inFile, line)) {
			std::istringstream iss(line);
			if (!(iss >> this->highScore >> this->bestAccuracy)) {
				// File exists but data is invalid
				this->highScore = 0;
				this->bestAccuracy = 0.f;
			}
			// Validate loaded values
			if (this->highScore < 0) this->highScore = 0;
			if (this->bestAccuracy < 0.f) this->bestAccuracy = 0.f;
			if (this->bestAccuracy > 100.f) this->bestAccuracy = 100.f;
		}
		inFile.close();
	}
	else
	{
		// File doesn't exist - create with default values
		this->highScore = 0;
		this->bestAccuracy = 0.f;

		std::ofstream outFile("highscore.txt");
		if (outFile.is_open()) {
			outFile << this->highScore << " " << this->bestAccuracy;
			outFile.close();
		}
		else {
			std::cerr << "Warning: Could not create highscore.txt\n";
		}
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
		// Load a basic SFML default font as fallback
		if (!this->font.loadFromFile("arial.ttf")) {
			// If even fallback fails, create a default font
			this->font = sf::Font();
		}
	}
}

void Game::initText()
{
	this->uiText.setFont(this->font);
	this->uiText.setCharacterSize(24);
	this->uiText.setFillColor(sf::Color::White); // Set to visible color
	this->uiText.setString(""); // Empty string
	this->uiText.setPosition(10.f, 10.f); // Set a proper position
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
	return this->endGame;
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

//Updated updateEnemies function with health block collision
void Game::updateEnemies()
{
	// Updating the timer for enemy spawning
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
		this->healthBlocks[i].move(0.f, 4.f); //Update position
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
			this->health--;
			--i;
		}
	}

	// Check if clicked on
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !this->mouseHeld)
	{
		this->mouseHeld = true;
		this->shotsFired++;

		// Check health blocks
		for (size_t i = 0; i < this->healthBlocks.size(); i++)
		{
			// Create expanded bounds for better click detection
			sf::FloatRect bounds = this->healthBlocks[i].getGlobalBounds();
			bounds.top -= 5.f;      // Move top edge up by 5 pixels
			bounds.height += 5.f;   // Increase height by 5 pixels

			if (bounds.contains(this->mousePosView))
			{
				this->health = std::min(this->health + 1, 20); // Heal max 20
				this->healthBlocks.erase(this->healthBlocks.begin() + i);
				std::cout << "Healed! Health: " << this->health << "\n";
				return; // skip checking other enemies if you hit a health block
			}
		}

		for (size_t i = 0; i < this->enemies.size(); i++)
		{
			// Create expanded bounds for better click detection
			sf::FloatRect bounds = this->enemies[i].getGlobalBounds();
			bounds.top -= 5.f;      // Move top edge up by 5 pixels
			bounds.height += 5.f;   // Increase height by 5 pixels

			if (bounds.contains(this->mousePosView))
			{
				this->shotsHit++;
				// Gain points based on the enemy color
				if (this->enemies[i].getFillColor() == sf::Color::Magenta)
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

				// Delete the enemy
				this->enemies.erase(this->enemies.begin() + i);
				break; // Exit the loop once the enemy is hit
			}
		}
	}
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
			// Toggle pause with ESC key
			if (this->ev.key.code == sf::Keyboard::Escape &&
				this->gameState == GAME)
			{
				this->isPaused = !this->isPaused;
			}
			break;

		case sf::Event::MouseButtonPressed:
			if (this->ev.mouseButton.button == sf::Mouse::Left)
			{
				// Menu screen button
				if (this->gameState == MENU &&
					this->startButton.getGlobalBounds().contains(this->mousePosView))
				{
					this->gameState = GAME;
					this->isPaused = false;
					this->endGame = false;
				}

				// Pause button (top-right during gameplay)
				if (this->gameState == GAME &&
					!this->isPaused &&
					this->pauseButton.getGlobalBounds().contains(this->mousePosView))
				{
					this->isPaused = true;
				}

				// Resume button (in pause menu)
				if (this->isPaused &&
					this->resumeButton.getGlobalBounds().contains(this->mousePosView))
				{
					this->isPaused = false;
				}

				// Exit button (in pause menu)
				if (this->isPaused &&
					this->exitButton.getGlobalBounds().contains(this->mousePosView))
				{
					this->window->close();
				}
			}
			break;

		case sf::Event::MouseButtonReleased:
			if (this->ev.mouseButton.button == sf::Mouse::Left)
				this->mouseHeld = false;
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
		<< "Accuracy: " << std::fixed << std::setprecision(1) << this->accuracy << "%\n"
		<< "Best Accuracy: " << std::fixed << std::setprecision(1) << this->bestAccuracy << "%\n"
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

void Game::update()
{
	this->pollEvents();
	this->updateMousePositions(); // Always update for button interaction

	if (!this->isPaused)  // Only update gameplay logic when unpaused
	{
		if (this->gameState == GAME && !this->endGame)
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
}

void Game::renderText(sf::RenderTarget& target)
{
	target.draw(this->uiText);
}


void Game::renderEnemies(sf::RenderTarget& target)
{
	// Rendering all the enemies
	for (auto& e : this->enemies)
	{
		target.draw(e);

		// Debug visualization - draw hitbox
		sf::RectangleShape debugRect;
		sf::FloatRect bounds = e.getGlobalBounds();
		// Apply the same expansion as in updateEnemies
		bounds.top -= 5.f;
		bounds.height += 5.f;

		debugRect.setPosition(bounds.left, bounds.top);
		debugRect.setSize(sf::Vector2f(bounds.width, bounds.height));
		debugRect.setFillColor(sf::Color::Transparent);
		debugRect.setOutlineColor(sf::Color::White);
		debugRect.setOutlineThickness(1.f);
		target.draw(debugRect);
	}

	// Draw health blocks and their "+" signs
	for (auto& h : this->healthBlocks)
	{
		target.draw(h);

		// Debug visualization - draw hitbox
		sf::RectangleShape debugRect;
		sf::FloatRect bounds = h.getGlobalBounds();
		// Apply the same expansion as in updateEnemies
		bounds.top -= 5.f;
		bounds.height += 5.f;

		debugRect.setPosition(bounds.left, bounds.top);
		debugRect.setSize(sf::Vector2f(bounds.width, bounds.height));
		debugRect.setFillColor(sf::Color::Transparent);
		debugRect.setOutlineColor(sf::Color::Yellow);
		debugRect.setOutlineThickness(1.f);
		target.draw(debugRect);

		// Create and position the plus sign
		sf::Text plusText;
		plusText.setFont(this->font);
		plusText.setString("+");
		plusText.setCharacterSize(24);
		plusText.setFillColor(sf::Color::Red);

		sf::FloatRect healthBlockBounds = h.getGlobalBounds();
		plusText.setPosition(healthBlockBounds.left + healthBlockBounds.width / 2.f - plusText.getLocalBounds().width / 2.f,
			healthBlockBounds.top + healthBlockBounds.height / 2.f - plusText.getLocalBounds().height / 2.f);

		target.draw(plusText);
	}
}

void Game::renderMenu(sf::RenderTarget& target)
{
	if (this->gameState == MENU)
	{
		// Render start button, etc. for menu state
		target.draw(this->startButton);
		target.draw(this->startButtonText);
	}

	if (this->gameState == GAME && !this->isPaused)
	{
		// Rendering game objects
		target.draw(this->accuracyBarBack);
		target.draw(this->accuracyBar);
		target.draw(this->uiText);

		// Render enemies and health blocks
		renderEnemies(target);
	}

	if (this->isPaused)
	{
		// Render pause overlay
		target.draw(this->pauseOverlay);

		// Render the resume button
		target.draw(this->resumeButton);
		target.draw(this->resumeButtonText);

		// Render exit button
		target.draw(this->exitButton);
		target.draw(this->exitButtonText);
	}
}

void Game::updateUI()
{
	// Skip UI updates if the game is paused
	if (this->isPaused)
		return;

	// Calculate accuracy (using class member variables)
	this->accuracy = (this->shotsFired > 0) ?
		static_cast<float>(this->shotsHit) / this->shotsFired * 100.f : 0.f;

	// Update accuracy bar width
	float percent = this->accuracy / 100.f;
	float maxBarWidth = 200.f; // max width of the bar
	this->accuracyBar.setSize(sf::Vector2f(percent * maxBarWidth, this->accuracyBar.getSize().y));

	// Change color based on accuracy
	if (this->accuracy >= 75.f) {
		this->accuracyBar.setFillColor(sf::Color::Green);
		this->uiText.setFillColor(sf::Color::Green);
	}
	else if (this->accuracy >= 50.f) {
		this->accuracyBar.setFillColor(sf::Color::Yellow);
		this->uiText.setFillColor(sf::Color::Yellow);
	}
	else {
		this->accuracyBar.setFillColor(sf::Color::Red);
		this->uiText.setFillColor(sf::Color::Red);
	}

	// Update the text display (moved from updateText() for better organization)
	std::stringstream ss;
	ss << "Points: " << this->points << "\n"
		<< "Health: " << this->health << "\n"
		<< std::fixed << std::setprecision(1) << "Accuracy: " << this->accuracy << "%\n"
		<< "Best Accuracy: " << static_cast<int>(this->bestAccuracy) << "%\n"
		<< "High Score: " << this->highScore << "\n";

	this->uiText.setString(ss.str());
}

void Game::render()
{
	// Clear old frames
	this->window->clear(sf::Color(0, 0, 0, 255));

	// Render based on game state
	if (this->gameState == MENU)
	{
		this->renderMenu(*this->window);
	}
	else if (this->gameState == GAME)
	{
		// Always render game elements
		this->renderEnemies(*this->window);
		this->window->draw(this->accuracyBarBack);
		this->window->draw(this->accuracyBar);
		this->renderText(*this->window);

		// Draw pause button (always visible during gameplay - top right corner)
		this->window->draw(this->pauseButton);
		this->window->draw(this->pauseButtonText);



		// Draw pause menu if paused (ESC or pause button)
		if (this->isPaused)
		{
			// Draw semi-transparent overlay
			this->window->draw(this->pauseOverlay);

			// Draw "Paused" text (centered near top of screen)
			sf::Text pausedText;
			pausedText.setFont(this->font);
			pausedText.setString("Paused");
			pausedText.setCharacterSize(50);
			pausedText.setFillColor(sf::Color::White);

			sf::FloatRect textBounds = pausedText.getLocalBounds();
			pausedText.setOrigin(textBounds.width / 2.f, textBounds.height / 2.f);
			pausedText.setPosition(
				this->videoMode.width / 2.f,
				this->videoMode.height / 3.f  // Higher up on screen
			);
			this->window->draw(pausedText);

			// Draw exit button (centered below "Paused" text)
			this->window->draw(this->exitButton);
			this->window->draw(this->exitButtonText);

			// Draw resume button (centered below exit button) - optional if you want to keep it
			this->window->draw(this->resumeButton);
			this->window->draw(this->resumeButtonText);
		}
	}

	// Display the final frame
	this->window->display();
}