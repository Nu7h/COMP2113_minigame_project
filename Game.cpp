#include "Game.h"
#include "Render.h"
#include <ncurses.h>

void Game::run(){
    while(true){
        input();
        update();
        render();
        napms(50);
    }
}

/*
switch(state){
    case GameState::MENU:
    case GameState::PLAYING:
    case GameState::PAUSED:
    case GameState::GAME_OVER:
    case GameState::WIN:
}
*/

// ===============
//      INPUT
// ===============

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
    int ch = getch();
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
    int ch = getch();
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
    int ch = getch();
    switch(ch){
        case 'p': state = GameState::PLAYING; break;
        case 'q': exit(0);
    }
}

void Game::inputGameOver(){
    int ch = getch();
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
    int ch = getch();
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
    renderer.drawGame(map, player, slime, isAttacking, attackX, attackY);
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

