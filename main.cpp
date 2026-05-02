#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <limits> // for handling unexcepted input

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
        cout << "\n    1   2   3\n";
        
        for (int i = 0; i < size; ++i) {
            cout << "  " << i + 1 << " ";
            
            for (int j = 0; j < size; ++j) {
                cout << " " << grid[i][j] << " ";
                if (j < size - 1) cout << "|";
            }
            cout << "\n";
            if (i < size - 1) cout << "   ---+---+---\n";
        }
        cout << "\n";
    }

    bool isValidMove(int row, int col) const{
        if (row < 0 || row >= size || col < 0 || col >= size) return false;
        return grid[row][col] == ' ';
    }

    bool makeMove(int row, int col, char symbol){
        if (isValidMove(row, col)){
            grid[row][col] = symbol;
            return true;
        }
        return false;
    }

    bool checkWin(char symbol) const {
        for (int i = 0; i < size; ++i) {
            if ((grid[i][0] == symbol && grid[i][1] == symbol && grid[i][2] == symbol) ||
                (grid[0][i] == symbol && grid[1][i] == symbol && grid[2][i] == symbol)) {
                return true;
            }
    }
    if ((grid[0][0] == symbol && grid[1][1] == symbol && grid[2][2] == symbol) ||
            (grid[0][2] == symbol && grid[1][1] == symbol && grid[2][0] == symbol)) {
            return true;
        }
        return false;

    }
    bool isFull() const {
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (grid[i][j] == ' ') return false;
            }
        }
        return true;
    }
};

class Player {
protected:
    string name;
    char symbol;
public:
    Player(const string& name, char symbol) : name(name), symbol(symbol) {}
    virtual ~Player() {};
    string getName() const { return name; }
    char getSymbol() const { return symbol; }
    void setName(const string& newName) { name = newName; }
    void setSymbol(char newSymbol) { symbol = newSymbol; }
    virtual void getMove(int& row, int& col, const Board& board) = 0;
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
        
    }

    void getMove(int& row, int& col, const Board& board) override {
        if(this->difficulty == Difficulty::EASY) {
            getRandomMove(board, row, col);
        } else {
            getBestMove(const_cast<Board&>(board), row, col);
        }
    }

    void setBoard(const Board* board) {
        currentBoard = board;
    }

    void setDifficulty(Difficulty newDifficulty) {
        difficulty = newDifficulty;
    }

    void getRandomMove(const Board& board, int& row, int& col) const {
        do {
            row = rand() % board.getSize();
            col = rand() % board.getSize();
        } while (!board.isValidMove(row, col));
    }

    void getBestMove(Board& board, int& row, int& col) const {
        int bestVal = -1000;
        row = -1; col = -1;

        for (int i = 0; i < board.getSize(); i++) {
            for (int j = 0; j < board.getSize(); j++) {
                if (board.isValidMove(i, j)) {
                    Board tempBoard = board;
                    tempBoard.makeMove(i, j, symbol);
                
                    int moveVal = minimax(tempBoard, 0, false);
                
                    if (moveVal > bestVal) {
                        row = i;
                        col = j;
                        bestVal = moveVal;
                    }
                }
            }
        }
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
        if(!(cin >> row >> col)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter numbers between 1 and 3.\n";
            getMove(row, col, board);
            return;
        }
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
        string name1, name2;
        cout << "Enter Player 1 Name (Plays as X): ";
        cin >> name1;
        cout << "Enter Player 2 Name (Plays as O): ";
        cin >> name2;
    
    
        delete player1; 
        delete player2;
    
        player1 = new HumanPlayer(name1, 'X');
        player2 = new HumanPlayer(name2, 'O');
        currentPlayer = player1;
    }

    void setupPvC(Difficulty difficulty) {
        string name1;
        cout << "Enter Your Name (Plays as X): ";
        cin >> name1;
    
        delete player1; 
        delete player2;
    
        player1 = new HumanPlayer(name1, 'X');
        player2 = new AIPlayer("Computer", 'O', difficulty);
        currentPlayer = player1;
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
            board.display();
            cout << currentPlayer->getName() << " wins!\n";
            resetGame();
            return true;
        } else if (board.isFull()) {
            board.display();
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
        if(!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number between 1 and 4.\n";
            continue;
        }

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
int main() {
    // SEEDS THE RANDOM NUMBER GENERATOR
    srand(static_cast<unsigned int>(time(0)));
    Game myGame;
    myGame.start();

    return 0;
}