#include "Game.h"

// ============== Initialization Functions ==============

Game::Game()
{
	this->initVariables();
	this->initWindow();
	this->initFonts();
	this->initText();
	this->initEnemies();
	this->initPauseMenu();
	this->initGameUI();
	this->initMenuUI();
	this->initShop();
	this->gameState = MENU;
}

Game::~Game()
{
	delete this->window;
}

//Private functions
void Game::initVariables()
{
	this->window = new sf::RenderWindow(sf::VideoMode(920, 1080), "Game Window");
	
	// Game state
	this->gameState = MENU;
	this->endGame = false;
	this->isPaused = false;
	this->shopOpen = false;
	this->mouseHeld = false;

	// Game stats
	this->points = 0;
	this->health = 20;
	this->shotsFired = 0;
	this->shotsHit = 0;
	this->accuracy = 0.f;
	this->bestAccuracy = 0.f;
	this->highScore = 0;

	// Enemy system
	this->enemySpawnTimerMax = 20.f;
	this->enemySpawnTimer = this->enemySpawnTimerMax;
	this->maxEnemies = 5;

	// Powerup system
	this->currency = 0;
	this->hasFreezeAbility = false;
	this->freezeActive = false;
	this->freezeTimer = 0.f;
	this->freezeDuration = 10.f;
	this->normalEnemySpeed = 5.f;
	this->frozenEnemySpeed = 2.5f;

	// Initialize UI elements
	initPauseMenu();
	initGameUI();
	initMenuUI();
	initShop();

	// Load saved data
	loadGameData();
}

void Game::initWindow()
{
	this->videoMode.height = 1080;
	this->videoMode.width = 920;
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

	// Determine current enemy speed based on freeze status
	float currentEnemySpeed = this->freezeActive ? this->frozenEnemySpeed : this->normalEnemySpeed;
	float currentHealthBlockSpeed = this->freezeActive ? 2.f : 4.f; // Also slow health blocks

	// Health blocks
	for (size_t i = 0; i < this->healthBlocks.size(); i++)
	{
		this->healthBlocks[i].move(0.f, currentHealthBlockSpeed); //Update position with current speed
		if (this->healthBlocks[i].getPosition().y > this->window->getSize().y)
		{
			this->healthBlocks.erase(this->healthBlocks.begin() + i);
			--i;
		}
	}

	// Move Enemies
	for (int i = 0; i < this->enemies.size(); i++)
	{
		this->enemies[i].move(0.f, currentEnemySpeed); // Use current speed
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
		bool clickedOnSomething = false;

		// Check health blocks first
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
				clickedOnSomething = true;
				break; // skip checking other objects if you hit a health block
			}
		}

		// If we didn't click on a health block, check enemies and count as a shot
		if (!clickedOnSomething)
		{
			this->shotsFired++; // Only increment shots fired if we're aiming at enemies

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
					int pointsEarned = 0;
					if (this->enemies[i].getFillColor() == sf::Color::Magenta)
						pointsEarned = 10;
					else if (this->enemies[i].getFillColor() == sf::Color::Blue)
						pointsEarned = 7;
					else if (this->enemies[i].getFillColor() == sf::Color::Cyan)
						pointsEarned = 5;
					else if (this->enemies[i].getFillColor() == sf::Color::Red)
						pointsEarned = 3;
					else if (this->enemies[i].getFillColor() == sf::Color::Green)
						pointsEarned = 1;

					this->points += pointsEarned;
					this->currency += pointsEarned; // Add to currency too

					std::cout << "Points: " << this->points << " Currency: " << this->currency << "\n";

					// Delete the enemy
					this->enemies.erase(this->enemies.begin() + i);
					break; // Exit the loop once the enemy is hit
				}
			}
		}
	}
}

void Game::updateShop()
{
	// Add shop update logic here if needed
	// Currently empty as no continuous shop updates are required
}

