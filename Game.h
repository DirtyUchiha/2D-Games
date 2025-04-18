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

    // Mouse positions
    sf::Vector2i mousePosWindow;
    sf::Vector2f mousePosView;

    // Fonts and UI
    sf::Font font;
    sf::Text uiText;

    // Game Logic
    bool endGame;
    unsigned points;
    int health;
    float enemySpawnTimer;

    float enemySpawnTimerMax;
    int maxEnemies;
    bool mouseHeld;
    unsigned shotsFired;
    unsigned shotsHit;
    float accuracy;
    float bestAccuracy;
    unsigned highScore;

    // Enemies including health blocks
    sf::RectangleShape enemy; //template shape
    std::vector<sf::RectangleShape> enemies;
    std::vector<sf::RectangleShape> healthBlocks;

    // Accuracy bar
    sf::RectangleShape accuracyBarBack;
    sf::RectangleShape accuracyBar;

    // Game state
    GameState gameState;  // **No change, just used the global enum**
    bool isPaused;

    // Pause overlay
    sf::RectangleShape pauseOverlay;

    // Pause button
    sf::RectangleShape pauseButton;
    sf::Text pauseButtonText;

    // Menu UI
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