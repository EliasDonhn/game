#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <time.h>
#include "tile.cpp"
#include "stopwatch.cpp"
#include "button.cpp"
#include "userInterface.cpp"
#include "SDL_utils.cpp"

using namespace std;

const unsigned int SCREEN_WIDTH = 410;
const unsigned int SCREEN_HEIGHT = 600;
const string WINDOW_TITLE = "SAVE ME!";

typedef vector<vector<Tile>> tileArray;

static inline bool inMap(const int row, const int col, const int maxRow, const int maxCol) {
    return (row >= 0 && row <= maxRow && col >= 0 && col <= maxCol);
}

static bool isEmptyTileInNeighbors(tileArray& tiles, const int row, const int col, Tile* emptyTile) {
    //Ham kiem tra xem trong nhung o lan can thi EmptyTile co nam trong do khong

    static const int deltas[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

    for (int i = 0; i < 4; i++) {
        if (inMap(row + deltas[i][0], col + deltas[i][1], tiles.size() - 1, tiles[0].size() - 1)) {
            if (emptyTile == &tiles[row + deltas[i][0]][col + deltas[i][1]]) {
                return true;
            }
        }
    }
    return false;
}

unsigned int playMenu(SDL_Renderer* renderer, bool* exit, const unsigned int SCREEN_WIDTH, 
                            const unsigned int SCREEN_HEIGHT) {
    // Ham de nguoi choi chon do kho cho game

    // Cac bien cho kich thuoc cac thanh phan giao dien nguoi dung
    const unsigned int NUMBER_OF_ROW_ELEMENTS = 1;              
    const unsigned int NUMBER_OF_COL_ELEMENTS = 4;                          
    const unsigned int NUMBER_OF_ROW_BORDERS = NUMBER_OF_ROW_ELEMENTS + 1; 
    const unsigned int NUMBER_OF_COL_BORDERS = NUMBER_OF_COL_ELEMENTS + 1;
    const unsigned int BORDER_THICKNESS = 20;

    // Khoi tao cac nut bam cua bang menu
    const unsigned int BUTTON_WIDTH = SCREEN_WIDTH - 2 * BORDER_THICKNESS;
    const unsigned int BUTTON_HEIGHT = (SCREEN_HEIGHT - NUMBER_OF_COL_BORDERS * BORDER_THICKNESS) / NUMBER_OF_COL_ELEMENTS; 

    // Mau sac cua menu
    const SDL_Color FONT_COLOUR = {255, 255, 255, 255}; // White
    const SDL_Color BUTTON_COLOUR = {0, 0, 0, 255}; // Black
    const SDL_Color BUTTON_DOWN_COLOUR = {50, 255, 100, 255}; // Green

    // Font cua chu cua text
    const int fontSize = BUTTON_HEIGHT - 40;
    TTF_Font* font = TTF_OpenFont("DS-DIGIB.ttf", fontSize);
    if (font == nullptr) {
        cout << "Failed to load font! Error: " << TTF_GetError() << endl;
    }
    
    // Van ban tren menu
    const char* buttonTexts[4] = {"3x3", "4x4", "5x5","6x6"};

    // Tao ra diem bat dau de bat dau thiet ke giao dien
    int startX = BORDER_THICKNESS;
    int startY = 0;

    // Su dung vector de tao ra cac button
    vector<Button> buttons;
    for (int row = 0; row < NUMBER_OF_COL_ELEMENTS; row++) {
        startY += BORDER_THICKNESS;
        SDL_Rect rect = {startX, startY, (int)BUTTON_WIDTH, (int)BUTTON_HEIGHT};
        Button button(rect, BUTTON_COLOUR, font, FONT_COLOUR);
        button.loadTexture(renderer, buttonTexts[row]);
        buttons.push_back(button);
        startY += BUTTON_HEIGHT;
    }

    const unsigned int FPS = 60;
    const float milliSecondsPerFrame = 1000 / FPS;
    //Bien bam gio
    float lastTimeRendered = SDL_GetTicks();
    //bien khoang thoi gian
    float deltaTimeRendered;


    // Cac bien cho vong lap game
    bool stop = false;
    SDL_Event event;
    unsigned int difficulty = 0;

    // Vong lap chay menu

    while (!stop) {

        while (SDL_PollEvent(&event) != 0) {
            // Trong truong hop quit
            if (event.type == SDL_QUIT) {
                *exit = true;
                stop = true;
            }
            // Khi con tro chuot duoc chon
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                for (int i = 0; i < NUMBER_OF_COL_ELEMENTS; i++) {
                    if (buttons[i].isMouseInside(x, y)) {
                        buttons[i].changeColourTo(BUTTON_DOWN_COLOUR);
                        // Bien the hien do kho difficulty se tang len 3 don vi so voi i khi do
                        // Vi du neu button 3*3 thi se co i = 0 nen difficulty = 3 -> map co do kho la 3*3
                        difficulty = i + 3;
                    }
                }
            }
            else if (event.type == SDL_MOUSEBUTTONUP){
                for (auto& button : buttons) {
                    button.changeColourTo(BUTTON_COLOUR);
                } 
                // Khi do kho da duoc chon thi bien stop chuyen thanh true, vong lap menu ket thuc
                if (difficulty != 0) {
                    stop = true;
                }
            }
        }
    

        // On dinh FPS 
        deltaTimeRendered = SDL_GetTicks() - lastTimeRendered;
        if (milliSecondsPerFrame > deltaTimeRendered) {
            SDL_Delay(milliSecondsPerFrame - deltaTimeRendered);
        }

        lastTimeRendered = SDL_GetTicks();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (auto& button : buttons) {
            button.render(renderer);
        }   

        SDL_RenderPresent(renderer);

    }

    for (auto& button : buttons)
        button.free();

    TTF_CloseFont(font);
    font = nullptr;

    return difficulty;
}

