#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <random>
#include <windows.h> // For GetUserName on Windows
#include <shlobj.h> // For SHGetFolderPath
#include <sstream>


// Enum for Game State
enum GameState { MENU, GAME, PAUSED };

class Game
{
private:
    // Window
    sf::RenderWindow* window;
    sf::VideoMode videoMode;
    sf::Event ev;

    // Game State
    GameState gameState;
    bool endGame;
    bool isPaused;
    bool shopOpen;
    bool mouseHeld;
    bool fontLoaded;

    // Mouse positions
    sf::Vector2i mousePosWindow;
    sf::Vector2f mousePosView;

    // Game logic variables
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
    const int MAX_RECENT_POSITIONS = 5;

    // Currency and powerups
    int currency;
    bool hasFreezeAbility;
    bool freezeActive;
    float freezeTimer;
    float freezeDuration;
    float normalEnemySpeed;
    float frozenEnemySpeed;
    int freezeCharges;
    const int FREEZE_COST = 500;
    bool showingPurchaseFeedback;
    const float FEEDBACK_DURATION = .5f;
    bool freezeAbilityInitialized;

    // Confirmation dialog
    bool showingConfirmation;
    sf::RectangleShape confirmationPanel;
    sf::Text confirmationText;
    sf::RectangleShape confirmYesButton;
    sf::Text confirmYesText;
    sf::RectangleShape confirmNoButton;
    sf::Text confirmNoText;

    // Game objects
    sf::RectangleShape enemy;
    std::vector<sf::RectangleShape> enemies;
    std::vector<sf::RectangleShape> healthBlocks;
    std::vector<sf::Vector2f> recentHealthBlockPositions;

    // UI Elements
    sf::Font font;
    sf::Text uiText;

    // Accuracy bar
    sf::RectangleShape accuracyBarBack;
    sf::RectangleShape accuracyBar;

    // Pause UI
    sf::RectangleShape pauseOverlay;
    sf::RectangleShape pauseButton;
    sf::Text pauseButtonText;
    sf::RectangleShape resumeButton;
    sf::Text resumeButtonText;
    sf::RectangleShape mainMenuButton;
    sf::Text mainMenuButtonText;
    sf::RectangleShape exitButton;
    sf::Text exitButtonText;

    // Menu UI
    sf::RectangleShape startButton;
    sf::Text startButtonText;
    sf::RectangleShape shopButton;
    sf::Text shopButtonText;

    // Shop UI
    sf::RectangleShape shopPanel;
    sf::RectangleShape freezeButton;
    sf::Text freezeButtonText;
    sf::Text freezePrice;
    sf::Clock purchaseFeedbackClock;

    // Freeze ability UI
    sf::RectangleShape freezeAbilityButton;
    sf::Text freezeAbilityButtonText;

    // Private initialization functions
    void initVariables();
    void initWindow();
    void initFonts();
    void initText();
    void initEnemies();
    void initShop();
    void initPauseMenu();
    void initGameUI();
    void initMenuUI();
    void initConfirmationDialog();
    void initFreezeButton();
    void loadGameData();
    void loadHighscore();
    void saveGameState();
    void centerText(sf::Text& text, const sf::RectangleShape& shape, float xOffset = 0.f, float yOffset = 0.f);

    // Update functions
    void updateMousePositions();
    void updateText();
    void updateEnemies();
    void updatePowerups();
    void updateShop();
    void updateUI();
    void handleGameObjectClicks();

    // Render functions
    void renderText(sf::RenderTarget& target);
    void renderEnemies(sf::RenderTarget& target);
    void renderMenu(sf::RenderTarget& target);
    void renderShop(sf::RenderTarget& target);
    void renderConfirmationDialog(sf::RenderTarget& target);

public:
    // Constructors / Destructors
    Game();
    virtual ~Game();

    // Accessors
    bool getWindowIsOpen() const;
    bool getEndGame() const;

    // Core game loop functions
    void pollEvents();
    void update();
    void render();

    // Game logic
    void spawnEnemy();
};