void Game::updateUI()
{
    if (this->isPaused)
        return;

    // Calculate accuracy
    this->accuracy = (this->shotsFired > 0) ?
        static_cast<float>(this->shotsHit) / this->shotsFired * 100.f : 0.f;

    // Update accuracy bar
    float percent = this->accuracy / 100.f;
    float maxBarWidth = 200.f;
    this->accuracyBar.setSize(sf::Vector2f(percent * maxBarWidth, this->accuracyBar.getSize().y));

    // Update freeze button if we have the ability
    if (this->hasFreezeAbility) {
        if (this->freezeActive) {
            this->freezeAbilityButton.setFillColor(sf::Color(50, 50, 150));
            
            int secondsLeft = static_cast<int>(std::ceil(this->freezeTimer));
            this->freezeAbilityButtonText.setString(std::to_string(secondsLeft) + "s");
        }
        else {
            this->freezeAbilityButton.setFillColor(sf::Color(100, 100, 255));
            this->freezeAbilityButtonText.setString("FREEZE (" + std::to_string(this->freezeCharges) + ")");
        }
        
        sf::FloatRect textRect = this->freezeAbilityButtonText.getLocalBounds();
        this->freezeAbilityButtonText.setOrigin(
            textRect.left + textRect.width/2.0f,
            textRect.top + textRect.height/2.0f
        );
        this->freezeAbilityButtonText.setPosition(
            this->freezeAbilityButton.getPosition().x + this->freezeAbilityButton.getSize().x/2.0f,
            this->freezeAbilityButton.getPosition().y + this->freezeAbilityButton.getSize().y/2.0f
        );
    }

    // Update the text display
    std::stringstream ss;
    ss << "Points: " << this->points << "\n"
       << "Currency: " << this->currency << "\n"
       << "Health: " << this->health << "\n"
       << std::fixed << std::setprecision(1) << "Accuracy: " << this->accuracy << "%\n"
       << "Best Accuracy: " << static_cast<int>(this->bestAccuracy) << "%\n"
       << "High Score: " << this->highScore << "\n";

    this->uiText.setString(ss.str());
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

void Game::centerText(sf::Text& text, const sf::RectangleShape& shape, float xOffset, float yOffset)
{
	sf::FloatRect textBounds = text.getLocalBounds();
	text.setOrigin(textBounds.width / 2, textBounds.height / 2);
	text.setPosition(
		shape.getPosition().x + shape.getSize().x / 2.f + xOffset,
		shape.getPosition().y + shape.getSize().y / 2.f + yOffset
	);
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
		debugRect.setOutlineColor(sf::Color::Yellow);
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
		// Render title
		sf::Text gameTitle;
		gameTitle.setFont(this->font);
		gameTitle.setString("AIM TRAINER");
		gameTitle.setCharacterSize(60);
		gameTitle.setFillColor(sf::Color::White);

		// Center the title
		sf::FloatRect titleBounds = gameTitle.getLocalBounds();
		gameTitle.setOrigin(titleBounds.width / 2, titleBounds.height / 2);
		gameTitle.setPosition(
			this->videoMode.width / 2.f,
			this->videoMode.height / 4.f
		);
		target.draw(gameTitle);

		// Render currency display on main menu
		sf::Text currencyText;
		currencyText.setFont(this->font);
		currencyText.setString("Currency: " + std::to_string(this->currency));
		currencyText.setCharacterSize(24);
		currencyText.setFillColor(sf::Color::Yellow);
		currencyText.setPosition(20.f, 20.f);
		target.draw(currencyText);

		// Render start button
		target.draw(this->startButton);
		target.draw(this->startButtonText);

		// Render shop button
		target.draw(this->shopButton);
		target.draw(this->shopButtonText);

		//Render exit button
		target.draw(this->exitButton);      
		target.draw(this->exitButtonText);  

		// If shop is open, render shop panel
		if (this->shopOpen) {
			this->renderShop(target);
		}
	}
}

