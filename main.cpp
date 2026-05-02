#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

enum Difficulty { EASY, HARD };

class Board {
private:
    vector<vector<char>> grid;
    const int size;

public:
    Board(int size = 3) : size(size) {
        reset();
    }

    void reset() {
        grid.assign(size, vector<char>(size, ' '));
    }

    int getSize() const { return size; }
    char getCell(int row, int col) const { return grid[row][col]; }

    void display() const {
        // TO DO: Implement a function to display the board
    }

    bool Board::isValidMove(int row, int col) const{
        if (row < 0 || row >= size || col < 0 || col >= size) return false;
        return grid[row][col] == ' ';
    }

    bool Board::makeMove(int row, int col, char symbol){
        if (isValidMove(row, col)){
            grid[row][col] = symbol;
            return true;
        }
        return false;
    }

    bool checkWin(char symbol) const {
        // TO DO: Implement a function to check if the specified player has won
    }
    bool isFull() const {
        // TO DO: Implement a function to check if the board is full (no more valid moves)
    }
};

class Player {
protected:
    string name;
    char symbol;
public:
    Player(const string& name, char symbol) : name(name), symbol(symbol) {}
    virtual ~Player() {};
    virtual void getMove(int& row, int& col, const Board& board) = 0;
    string getName() const { return name; }
    char getSymbol() const { return symbol; }
    void setName(const string& newName) { name = newName; }
    void setSymbol(char newSymbol) { symbol = newSymbol; }
};

class AIPlayer : public Player {
private:
    Difficulty difficulty;
    const Board* currentBoard;

    int minimax(Board tempBoard, int depth, bool isMaximizing) const {
    int score = evaluateBoard(tempBoard);

    if (score == 10) return score - depth;
    if (score == -10) return score + depth;
    if (tempBoard.isFull()) return 0;

    char opponentSymbol = (symbol == 'X') ? 'O' : 'X';

    if (isMaximizing) {
        int best = -1000;
        for (int i = 0; i < tempBoard.getSize(); i++) {
            for (int j = 0; j < tempBoard.getSize(); j++) {
                if (tempBoard.isValidMove(i, j)) {
                    Board nextBoard = tempBoard;
                    nextBoard.makeMove(i, j, symbol);
                    best = max(best, minimax(nextBoard, depth + 1, !isMaximizing));
                }
            }
        }
        return best;
    } else {
        int best = 1000;
        for (int i = 0; i < tempBoard.getSize(); i++) {
            for (int j = 0; j < tempBoard.getSize(); j++) {
                if (tempBoard.isValidMove(i, j)) {
                    Board nextBoard = tempBoard;
                    nextBoard.makeMove(i, j, opponentSymbol);
                    best = min(best, minimax(nextBoard, depth + 1, !isMaximizing));
                }
            }
        }
        return best;
    }
}
public:
    AIPlayer(const string& name, char symbol, Difficulty difficulty)
        : Player(name, symbol), difficulty(difficulty) {
        // TO DO: Initialize random seed for random move generation
    }

    void getMove(int& row, int& col, const Board& board) override {
        // TO DO: Implement logic to select a move based on the current difficulty level
    }

    void setBoard(const Board* board) {
        currentBoard = board;
    }

    void setDifficulty(Difficulty newDifficulty) {
        difficulty = newDifficulty;
    }

    // Selects a random valid move for easy difficulty
    void getRandomMove(const Board& board, int& row, int& col) const {
        // TO DO: Implement a function to select a random valid move from the board
    }

    void getBestMove(Board& board, int& row, int& col) const {
        // TO DO: Implement the Minimax algorithm to evaluate the best move for the AI
    }

    int evaluateBoard(const Board& board) const{
        if (board.checkWin(symbol)) return 10;
        char opponentSymbol = (symbol == 'X') ? 'O' : 'X';
        if (board.checkWin(opponentSymbol)) return -10;
        return 0;
    }
};

class HumanPlayer : public Player {
public:
    HumanPlayer(const string& name, char symbol) : Player(name, symbol) {}

    void getMove(int& row, int& col, const Board& board) override {
        cout << name << "'s turn (" << symbol << "). Enter row and col (1-3): ";
        cin >> row >> col;
        row--;
        col--;
    }
};

class Game {
    private:
    Board board;
    Player* player1;
    Player* player2;
    Player* currentPlayer;

public:
    Game() : player1(nullptr), player2(nullptr), currentPlayer(nullptr) {}

    void showMenu() {
        cout << "TIC-TAC-TOE GAME\n====\n";
        cout << "1. Player vs Player\n2. Player vs Computer (Easy)\n";
        cout << "3. Player vs Computer (Hard)\n4. Exit\nSelect game mode: ";
    }

    void setupPvP() {
        // TO DO: Implement a function to set up a Player vs Player game, including getting player names and symbols
    }

    void setupPvC(Difficulty difficulty) {
        // TO DO: Implement a function to set up a Player vs Computer game, including getting player name and symbol, and initializing the AI player with the selected difficulty
    }

    void resetGame() {
        board.reset();
        currentPlayer = player1;
    }

    void switchPlayer() {
        currentPlayer = (currentPlayer == player1) ? player2 : player1;
    }

    bool checkGameEnd() {
        if (board.checkWin(currentPlayer->getSymbol())) {
            cout << currentPlayer->getName() << " wins!\n";
            resetGame();
            return true;
        } else if (board.isFull()) {
            cout << "Game Over! It's a draw!\n";
            resetGame();
            return true;
        }
        return false;
    }
    void start(){
        bool programRunning = true;

    while (programRunning) {
        showMenu();
        int choice;
        cin >> choice;

        if (choice == 1) {
            setupPvP();
        } else if (choice == 2) {
            setupPvC(Difficulty::EASY);
        } else if (choice == 3) {
            setupPvC(Difficulty::HARD);
        } else if (choice == 4) {
            cout << "Exiting game...\n";
            programRunning = false;
            continue;
        } else {
            cout << "Invalid selection. Please try again.\n";
            continue;
        }

        bool matchActive = true;
        while (matchActive) {
            board.display();
            int row, col;

            AIPlayer* ai = dynamic_cast<AIPlayer*>(currentPlayer);
            if (ai) {
                ai->setBoard(&board);
                cout << currentPlayer->getName() << " is thinking...\n";
            }

            currentPlayer->getMove(row, col, board);

            if (board.makeMove(row, col, currentPlayer->getSymbol())) {
                if (checkGameEnd()) {
                    board.display();
                    matchActive = false;
                } else {
                    switchPlayer();
                }
            } else {
                cout << "Invalid move. Cell is occupied or out of bounds. Try again.\n";
            }
        }

        char replay;
        cout << "Play again? (y/n): ";
        cin >> replay;
        if (tolower(replay) != 'y') {
            programRunning = false;
        } else {
            resetGame();
        }
    }
    }
};