void playPuzzle(SDL_Renderer* renderer, bool* exit, const unsigned int DIFFICULTY, const unsigned int SCREEN_WIDTH, 
                        const unsigned int SCREEN_HEIGHT) {
    
    // Cac dac diem o giao dien nguoi dung
    const unsigned int NUMBER_OF_ROW_ELEMENTS = DIFFICULTY;
    const unsigned int NUMBER_OF_COL_ELEMENTS = DIFFICULTY + 2; // including stopwatch and "new" button
    const unsigned int NUMBER_OF_ROW_BORDERS = NUMBER_OF_ROW_ELEMENTS + 1;
    const unsigned int NUMBER_OF_COL_BORDERS = NUMBER_OF_COL_ELEMENTS + 1;
    const unsigned int BORDER_THICKNESS = 5;
    const unsigned int TILE_WIDTH = (SCREEN_WIDTH - NUMBER_OF_ROW_BORDERS * BORDER_THICKNESS) / NUMBER_OF_ROW_ELEMENTS; 
    const unsigned int TILE_HEIGHT = (SCREEN_HEIGHT - NUMBER_OF_COL_BORDERS * BORDER_THICKNESS) / NUMBER_OF_COL_ELEMENTS; 

    const unsigned int STOPWATCH_WIDTH = SCREEN_WIDTH - 2 * BORDER_THICKNESS;
    const unsigned int STOPWATCH_HEIGHT = TILE_HEIGHT;

    const unsigned int BUTTON_WIDTH = SCREEN_WIDTH - 2 * BORDER_THICKNESS;
    const unsigned int BUTTON_HEIGHT = TILE_HEIGHT;

    // Dinh nghia cac mau sac o giao dien
    const SDL_Color TILE_COLOUR = {0, 20, 50, 255};
    const SDL_Color TILE_COMPLETION_COLOUR = {50, 255, 100, 255}; // Green
    const SDL_Color FONT_COLOUR = {255, 40, 10, 255}; // Led red
    const SDL_Color FONT_COMPLETION_COLOUR = {255, 255, 255, 255}; // White
    const SDL_Color STOPWATCH_COLOUR = {160, 102, 198, 255}; // Purple
    const SDL_Color BUTTON_COLOUR = {0, 20, 50, 255};
    const SDL_Color BUTTON_DOWN_COLOUR = {50, 255, 100, 255}; // Green

    // Load font cho van ban
    const int fontSize = TILE_HEIGHT - 40;
    TTF_Font* font = TTF_OpenFont("DS-DIGIB.ttf", fontSize);
    if (font == nullptr) {
        cout << "Failed to load font! Error: " << TTF_GetError() << endl;
    }

    // Vi tri của dong ho bam gio
    int startX = BORDER_THICKNESS;
    int startY = BORDER_THICKNESS;
    SDL_Rect rect = {startX, startY, (int)STOPWATCH_WIDTH, (int)STOPWATCH_HEIGHT};
    Stopwatch stopwatch(rect, STOPWATCH_COLOUR, font, FONT_COLOUR);

    // Tao ra cac tiles
    tileArray tiles;
    startY += STOPWATCH_HEIGHT;
    for (int row = 0; row < DIFFICULTY; row++) {
        vector<Tile> tileRow;
        startY += BORDER_THICKNESS;
        startX = 0;
        for (int col = 0; col < DIFFICULTY; col++) {
            startX += BORDER_THICKNESS;
            rect = {startX, startY, (int)TILE_WIDTH, (int)TILE_HEIGHT};
            
            // Tao ra mot tile rong o vi tri goc phai ben duoi
            SDL_Color colour;
            if (!(row == DIFFICULTY - 1 && col == DIFFICULTY - 1))
                colour = TILE_COLOUR;
            
            // Tao ra so theo thu tu o tren ma tran
            int number = row * DIFFICULTY + col + 1;

            Tile tile(rect, colour, font, FONT_COLOUR, number);
            const char* numberStr = to_string(number).c_str();
            tile.loadTexture(renderer, numberStr);
            tileRow.push_back(tile);
            
            startX += TILE_WIDTH;
        }
        startY += TILE_HEIGHT;
        tiles.push_back(tileRow);
    }

    // Nut "Menu" o duoi cung
    startX = BORDER_THICKNESS;
    startY += BORDER_THICKNESS;
    rect = {startX, startY, (int)BUTTON_WIDTH, (int)BUTTON_HEIGHT};
    Button menuButton(rect, BUTTON_COLOUR, font, FONT_COLOUR);
    menuButton.loadTexture(renderer, "Menu");

    // Tao cac bien cho viec on dinh FPS
    const unsigned int FPS = 60;
    const float milliSecondsPerFrame = 1000 / FPS;
    float lastTimeRendered = SDL_GetTicks();
    float deltaTimeRendered;

    // Cac bien lien quan den animation
    const unsigned int pixelsPerSecond = 500;
    const float milliSecondsPerPixel = 1000 / pixelsPerSecond;
    float lastTimeMoved;
    float deltaTimeMoved;
    
    // Track the "moving" and "empty" tile
    Tile* movingTile = nullptr;
    bool doneMoving = true;
    Tile* emptyTile = &tiles[DIFFICULTY - 1][DIFFICULTY - 1];
    bool selected = false;
    int tempXPosition;
    int tempYPosition;

    // Tong so lan swap
    const unsigned int TOTAL_SWAPS = 1000;

    int emptyTileRow = DIFFICULTY - 1;
    int emptyTileCol = DIFFICULTY - 1;
    // Tao ra vector 2 chieu la toa do cua cac tile lan can
    vector<vector<int>> neighbors;
    
    srand(time(NULL));
    // Mang toa do khi di chuyen
    const int deltas[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
    // Dao cac tile
    for (int swap = 0; swap < TOTAL_SWAPS; ++swap) {
        for (int i = 0; i < 4; i++) {
            const int deltaRow = deltas[i][0];
            const int deltaCol = deltas[i][1];
            if (inMap(emptyTileRow + deltaRow, emptyTileCol + deltaCol, tiles.size() - 1, tiles[0].size() - 1)) {
                vector<int> neighbor = {emptyTileRow + deltaRow, emptyTileCol + deltaCol};
                neighbors.push_back(neighbor);
            }
        }
        // Chon mot tile ngau nhien trong cac tile trong vector neighbor
        const int randIndex = rand() % neighbors.size();
        const int row = neighbors[randIndex][0];
        const int col = neighbors[randIndex][1];
        // Dao lai cac thong so vi tri
        tempXPosition = emptyTile->getXPosition();
        tempYPosition = emptyTile->getYPosition();
        emptyTile->setPositionTo(tiles[row][col].getXPosition(), tiles[row][col].getYPosition());
        tiles[row][col].setPositionTo(tempXPosition, tempYPosition);
        
        // Swap cac thanh phan trong vector bang ham iter_swap
        iter_swap(&tiles[row][col], emptyTile);
        emptyTile = &tiles[row][col];
        emptyTileRow = row;
        emptyTileCol = col;
        neighbors.clear();
    }

    bool stop = false;
    SDL_Event event;
    bool checkSolved = false;
    bool solved = false;
    bool menuButtonPressed = false;

    stopwatch.start();

    for (int row = 0; row < tiles.size(); row++) {
        for (int col = 0; col < tiles[row].size(); col++) {
            const int number = row * DIFFICULTY + col + 1;
            // Tile nao dung vi tri thi chuyen sang mau chinh xac
            if (tiles[row][col].getNumber() == number) {
                tiles[row][col].changeColourTo(TILE_COMPLETION_COLOUR);
                tiles[row][col].changeFontColourTo(FONT_COMPLETION_COLOUR);
                tiles[row][col].loadTexture(renderer, to_string(tiles[row][col].getNumber()).c_str());
            }
        }
    }

    while (!stop) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                stop = true;
                *exit = true;
            }

            if (doneMoving) {
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    if (!solved) 
                        for (int row = 0; row < tiles.size(); row++) 
                            for (int col = 0; col < tiles[row].size(); col++) {
                                if (tiles[row][col].isMouseInside(x, y))
                                    if (isEmptyTileInNeighbors(tiles, row, col, emptyTile)) {
                                        movingTile = &tiles[row][col];
                                        selected = true;
                                        doneMoving = false;
                                        lastTimeMoved = SDL_GetTicks();
                                    }
                            }
                    if (menuButton.isMouseInside(x, y)) {
                        menuButton.changeColourTo(BUTTON_DOWN_COLOUR);
                        menuButtonPressed = true;
                    }
                }
                else if (event.type == SDL_MOUSEBUTTONUP)
                    if (menuButtonPressed)
                        stop = true;
            }
        }

        // Dat lai vi tri cho tile rong
        if (selected && movingTile != nullptr) {
            tempXPosition = movingTile->getXPosition();
            tempYPosition = movingTile->getYPosition();
            selected = false;
        }

        // Lam cho tile chuyen dong va on dinh animation
        if (movingTile != nullptr) {
            deltaTimeMoved = SDL_GetTicks() - lastTimeMoved;
            if (milliSecondsPerPixel < deltaTimeMoved) {
                int pixelsToMove = deltaTimeMoved / milliSecondsPerPixel;
                for (int i = 0; i < pixelsToMove; i++) {
                    doneMoving = movingTile->moveTo(emptyTile->getXPosition(), emptyTile->getYPosition());
                    if (doneMoving) {
                        emptyTile->setPositionTo(tempXPosition, tempYPosition);
                        iter_swap(movingTile, emptyTile);
                        emptyTile = movingTile;
                        movingTile = nullptr;
                        checkSolved = true;
                        break;
                    }
                }
                lastTimeMoved = SDL_GetTicks();
            }
        }

        if (checkSolved) {
            solved = true;
            for (int row = 0; row < tiles.size(); row++) {
                for (int col = 0; col < tiles[row].size(); col++) {
                    const int number = row * DIFFICULTY + col + 1;
                    if (tiles[row][col].getNumber() != number) {
                        tiles[row][col].changeColourTo(BUTTON_COLOUR);
                        tiles[row][col].changeFontColourTo(FONT_COLOUR);
                        tiles[row][col].loadTexture(renderer, to_string(tiles[row][col].getNumber()).c_str());
                        solved = false;
                    }
                    // Tile nao dung vi tri thi chuyen sang mau chinh xac
                    if (tiles[row][col].getNumber() == number) {
                        tiles[row][col].changeColourTo(TILE_COMPLETION_COLOUR);
                        tiles[row][col].changeFontColourTo(FONT_COMPLETION_COLOUR);
                        tiles[row][col].loadTexture(renderer, to_string(tiles[row][col].getNumber()).c_str());
                    }
                }
            }
            checkSolved = false;
        }

        // Neu xu ly xong thi stopwatch dung lai
        if (!solved)
            stopwatch.calculateTime(renderer);

        // On dinh FPS
        deltaTimeRendered = SDL_GetTicks() - lastTimeRendered;
        if (milliSecondsPerFrame > deltaTimeRendered) {
            SDL_Delay(milliSecondsPerFrame - deltaTimeRendered);
        }

            lastTimeRendered = SDL_GetTicks();

            // tile.loadTexture(renderer, numberStr);

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            
            stopwatch.render(renderer);

            for (int row = 0; row < tiles.size(); row++) {
                for (int col = 0; col < tiles[row].size(); col++) {
                    if (emptyTile != &tiles[row][col])
                    tiles[row][col].render(renderer);
                }
            }

            menuButton.render(renderer);

            SDL_RenderPresent(renderer);

    }

    // Giai phong
    for (int row = 0; row < tiles.size(); row++) {
        for (int col = 0; col < tiles[row].size(); col++)
            tiles[row][col].free();
    }
    stopwatch.free();

    TTF_CloseFont(font);
    font = nullptr;
}

int main( int argc, char* args[] ) {

    SDL_Window* window;
    SDL_Renderer* renderer;
    initSDL(window, renderer, WINDOW_TITLE, SCREEN_WIDTH, SCREEN_HEIGHT);

    bool exit = false;

    unsigned int difficulty;

    while (1) {
        difficulty = playMenu(renderer, &exit, SCREEN_WIDTH, SCREEN_HEIGHT);
        if (exit)
            break;

        playPuzzle(renderer, &exit, difficulty, SCREEN_WIDTH, SCREEN_HEIGHT);
        if (exit)
            break;
    }

    quitSDL(window, renderer);
    return 0;
}