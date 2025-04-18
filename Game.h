#pragma once

#include <iostream>
#include <vector>
#include <ctime>
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

// Enum for Game State
enum GameState { MENU, GAME, PAUSED };  // **Removed duplicate enum inside class**

class Game
{
private:
    // Window
    sf::RenderWindow* window;
    sf::VideoMode videoMode;
    sf::Event ev;

    //Game State
    GameState gameState;

    // Game Logic
    bool endGame;
    bool isPaused;
    bool mouseHeld;

    // Mouse positions
    sf::Vector2i mousePosWindow;
    sf::Vector2f mousePosView;

    // UI Elements and fonts
    sf::RectangleShape pauseButton;
    sf::Text pauseButtonText;
    sf::RectangleShape exitButton;
    sf::Text exitButtonText;
    sf::RectangleShape pauseOverlay;
    sf::Font font;
    sf::Text uiText;
    sf::RectangleShape resumeButton;
    sf::Text resumeButtonText;

	//Game variables
    int points;
    int health;
    float enemySpawnTimer;
    float enemySpawnTimerMax;
    int maxEnemies;
    unsigned shotsFired;
    unsigned shotsHit;
    float accuracy;
    float bestAccuracy;
    unsigned highScore;

    // Accuracy bar
    sf::RectangleShape accuracyBarBack;
    sf::RectangleShape accuracyBar;

    //Game objects
    sf::RectangleShape enemy;
	std::vector<sf::RectangleShape> enemies;
	std::vector<sf::RectangleShape> healthBlocks;

    // Menu UI/Start button setup
    sf::RectangleShape startButton;
    sf::Text startButtonText;

	// Private functions/Init functions
    void initVariables();
    void initWindow();
    void initFonts();
    void initText();
    void initEnemies();
 

public:
    // Constructors / Destructors
    Game();
    virtual ~Game();

    // Accessors
    bool getWindowIsOpen() const;
    bool getEndGame() const;

    // Functions
    void pollEvents();
    void update();
    void render();
    void updateUI();

    //Game specific logic
    void spawnEnemy();
    void updateEnemies();
    void updateText();
    void updateMousePositions();

    //Render helpers
    void renderEnemies(sf::RenderTarget& target);
    void renderText(sf::RenderTarget& target);

    // Menu render helper
    void renderMenu(sf::RenderTarget& target);
};