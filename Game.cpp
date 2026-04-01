#include "Game.h"
#include <chrono>
#include <thread>
#include <unistd.h> 
#include <cstdlib>


static const int KEY_UP    = 1000;
static const int KEY_DOWN  = 1001;
static const int KEY_RIGHT = 1002;
static const int KEY_LEFT  = 1003;

void Game::run(){
    using clock = std::chrono::steady_clock;
    using ms    = std::chrono::milliseconds;
    auto next = clock::now();

    while(true){
        input();
        update();
        render();

        next += ms(50);
        std::this_thread::sleep_until(next);
    }
}

static int readKey() {
    unsigned char c = 0;
    if (read(STDIN_FILENO, &c, 1) != 1) return -1;  // -1 = no key

    // Handle arrow key escape sequences: ESC [ A/B/C/D
    if (c == '\033') {
        unsigned char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\033';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\033';
        if (seq[0] == '[') {
            switch (seq[1]) {
                case 'A': return KEY_UP;
                case 'B': return KEY_DOWN;
                case 'C': return KEY_RIGHT;
                case 'D': return KEY_LEFT;
            }
        }
        return '\033';
    }
    return (int)c;
}

void Game::input(){
    switch(state){
        case GameState::MENU:     inputMenu();    break;
        case GameState::PLAYING:  inputPlaying(); break;
        case GameState::PAUSED:   inputPaused();  break;
        case GameState::GAME_OVER:inputGameOver();break;
        case GameState::WIN:      inputWin();     break;
    }
}

void Game::inputMenu(){
    int ch = readKey();
    switch(ch){
        case 'w':
        case KEY_UP:
            menuSelection = (menuSelection - 1 + 2) % 2;
            break;
        case 's':
        case KEY_DOWN:
            menuSelection = (menuSelection + 1) % 2;
            break;
        case '\n':
        case ' ':
            if(menuSelection == 0) state = GameState::PLAYING;
            else exit(0);
            break;
    }
}

void Game::inputPlaying(){
    int ch = readKey();
    switch(ch){
        case 'q': exit(0);
        case 'p': state = GameState::PAUSED; break;

        case 'w': player.move(0, -1, map); break;
        case 's': player.move(0, 1, map); break;
        case 'a': player.move(-1, 0, map); break;
        case 'd': player.move(1, 0, map); break;

        case ' ':
            if(!isAttacking && attackCD <= 0){
                int ax = player.getLastX() + player.getX();
                int ay = player.getLastY() + player.getY();

                if(map.isWalkable(ax, ay)){
                    isAttacking = true;
                    attackTimer = 5;
                    attackCD = 10;
                    attackX = ax;
                    attackY = ay;
                }
            }
            break;
    }

}

void Game::inputPaused(){
    int ch = readKey();
    switch(ch){
        case 'p': state = GameState::PLAYING; break;
        case 'q': exit(0);
    }
}

void Game::inputGameOver(){
    int ch = readKey();
    if(ch == ' ' || ch == '\n'){
        player = Player();
        isAttacking   = false;
        attackTimer   = 0;
        attackCD = 0;
        menuSelection  = 0;
        state = GameState::MENU;
    }
}

void Game::inputWin(){
    int ch = readKey();
    if(ch == '\n' || ch == ' '){
        menuSelection = 0;
        state = GameState::MENU;
    }
}

// ===============
//     UPDATE
// ===============

void Game::update(){
    switch(state){
        case GameState::MENU:     updateMenu();     break;
        case GameState::PLAYING:  updatePlaying();  break;
        case GameState::PAUSED:   updatePaused();   break;
        case GameState::GAME_OVER:updateGameOver(); break;
        case GameState::WIN:      updateWin();      break;
    }
}

void Game::updateMenu()    { /* nothing to tick */ }
void Game::updatePaused()  { /* freeze evrytng  */ }
void Game::updateGameOver(){ /* nothing to tick */ }
void Game::updateWin()     { /* nothing to tick */ }

void Game::updatePlaying(){
    if(isAttacking){
        attackTimer--;
        if(attackTimer <= 0){
            isAttacking = false;
        }
    }

    if(attackCD > 0){
        attackCD--;
    }
}

// ===============
//     RENDER
// ===============

void Game::render(){
    switch(state){
        case GameState::MENU:     renderMenu();     break;
        case GameState::PLAYING:  renderPlaying();  break;
        case GameState::PAUSED:   renderPaused();   break;
        case GameState::GAME_OVER:renderGameOver(); break;
        case GameState::WIN:      renderWin();      break;
    }  
}

void Game::renderMenu(){
    renderer.drawMenu(menuSelection);
}

void Game::renderPlaying(){
    renderer.drawGame(map, player, isAttacking, attackX, attackY);
}

void Game::renderPaused(){
    renderer.drawPauseOverlay();
}

void Game::renderGameOver(){
    renderer.drawGameOver();
}

void Game::renderWin(){
    renderer.drawWin();
}

Game::Game(){}