void Game::initShop()
{
	// Shop panel (background) - centered in window
	this->shopPanel.setSize(sf::Vector2f(500.f, 400.f));
	this->shopPanel.setFillColor(sf::Color(30, 30, 60, 230));
	this->shopPanel.setOutlineColor(sf::Color::White);
	this->shopPanel.setOutlineThickness(2.f);
	this->shopPanel.setPosition(
		(this->window->getSize().x - this->shopPanel.getSize().x) / 2.f,  // Center X
		(this->window->getSize().y - this->shopPanel.getSize().y) / 2.f   // Center Y
	);

	// Freeze power-up button
	this->freezeButton.setSize(sf::Vector2f(400.f, 60.f));
	this->freezeButton.setFillColor(
		this->hasFreezeAbility ? sf::Color(50, 50, 120) : sf::Color(100, 100, 255)
	);
	this->freezeButton.setPosition(
		this->shopPanel.getPosition().x + (this->shopPanel.getSize().x - this->freezeButton.getSize().x) / 2.f,
		this->shopPanel.getPosition().y + 100.f
	);

	// Freeze button text
	this->freezeButtonText.setFont(this->font);
	this->freezeButtonText.setString(
		this->hasFreezeAbility ? "PURCHASED" : "FREEZE POWER-UP"
	);
	this->freezeButtonText.setCharacterSize(28);
	this->freezeButtonText.setFillColor(sf::Color::White);
	centerText(this->freezeButtonText, this->freezeButton, 0.f, 0.f);

	// Freeze price text
	this->freezePrice.setFont(this->font);
	this->freezePrice.setString(
		this->hasFreezeAbility ? "OWNED" : "500"
	);
	this->freezePrice.setCharacterSize(24);
	this->freezePrice.setFillColor(
		this->hasFreezeAbility ? sf::Color::Green : sf::Color::Yellow
	);
	centerText(this->freezePrice, this->freezeButton, 0.f, 30.f);
}

