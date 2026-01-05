#include <iostream>
#include <array>
#include <vector>
using namespace std;

constexpr int SIZE = 8;
constexpr int dx[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
constexpr int dy[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };

namespace Reversi {
    struct Coord {
        int x, y;
    };
    
    enum class Disc: int {
        Empty = -1,
        White = 0,
        Black = 1
    };

    Disc flipped(const Disc& disc) {
        switch (disc) {
        case Disc::White:
            return Disc::Black;
        case Disc::Black:
            return Disc::White;
        default:
            break;
        }
        return Disc::Empty;
    }

    using grid = array<array<Disc, SIZE>, SIZE>;
    struct CountResult {
        int mine, theirs;
        float percentage;
    };
    void initBoard(grid& board) {
        for (int y=0; y<SIZE; y++) {
            for (int x=0; x<SIZE; x++) {
                board[y][x] = Disc::Empty;
            }
        }
        board[3][3] = Disc::White;
        board[4][4] = Disc::White;
        board[4][3] = Disc::Black;
        board[3][4] = Disc::Black;
    }
    // The move must be legal when calling place()
    void place(grid& board, const Disc& turn, const Coord& move) {
        board[move.y][move.x] = turn;
        for (int d = 0; d < 8; d++) {
            int nx = move.x + dx[d];
            int ny = move.y + dy[d];
            // Check bounds
            if (nx < 0 || ny < 0 || nx >= SIZE || ny >= SIZE) continue;
            if (board[ny][nx] != flipped(turn)) continue;
            vector<Coord> cand;
            cand.push_back({nx, ny});
            nx += dx[d];
            ny += dy[d];
            while (!(nx < 0 || ny < 0 || nx >= SIZE || ny >= SIZE)) {
                if (board[ny][nx] == turn) {
                    for (auto c: cand) {
                        board[c.y][c.x] = turn;
                    }
                    break;
                };
                if (board[ny][nx] == flipped(turn)) {
                    cand.push_back({nx, ny});
                }
                nx += dx[d];
                ny += dy[d];
            }
        }
    }
    bool isFull(const grid& board) {
        for (int y=0;y<SIZE;y++) {
            for (int x=0;x<SIZE;x++) {
                if (board[y][x] == Disc::Empty) return false;
            }
        }
        return true;
    }
}

// Check if the move is legal
bool isLegal(const Reversi::grid& board, const Reversi::Disc& turn, const Reversi::Coord& move) {
    if (board[move.y][move.x] != Reversi::Disc::Empty) return false;
    for (int d = 0; d < 8; d++) {
        int nx = move.x + dx[d];
        int ny = move.y + dy[d];
        // Check bounds
        if (nx < 0 || ny < 0 || nx >= SIZE || ny >= SIZE) continue;
        if (board[ny][nx] != Reversi::flipped(turn)) continue;
        nx += dx[d];
        ny += dy[d];
        while (!(nx < 0 || ny < 0 || nx >= SIZE || ny >= SIZE)) {
            if (board[ny][nx] == turn) return true;
            if (board[ny][nx] != Reversi::flipped(turn)) break;
            nx += dx[d];
            ny += dy[d];
        }
    }
    return false;
}

// Generate legal moves
vector<Reversi::Coord> generateLegalMoves(const Reversi::grid& board, const Reversi::Disc& turn) {
    vector<Reversi::Coord> moves;
    for (int y=0; y<SIZE; y++) {
        for (int x=0; x<SIZE; x++) {
            if (board[y][x] == Reversi::Disc::Empty) {
                if (isLegal(board, turn, {x, y})) {
                    // Legal
                    moves.push_back({x, y});
                }
            }
        }
    }
    return moves;
}

Reversi::CountResult countDiscs(const Reversi::grid& board, const Reversi::Disc& turn) {
    int all_disc_count = 0;
    int my_disc_count = 0;
    int their_disc_count = 0;
    float percentage = 0;
    for (int y=0; y<SIZE; y++) {
        for (int x=0; x<SIZE; x++) {
            if (board[y][x] != Reversi::Disc::Empty) {
                all_disc_count++;
            }
            if (board[y][x] == turn) {
                my_disc_count++;
            }
            if (board[y][x] == Reversi::flipped(turn)) {
                their_disc_count++;
            }
        }
    }
    if (my_disc_count != 0 && their_disc_count != 0) {
        percentage = 1 / ((float)my_disc_count/(float)their_disc_count + 1);
    }
    return {
        my_disc_count,
        their_disc_count,
        percentage
    };
}

float evaluate(const Reversi::grid& board, const Reversi::Disc& turn) {
    Reversi::CountResult countResult = countDiscs(board, turn);
    float mine = countResult.mine * 0.8; // max: 64
    float theirs = countResult.theirs * -1.2; // max: 64
    float percentage = countResult.percentage * 100; // max: 1
    return mine + theirs + percentage;
}

Reversi::Coord bestMove(const Reversi::grid& board, const Reversi::Disc& turn) {
    vector<Reversi::Coord> candidates = generateLegalMoves(board, turn);
    pair<float, Reversi::Coord> scoreAndMove = make_pair(-50000, candidates[0]);
    for (auto candidate: candidates) {
        Reversi::grid board_next = board;
        Reversi::place(board_next, turn, candidate);
        float score = evaluate(board, turn);
        if (scoreAndMove.first < score) {
            scoreAndMove = make_pair(score, candidate);
        }
    }
    // cout << scoreAndMove.first << endl;
    return scoreAndMove.second;
}

void play() {
    cout << "Select your color [b=black, w=white, r=random]: " << endl;
    string colorstr;
    cin >> colorstr;
    Reversi::Disc player = Reversi::Disc::Black;
    if (colorstr[0] == 'b') {
        player = Reversi::Disc::Black;
    }
    if (colorstr[0] == 'w') {
        player = Reversi::Disc::White;
    }
    if (colorstr[0] == 'r') {
        if (rand() % 2 == 0) {
            player = Reversi::Disc::Black;
            cout << "Your side is Black." << endl;
        } else {
            player = Reversi::Disc::White;
            cout << "Your side is Bhite." << endl;
        }
    }
    Reversi::grid board;
    Reversi::initBoard(board);
    Reversi::Disc turn = Reversi::Disc::Black;
    bool prevPlayerPassed = false;
    while (true) {
        if (Reversi::isFull(board)) break;
        if (generateLegalMoves(board, turn).size() == 0) {
            // pass
            if (prevPlayerPassed) {
                break;
            }
            turn = Reversi::flipped(turn);
            prevPlayerPassed = true;
            cout << "pass" << endl;
            continue;
        }
        if (turn == player) {
            // player turn
            string move;
            cin >> move;
            int x = move[0] - 'a';
            int y = move[1] - '1';
            Reversi::Coord coord = {x, y};
            if (isLegal(board, turn, coord)) {
                Reversi::place(board, turn, coord);
            } else {
                cout << "error: invalid move." << endl;
                continue;
            }
        } else {
            // AI turn
            Reversi::Coord best = bestMove(board, turn);
            cout << (char)(best.x + 'a') << (char)(best.y + '1') << endl;
            Reversi::place(board, turn, best);
        }
        turn = Reversi::flipped(turn);
        for (int y=0;y<SIZE;y++) {
            for (int x=0;x<SIZE;x++) {
                if (board[y][x] == Reversi::Disc::Empty) {
                    cout << "\e[42m \e[0m";
                } else if (board[y][x] == Reversi::Disc::White) {
                    cout << "\e[47mW\e[0m";
                } else if (board[y][x] == Reversi::Disc::Black) {
                    cout << "\e[40mB\e[0m";
                }
            }
            cout << endl;
        }
    }
    int emptyCount = 0, blackCount = 0, whiteCount = 0;
    for (int y=0;y<SIZE;y++) {
        for (int x=0;x<SIZE;x++) {
            if (board[y][x] == Reversi::Disc::Empty) {
                emptyCount++;
            } else if (board[y][x] == Reversi::Disc::Black) {
                blackCount++;
            } else if (board[y][x] == Reversi::Disc::White) {
                whiteCount++;
            }
        }
    }
    if (blackCount < whiteCount) {
        cout << "Result: White wins (" << (whiteCount + emptyCount) << ":" << blackCount << ")" << endl;
    } else if (whiteCount < blackCount) {
        cout << "Result: Black wins (" << (whiteCount + emptyCount) << ":" << blackCount << ")" << endl;
    } else {
        cout << "Result: Tie (" << whiteCount << ":" << blackCount << ")" << endl;
    }
}
int main() {
    srand(time(0));
    play();
    return 0;
}
