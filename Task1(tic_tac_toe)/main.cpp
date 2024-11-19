#include <iostream>
#include <vector>
#include "TicTacToeNetwork.h"  // Include the networking header for server/client setup

class TicTacToe {
private:
    std::vector<std::vector<char>> board;
    char currPlayer;

public:
    TicTacToe() : board(3, std::vector<char>(3, ' ')), currPlayer('X') {}

    void display_Board() {
        std::cout << "\n";
        for (const auto& row : board) {
            for (const auto& cell : row) {
                std::cout << (cell == ' ' ? '.' : cell) << " ";
            }
            std::cout << "\n";
        }
    }

    bool make_Move(int row, int col) {
        if (row < 0 || row >= 3 || col < 0 || col >= 3 || board[row][col] != ' ') {
            return false;
        }
        board[row][col] = currPlayer;
        currPlayer = (currPlayer == 'X') ? 'O' : 'X';
        return true;
    }

    bool check_Winner(char player) {
        for (int i = 0; i < 3; ++i) {
            if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) ||
                (board[0][i] == player && board[1][i] == player && board[2][i] == player)) {
                return true;
            }
        }
        if ((board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
            (board[0][2] == player && board[1][1] == player && board[2][0] == player)) {
            return true;
        }
        return false;
    }

    bool is_Draw() {
        for (const auto& row : board) {
            for (char cell : row) {
                if (cell == ' ') return false;
            }
        }
        return true;
    }

    char getCurrPlayer() const {
        return currPlayer;
    }
};

int main() {
    TicTacToe game;
    TicTacToeNetwork network("127.0.0.1", 8080);  // Setup network with localhost and port 8080

    int playerType;
    std::cout << "Enter 1 to host the game (server), or 2 to join (client): ";
    std::cin >> playerType;

    // Set up as either server (1) or client (2)
    if (playerType == 1) {
        network.startServer();
        std::cout << "Waiting for a player to join...\n";
        network.acceptConnection();
        std::cout << "Player connected!\n";
    } else if (playerType == 2) {
        network.connectToServer();
        std::cout << "Connected to the server!\n";
    } else {
        std::cout << "Invalid choice, exiting.\n";
        return 1;
    }

    game.display_Board();
    int row, col;

    while (true) {
        // Server’s turn to make a move
        if (playerType == 1) {
            std::cout << "Your turn (enter row and column): ";
            std::cin >> row >> col;

            while (!game.make_Move(row, col)) {
                std::cout << "Invalid move. Try again: ";
                std::cin >> row >> col;
            }

            network.send_Move(row, col);  // Send move to the client
            game.display_Board();

            if (game.check_Winner('X')) {
                std::cout << "Player X wins!\n";
                break;
            }
            if (game.is_Draw()) {
                std::cout << "It's a draw!\n";
                break;
            }

            std::cout << "Waiting for the other player's move...\n";
            network.receive_Move(row, col);  // Receive move from client
            game.make_Move(row, col);
            game.display_Board();

            if (game.check_Winner('O')) {
                std::cout << "Player O wins!\n";
                break;
            }
            if (game.is_Draw()) {
                std::cout << "It's a draw!\n";
                break;
            }
        }

        // Client’s turn to make a move
        else if (playerType == 2) {
            std::cout << "Waiting for the other player's move...\n";
            network.receive_Move(row, col);  // Receive move from server
            game.make_Move(row, col);
            game.display_Board();

            if (game.check_Winner('X')) {
                std::cout << "Player X wins!\n";
                break;
            }
            if (game.is_Draw()) {
                std::cout << "It's a draw!\n";
                break;
            }

            std::cout << "Your turn (enter row and column): ";
            std::cin >> row >> col;

            while (!game.make_Move(row, col)) {
                std::cout << "Invalid move. Try again: ";
                std::cin >> row >> col;
            }

            network.send_Move(row, col);  // Send move to the server
            game.display_Board();

            if (game.check_Winner('O')) {
                std::cout << "Player O wins!\n";
                break;
            }
            if (game.is_Draw()) {
                std::cout << "It's a draw!\n";
                break;
            }
        }
    }
    return 0;
}