void Game::renderShop(sf::RenderTarget& target)
{
	// Draw shop background panel (drawn first)
	target.draw(this->shopPanel);

	// Set button appearance based on purchase state
	if (showingPurchaseFeedback)
	{
		// Temporary "success" appearance (green)
		this->freezeButton.setFillColor(sf::Color(0, 200, 0));
		this->freezeButtonText.setString("PURCHASED!");
		this->freezePrice.setString("+1 CHARGE");
	}
	else if (this->currency < FREEZE_COST)
	{
		// Can't afford appearance (grayed out)
		this->freezeButton.setFillColor(sf::Color(100, 100, 100));
		this->freezeButtonText.setString(freezeCharges > 0 ?
			"MORE CHARGES (" + std::to_string(freezeCharges) + ")" :
			"BUY FREEZE");
		this->freezePrice.setString(std::to_string(FREEZE_COST));
	}
	else
	{
		// Normal purchasable appearance (blue)
		this->freezeButton.setFillColor(sf::Color(100, 100, 255));
		this->freezeButtonText.setString(freezeCharges > 0 ?
			"BUY MORE (" + std::to_string(freezeCharges) + ")" :
			"BUY FREEZE");
		this->freezePrice.setString(std::to_string(FREEZE_COST));
	}

	// Center all text elements
	centerText(this->freezeButtonText, this->freezeButton, 0.f, 0.f);
	centerText(this->freezePrice, this->freezeButton, 0.f, 30.f);

	// Draw interactive elements
	target.draw(this->freezeButton);
	target.draw(this->freezeButtonText);
	target.draw(this->freezePrice);

	// Draw shop title
	sf::Text shopTitle;
	shopTitle.setFont(this->font);
	shopTitle.setString("POWER-UP SHOP");
	shopTitle.setCharacterSize(32);
	shopTitle.setFillColor(sf::Color::White);

	sf::FloatRect titleBounds = shopTitle.getLocalBounds();
	shopTitle.setOrigin(titleBounds.width / 2, titleBounds.height / 2);
	shopTitle.setPosition(
		this->shopPanel.getPosition().x + this->shopPanel.getSize().x / 2.f,
		this->shopPanel.getPosition().y + 30.f
	);
	target.draw(shopTitle);

	// Draw currency display
	sf::Text currencyText;
	currencyText.setFont(this->font);
	currencyText.setString("Currency: " + std::to_string(this->currency));
	currencyText.setCharacterSize(24);
	currencyText.setFillColor(sf::Color::Yellow);
	currencyText.setPosition(
		this->shopPanel.getPosition().x + (this->shopPanel.getSize().x - currencyText.getLocalBounds().width) / 2.f,
		this->shopPanel.getPosition().y + 70.f
	);
	target.draw(currencyText);

	// Draw close instruction
	sf::Text closeText;
	closeText.setFont(this->font);
	closeText.setString("Press ESC to close shop");
	closeText.setCharacterSize(18);
	closeText.setFillColor(sf::Color(200, 200, 200));

	sf::FloatRect closeBounds = closeText.getLocalBounds();
	closeText.setOrigin(closeBounds.width / 2, closeBounds.height / 2);
	closeText.setPosition(
		this->shopPanel.getPosition().x + this->shopPanel.getSize().x / 2.f,
		this->shopPanel.getPosition().y + this->shopPanel.getSize().y - 30.f
	);
	target.draw(closeText);

	// Ensure text visibility (called last to override any previous settings)
	this->freezeButtonText.setFillColor(sf::Color::White);
	this->freezePrice.setFillColor(showingPurchaseFeedback ? sf::Color::White : sf::Color::Yellow);
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
			if (this->ev.key.code == sf::Keyboard::Escape)
			{
				if (this->gameState == GAME)
				{
					this->isPaused = !this->isPaused;
				}
				else if (this->gameState == MENU && this->shopOpen)
				{
					this->shopOpen = false;
				}
			}
			break;

		case sf::Event::MouseButtonPressed:
			if (this->ev.mouseButton.button == sf::Mouse::Left)
			{
				this->mouseHeld = true;

				if (this->gameState == MENU)
				{
					if (!this->shopOpen)
					{
						if (this->startButton.getGlobalBounds().contains(this->mousePosView))
						{
							// Start game logic...
							this->gameState = GAME;
							this->isPaused = false;
							this->endGame = false;
							this->health = 20;
							this->points = 0;
							this->shotsFired = 0;
							this->shotsHit = 0;
							this->accuracy = 0.f;
							this->freezeActive = false;
							this->freezeTimer = 0.f;
							this->enemies.clear();
							this->healthBlocks.clear();
						}
						else if (this->shopButton.getGlobalBounds().contains(this->mousePosView))
						{
							this->shopOpen = true;
						}
						else if (this->exitButton.getGlobalBounds().contains(this->mousePosView))
						{
							this->window->close();
						}
					}
					else if (this->shopOpen)
					{
						if (this->freezeButton.getGlobalBounds().contains(this->mousePosView) &&
							this->currency >= FREEZE_COST)
						{
							this->currency -= FREEZE_COST;
							this->freezeCharges++;
							this->hasFreezeAbility = true;  // Make sure this is set
							this->initFreezeButton();      // Initialize the button
							this->showingPurchaseFeedback = true;
							this->purchaseFeedbackClock.restart();
							saveGameState();
						}
					}
				}
				else if (this->gameState == GAME)
				{
					if (this->isPaused)
					{
						if (this->resumeButton.getGlobalBounds().contains(this->mousePosView))
						{
							this->isPaused = false;
						}
						else if (this->exitButton.getGlobalBounds().contains(this->mousePosView))
						{
							saveGameState();
							this->window->close();
						}
					}
					else
					{
						// First check UI buttons
						if (this->pauseButton.getGlobalBounds().contains(this->mousePosView))
						{
							this->isPaused = true;
						}
						else if (this->freezeCharges > 0 &&
							!this->freezeActive &&
							this->freezeAbilityButton.getGlobalBounds().contains(this->mousePosView))
						{
							this->freezeActive = true;
							this->freezeCharges--;
							this->freezeTimer = this->freezeDuration;
						}
						else
						{
							// If no UI button was clicked, handle game object clicks
							this->handleGameObjectClicks();
						}
					}
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
void Game::saveGameState()
{
	std::ofstream outFile("savegame.txt");
	if (outFile.is_open())
	{
		outFile << this->highScore << " "
			<< this->bestAccuracy << " "
			<< this->currency << " "
			<< (this->hasFreezeAbility ? 1 : 0) << " "
			<< this->freezeCharges;
		outFile.close();
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

void Game::update()
{
	this->pollEvents();
	this->updateMousePositions(); // Always update for button interaction

	if (this->gameState == MENU)
	{
		// Nothing special to update for menu
	}
	else if (this->gameState == GAME && !this->isPaused && !this->endGame)
	{
		this->updateText();
		this->updateEnemies();
		this->updatePowerups(); // Update powerup timers

		if (this->health <= 0)
		{
			this->endGame = true;

			if (this->accuracy > this->bestAccuracy)
				this->bestAccuracy = this->accuracy;

			if (this->points > this->highScore)
				this->highScore = this->points;


			// Save all data including currency and powerups
			std::ofstream outFile("savegame.txt");
			if (outFile.is_open())
			{
				outFile << this->highScore << " " << this->bestAccuracy << " "
					<< this->currency << " " << (this->hasFreezeAbility ? 1 : 0);
				outFile.close();
			}
		}
	}
	// Handle purchase feedback
	if (showingPurchaseFeedback &&
		purchaseFeedbackClock.getElapsedTime().asSeconds() > FEEDBACK_DURATION)
	{
		showingPurchaseFeedback = false;
	}

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
		// Update UI information first
		this->updateUI();

		// Always render game elements
		this->renderEnemies(*this->window);
		this->window->draw(this->accuracyBarBack);
		this->window->draw(this->accuracyBar);
		this->renderText(*this->window);

		// Draw pause button (always visible during gameplay - top right corner)
		this->window->draw(this->pauseButton);
		this->window->draw(this->pauseButtonText);

		// Draw freeze ability button if player has purchased it
		if (this->hasFreezeAbility && this->freezeCharges > 0) {
			this->window->draw(this->freezeAbilityButton);
			this->window->draw(this->freezeAbilityButtonText);
		}

		// Draw freeze effect indicator if active
		if (this->freezeActive) {
			// Draw a blue semi-transparent overlay on screen edges to indicate freeze
			sf::RectangleShape freezeIndicator;
			freezeIndicator.setSize(sf::Vector2f(this->videoMode.width, 10.f));
			freezeIndicator.setFillColor(sf::Color(100, 100, 255, 150));

			// Top edge
			freezeIndicator.setPosition(0.f, 0.f);
			this->window->draw(freezeIndicator);

			// Bottom edge
			freezeIndicator.setPosition(0.f, this->videoMode.height - 10.f);
			this->window->draw(freezeIndicator);

			// Left edge (vertical)
			freezeIndicator.setSize(sf::Vector2f(10.f, this->videoMode.height));
			freezeIndicator.setPosition(0.f, 0.f);
			this->window->draw(freezeIndicator);

			// Right edge (vertical)
			freezeIndicator.setPosition(this->videoMode.width - 10.f, 0.f);
			this->window->draw(freezeIndicator);
		}

		// Draw pause menu if paused
		if (this->isPaused)
		{
			// Draw semi-transparent overlay
			this->window->draw(this->pauseOverlay);

			// Draw "Paused" text
			sf::Text pausedText;
			pausedText.setFont(this->font);
			pausedText.setString("Paused");
			pausedText.setCharacterSize(50);
			pausedText.setFillColor(sf::Color::White);

			sf::FloatRect textBounds = pausedText.getLocalBounds();
			pausedText.setOrigin(textBounds.width / 2.f, textBounds.height / 2.f);
			pausedText.setPosition(
				this->videoMode.width / 2.f,
				this->videoMode.height / 3.f
			);
			this->window->draw(pausedText);

			// Draw exit button
			this->window->draw(this->exitButton);
			this->window->draw(this->exitButtonText);

			// Draw resume button
			this->window->draw(this->resumeButton);
			this->window->draw(this->resumeButtonText);
		}
	}

	// Display the final frame
	this->window->display();
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

void Game::loadGameData()
{
	std::ifstream inFile("savegame.txt");
	if (inFile.is_open())
	{
		int hasFreeze = 0;
		inFile >> this->highScore >> this->bestAccuracy
			>> this->currency >> hasFreeze >> this->freezeCharges;
		this->hasFreezeAbility = (hasFreeze == 1);
		inFile.close();

		if (this->hasFreezeAbility) {
			this->initFreezeButton();
		}
	}
	else
	{
		loadHighscore();
	}
}

void Game::loadHighscore()
{
	std::ifstream inFile("highscore.txt");
	if (inFile.is_open())
	{
		std::string line;
		if (std::getline(inFile, line)) {
			std::istringstream iss(line);
			if (!(iss >> this->highScore >> this->bestAccuracy)) {
				this->highScore = 0;
				this->bestAccuracy = 0.f;
			}
		}
		inFile.close();
	}
	else
	{
		// Create default file
		std::ofstream outFile("highscore.txt");
		if (outFile.is_open()) {
			outFile << "0 0";
			outFile.close();
		}
	}
}

void Game::initPauseMenu()
{
	// Pause overlay
	this->pauseOverlay.setSize(sf::Vector2f(this->videoMode.width, this->videoMode.height));
	this->pauseOverlay.setFillColor(sf::Color(0, 0, 0, 150));

	// Pause button
	this->pauseButton.setSize(sf::Vector2f(100.f, 50.f));
	this->pauseButton.setFillColor(sf::Color(100, 100, 255));
	this->pauseButton.setPosition(this->videoMode.width - 120.f, 20.f);

	this->pauseButtonText.setFont(this->font);
	this->pauseButtonText.setString("Pause");
	this->pauseButtonText.setCharacterSize(30);
	this->pauseButtonText.setFillColor(sf::Color::White);
	centerText(this->pauseButtonText, this->pauseButton);

	// Exit button
	this->exitButton.setSize(sf::Vector2f(200.f, 50.f));
	this->exitButton.setFillColor(sf::Color(255, 0, 0));
	this->exitButton.setPosition(
		(this->videoMode.width - 200.f) / 2.f,
		(this->videoMode.height - 50.f) / 2.f
	);

	this->exitButtonText.setFont(this->font);
	this->exitButtonText.setString("Exit");
	this->exitButtonText.setCharacterSize(32);
	this->exitButtonText.setFillColor(sf::Color::White);
	centerText(this->exitButtonText, this->exitButton);

	// Resume button
	this->resumeButton.setSize(sf::Vector2f(200.f, 50.f));
	this->resumeButton.setFillColor(sf::Color(0, 255, 0));
	this->resumeButton.setPosition(
		this->exitButton.getPosition().x,
		this->exitButton.getPosition().y - 70.f
	);

	this->resumeButtonText.setFont(this->font);
	this->resumeButtonText.setString("Resume");
	this->resumeButtonText.setCharacterSize(32);
	this->resumeButtonText.setFillColor(sf::Color::White);
	centerText(this->resumeButtonText, this->resumeButton);
}

void Game::initGameUI()
{
	// Accuracy bar
	this->accuracyBarBack.setSize(sf::Vector2f(200.f, 20.f));
	this->accuracyBarBack.setFillColor(sf::Color(50, 50, 50, 200));
	this->accuracyBarBack.setPosition(0.f, 205.f);

	// Accuracy bar foreground
	this->accuracyBar.setSize(sf::Vector2f(0.f, 20.f)); // Start empty
	this->accuracyBar.setFillColor(sf::Color::Green);
	this->accuracyBar.setPosition(0.f, 205.f);

	// Pause button (top right - unchanged)
	this->pauseButton.setSize(sf::Vector2f(100.f, 50.f));
	this->pauseButton.setFillColor(sf::Color(100, 100, 255));
	this->pauseButton.setPosition(this->videoMode.width - 120.f, 20.f);

	// Freeze ability button (below accuracy bar)
	this->freezeAbilityButton.setSize(sf::Vector2f(100.f, 50.f));
	this->freezeAbilityButton.setFillColor(sf::Color(100, 100, 255));
	this->freezeAbilityButton.setPosition(20.f, 320.f);  // 40px below accuracy bar (280+20+20)

	this->freezeAbilityButtonText.setFont(this->font);
	this->freezeAbilityButtonText.setString("FREEZE");
	this->freezeAbilityButtonText.setCharacterSize(20);
	this->freezeAbilityButtonText.setFillColor(sf::Color::White);
	centerText(this->freezeAbilityButtonText, this->freezeAbilityButton);

	// Game text
	this->uiText.setFont(this->font);
	this->uiText.setCharacterSize(24);
	this->uiText.setFillColor(sf::Color::White);
	this->uiText.setString("");
	this->uiText.setPosition(10.f, 10.f);

	// Enemy
	this->enemy.setSize(sf::Vector2f(100.f, 100.f));
	this->enemy.setFillColor(sf::Color::Cyan);
	this->enemy.setPosition(0.f, 0.f);

	// Add freeze ability button initialization
	/*
	this->freezeAbilityButton.setSize(sf::Vector2f(100.f, 50.f));
	this->freezeAbilityButton.setFillColor(sf::Color(100, 100, 255));
	this->freezeAbilityButton.setPosition(20.f, 220.f);

	this->freezeAbilityButtonText.setFont(this->font);
	this->freezeAbilityButtonText.setString("FREEZE");
	this->freezeAbilityButtonText.setCharacterSize(20);
	this->freezeAbilityButtonText.setFillColor(sf::Color::White);
	centerText(this->freezeAbilityButtonText, this->freezeAbilityButton);
	*/
}

void Game::initMenuUI()
{
	// Start button
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
	centerText(this->startButtonText, this->startButton, 0.f, -5.f);
	// Shop button (added below start button)
	this->shopButton.setSize(sf::Vector2f(200.f, 60.f));
	this->shopButton.setFillColor(sf::Color(255, 165, 0)); // Orange color
	this->shopButton.setPosition(
		this->videoMode.width / 2.f - this->shopButton.getSize().x / 2.f,
		this->startButton.getPosition().y + this->startButton.getSize().y + 20.f
	);

	this->shopButtonText.setFont(this->font);
	this->shopButtonText.setString("SHOP");
	this->shopButtonText.setCharacterSize(28);
	this->shopButtonText.setFillColor(sf::Color::White);
	centerText(this->shopButtonText, this->shopButton, 0.f, -5.f);
	initShop();

	// Exit Button (placed below shop button)
	this->exitButton.setSize(sf::Vector2f(200.f, 60.f));
	this->exitButton.setFillColor(sf::Color(255, 50, 50)); // Red color
	this->exitButton.setPosition(
		this->videoMode.width / 2.f - this->exitButton.getSize().x / 2.f,
		this->shopButton.getPosition().y + this->shopButton.getSize().y + 20.f
	);

	this->exitButtonText.setFont(this->font);
	this->exitButtonText.setString("EXIT");
	this->exitButtonText.setCharacterSize(28);
	this->exitButtonText.setFillColor(sf::Color::White);
	centerText(this->exitButtonText, this->exitButton, 0.f, -5.f);
}

void Game::updatePowerups()
{
	if (this->freezeActive)
	{
		this->freezeTimer -= 1.f / 60.f; // Decrease by ~1/60th of a second (assumes 60fps)
		if (this->freezeTimer <= 0.f)
		{
			this->freezeActive = false;
			this->freezeTimer = 0.f;

			// Reset freeze button text
			if (this->hasFreezeAbility) {
				this->freezeAbilityButtonText.setString("FREEZE");
				centerText(this->freezeAbilityButtonText, this->freezeAbilityButton);
			}
		}
	}
}

void Game::handleGameObjectClicks()
{
	bool clickedOnSomething = false;

	// Check health blocks first
	for (size_t i = 0; i < this->healthBlocks.size(); i++)
	{
		sf::FloatRect bounds = this->healthBlocks[i].getGlobalBounds();
		// Expand hitbox more generously
		bounds.left -= 10.f;
		bounds.top -= 10.f;
		bounds.width += 20.f;
		bounds.height += 20.f;

		if (bounds.contains(this->mousePosView))
		{
			this->health = std::min(this->health + 1, 20);
			this->healthBlocks.erase(this->healthBlocks.begin() + i);
			clickedOnSomething = true;
			break;
		}
	}

	// If we didn't click on a health block, check enemies
	if (!clickedOnSomething)
	{
		this->shotsFired++;
		for (size_t i = 0; i < this->enemies.size(); i++)
		{
			sf::FloatRect bounds = this->enemies[i].getGlobalBounds();
			// Expand hitbox more generously
			bounds.left -= 10.f;
			bounds.top -= 10.f;
			bounds.width += 20.f;
			bounds.height += 20.f;

			if (bounds.contains(this->mousePosView))
			{
				this->shotsHit++;

				// Gain points based on the enemy color
				int pointsEarned = 0;
				if (this->enemies[i].getFillColor() == sf::Color::Magenta)
					pointsEarned = 10;
				else if (this->enemies[i].getFillColor() == sf::Color::Blue)
					pointsEarned = 7;
				else if (this->enemies[i].getFillColor() == sf::Color::Cyan)
					pointsEarned = 5;
				else if (this->enemies[i].getFillColor() == sf::Color::Red)
					pointsEarned = 3;
				else if (this->enemies[i].getFillColor() == sf::Color::Green)
					pointsEarned = 1;

				this->points += pointsEarned;
				this->currency += pointsEarned;

				// Delete the enemy
				this->enemies.erase(this->enemies.begin() + i);
				break;
			}
		}
	}
}
void Game::initFreezeButton()
		{
			if (this->hasFreezeAbility) {
				this->freezeAbilityButton.setSize(sf::Vector2f(150.f, 50.f));  // Wider to fit text
				this->freezeAbilityButton.setFillColor(sf::Color(100, 100, 255));
				this->freezeAbilityButton.setPosition(
					this->window->getSize().x - 170.f,  // Right side
					20.f                                // Top
				);

				this->freezeAbilityButtonText.setFont(this->font);
				this->freezeAbilityButtonText.setString("FREEZE (" + std::to_string(this->freezeCharges) + ")");
				this->freezeAbilityButtonText.setCharacterSize(24);
				this->freezeAbilityButtonText.setFillColor(sf::Color::White);

				// Center text in button
				sf::FloatRect textRect = this->freezeAbilityButtonText.getLocalBounds();
				this->freezeAbilityButtonText.setOrigin(
					textRect.left + textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f
				);
				this->freezeAbilityButtonText.setPosition(
					this->freezeAbilityButton.getPosition().x + this->freezeAbilityButton.getSize().x / 2.0f,
					this->freezeAbilityButton.getPosition().y + this->freezeAbilityButton.getSize().y / 2.0f
				);
			}
		}

