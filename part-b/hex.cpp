/*
   Hex Game Implementation in C++

   This program defines a Hex board game, where two players (Blue and Red) take turns making moves
   on a hexagonal grid. The goal is to connect their respective sides of the board.

   The HexStatus enum represents the status of a hexagon cell (EMPTY, BLUE, RED).
   The Player enum represents the current player (BLUE, RED).

   The Hex struct represents a hexagon cell with its coordinates, status, and neighboring edges.

   The HexBoard class defines the game board, including methods for playing, printing, checking
   for a winner, and handling player and AI moves.

   The main function initializes the game by specifying the board size and starts the HexBoard game.

    The AI chooses the next moves based on a MonteCarlo simulation.

*/

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <iomanip>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <sstream>

enum class HexStatus {EMPTY, BLUE, RED};

// Overloaded stream insertion operator for HexStatus enum to enable colorful printing.
std::ostream& operator<<(std::ostream& out, const HexStatus& s){
    if (s == HexStatus::EMPTY){
        out << '*';
    }
    else if (s == HexStatus::BLUE){
        out << "\033[34m" << 'X' << "\033[0m";
    }
    else {
        out << "\033[31m" << 'O' << "\033[0m";
    }
    return out;
}

enum class Player {BLUE, RED};

// Overloaded stream insertion operator for Player enum to enable colorful printing.
std::ostream& operator<<(std::ostream& out, const Player& p){
    if (p == Player::BLUE)
        out << "\033[34m" << "Blue" << "\033[0m";
    else
        out << "\033[31m" << "Red" << "\033[0m";
    return out;
}

// Hex struct representing a hexagon cell with coordinates, status, and neighboring edges.
struct Hex{

    Hex(){};
    Hex(unsigned n,unsigned i,unsigned j);
    unsigned n;
    HexStatus s;
    unsigned i, j;
    std::vector<Hex*> edges;
    void info();
};

// Constructor for Hex struct, initializing its coordinates and status.
Hex::Hex(unsigned n,unsigned i,unsigned j): n(n), i(i), j(j){
    s = HexStatus::EMPTY;
}

// Overloaded stream insertion operator for Hex struct to enable printing its status.
std::ostream& operator<<(std::ostream& out, const Hex& h){
    out << h.s;
    return out;
}

// Print detailed information about a Hex cell.
void Hex::info(){
    std::cout << "N: " << std::setw(2) << n << " | ";
    std::cout << "S: " << s << " | ";
    std::cout << "Coord: (" << i << ',' << j << ')' << " | ";
    std::cout << "Edges: ";
    for (const auto& e : edges){
        std::cout << '(' << e->i << ',' << e->j << ") ";
    }
    std::cout << std::endl;
}

// HexBoard class representing the game board and its functionality.
class HexBoard{
    public:
        HexBoard(){};
        HexBoard(unsigned n);
        ~HexBoard();
        void Play();
    private:
        void print();
        void printEdgeList();
        void printPlayerEdgeList(const Player& p);
        void randomize();
        void revertRandom();
        void clear();
        bool check(const Player& p);
        bool isLegal(const unsigned& x, const unsigned& y);
        void move(const Player& p, const unsigned& x, const unsigned& y, bool verbose = false);
        void undo(const Player& p, const unsigned& x, const unsigned& y);
        unsigned size;
        unsigned occupied = 0;
        Player last = Player::BLUE;
        std::vector<Hex*> edgeList;
        std::unordered_map<unsigned, Hex*> blueEdgeList, redEdgeList;
        std::vector<int> randomized;
        bool isOOB(const unsigned& x, const unsigned& y);
        bool isEmpty(const unsigned& x, const unsigned& y);
        void getHumanMove(unsigned& x, unsigned& y);
        void getAIMove(const Player& p, unsigned& x, unsigned& y, const unsigned& N);
        int playTurn(const Player& human, const Player& ai, const unsigned& N);
        double gwins = 0;
        void printAIConf();
};

// Constructor for HexBoard class, initializing the game board based on the specified size.
HexBoard::HexBoard(unsigned n): size(n){

    int c = 0;
    edgeList.resize(size * size);
    for (int i = 0; i < size; ++i){
        for (int j = 0; j < size; ++j){
            edgeList[c] = new Hex(c, i, j);
            if (j > 0){
                (*edgeList[c]).edges.push_back(edgeList[c-1]);
                (*edgeList[c-1]).edges.push_back(edgeList[c]);
            }
            c++;
        }
        if (i > 0){
            for (int k = 0; k < size; ++k){
                (*edgeList[k + size * i]).edges.push_back(edgeList[k + size * (i-1)]);
                (*edgeList[k + size * (i-1)]).edges.push_back(edgeList[k + size * i]);
                if (k >= 0 && k < size - 1){
                    (*edgeList[k + size * i]).edges.push_back(edgeList[k + size * (i-1) + 1]);
                    (*edgeList[k + size * (i-1) + 1]).edges.push_back(edgeList[k + size * i]);
                }
            }
        }
    }

}

// Destructor for HexBoard class, freeing allocated memory.
HexBoard::~HexBoard(){
    for (auto h : edgeList){
        delete h;
    }
    edgeList.clear();
    blueEdgeList.clear();
    redEdgeList.clear();
    randomized.clear();
    edgeList.shrink_to_fit();
    randomized.shrink_to_fit();
}

// Check if a cell is out of bounds on the game board.
bool HexBoard::isOOB(const unsigned& x, const unsigned& y){
    return !((x-1 >= 0) && (y-1 >= 0) && (x-1 < size) && (y-1 < size));
}

// Check if a cell is empty on the game board.
bool HexBoard::isEmpty(const unsigned& x, const unsigned& y){
    return (edgeList[(x-1) * size + (y-1)]->s == HexStatus::EMPTY);
}

// Check if a move is legal on the specified coordinates.
bool HexBoard::isLegal(const unsigned& x, const unsigned& y){
    return !isOOB(x, y) && isEmpty(x, y);
}

// Print the current state of the game board.
void HexBoard::print(){

    std::system("clear");

    std::cout << std::endl;
    int c = 0;
    std::cout << "  ";
    for (int i = 0; i < size; ++i)
        std::cout << std::setw(2) << (i+1) << "  ";
    std::cout << std::endl;

    for (int i = 0; i < size * 2; ++i){

        if (i % 2 == 0){
            std::cout << std::setw(2) << (i / 2) + 1 << ' ';
        } else {
            std::cout << "   ";
        }

        for (int t = 0; t < i; ++t)
            std::cout << ' ';
    
        for (int j = 0; j < size; ++j){
            if (i % 2 == 0){
                std::cout << (*edgeList[c]);
                if (j < size - 1)
                    std::cout << " - ";
                c++;
            } else {
                if (i < size * 2 - 1){
                    std::cout << "\\ " ;
                    if (j < size - 1)
                        std::cout << "/ ";
                }
            }
        }
        std::cout << std::endl;        
    }
}

// Print detailed information about each hexagon cell in the edgeList.
void HexBoard::printEdgeList(){
    for (int i = 0; i < size * size; ++i){
        edgeList[i]->info();
    }
}

// Make a move on the game board, updating the status of the specified hexagon cell.
void HexBoard::move(const Player& p, const unsigned& x, const unsigned& y, bool verbose){

    if (isLegal(x, y)){
        edgeList[(x-1) * size + (y-1)]->s = static_cast<HexStatus>(static_cast<int>(p) + 1);
        if (p == Player::BLUE)
            blueEdgeList[(x-1) * size + (y-1)] = edgeList[(x-1) * size + (y-1)];
        else
            redEdgeList[(x-1) * size + (y-1)] = edgeList[(x-1) * size + (y-1)];
        occupied++;
        if (verbose){
            print();
            std::cout << "> " << p << " occupied Hex (" ;
            std::cout<< x << "," << y << ')' << std::endl;
        }
        randomized.clear();
        last = p;
        return ;
    }
    std::cout << "> " << '(' << x << "," << y << ')';
    std::cout << " is not a legal move" << std::endl;
}

// Undo a move on the game board, reverting the status of the specified hexagon cell.
void HexBoard::undo(const Player& p, const unsigned& x, const unsigned& y){
    
    if ((!isOOB(x, y) && !isEmpty(x, y))){
        edgeList[(x-1) * size + (y-1)]->s = HexStatus::EMPTY;
        occupied--;
        if (p == Player::BLUE && edgeList[(x-1) * size + (y-1)]->s == HexStatus::BLUE){
            blueEdgeList.erase((x-1) * size + (y-1));
            last = Player::RED;
        } else if (p == Player::RED && edgeList[(x-1) * size + (y-1)]->s == HexStatus::RED) {
            redEdgeList.erase((x-1) * size + (y-1));
            last = Player::BLUE;
        }       
    }
}

// Randomly assign hexagon cells to players, simulating a game.
void HexBoard::randomize(){

    std::random_device rd;
    std::mt19937 g(rd());

    if (randomized.size() == 0){

        unsigned freeHexes = (size*size) - occupied;
        unsigned ForBlue = freeHexes / 2, ForRed = freeHexes / 2;

        if (freeHexes % 2 != 0){
            if (last == Player::BLUE)
                ForRed++;
            else
                ForBlue++;
        }
        
        std::vector<HexStatus> v(ForBlue, HexStatus::BLUE), v1(ForRed, HexStatus::RED);
        v.insert(v.end(), v1.begin(), v1.end());
        std::shuffle(v.begin(), v.end(), g);

        int c = 0;
        randomized.resize(freeHexes);
        for (int i = 0; i < size * size; ++i){
            if (edgeList[i]->s == HexStatus::EMPTY){
                edgeList[i]->s = v[c];
                if (v[c] == HexStatus::BLUE)
                    blueEdgeList[i] = edgeList[i];
                else
                    redEdgeList[i] = edgeList[i];
                randomized[c] = i;
                c++;
            }
        }

    } else {

        std::vector<HexStatus> v(randomized.size());
        for (int i = 0; i < randomized.size(); ++i){
            v[i] = edgeList[randomized[i]]->s;
        }

        std::shuffle(v.begin(), v.end(), g);
        for (int i = 0; i < randomized.size(); ++i){
            if (v[i] != edgeList[randomized[i]]->s){
                if (v[i] == HexStatus::BLUE){
                    redEdgeList.erase(randomized[i]);
                    blueEdgeList[randomized[i]] = edgeList[randomized[i]];
                } else {
                    blueEdgeList.erase(randomized[i]);
                    redEdgeList[randomized[i]] = edgeList[randomized[i]];
                }
                edgeList[randomized[i]]->s = v[i];
            }
        }

    }
    occupied = size * size;
}

// Revert the random assignment, undoing the changes made by randomize().
void HexBoard::revertRandom(){
    for (const auto& h : randomized){

        if (edgeList[h]->s == HexStatus::BLUE)
            blueEdgeList.erase(h);
        else
            redEdgeList.erase(h);

        edgeList[h]->s = HexStatus::EMPTY;
        occupied--;
    }
}

// Clear the game board, resetting it to its initial state.
void HexBoard::clear(){
    for (int i = 0; i < size * size; ++i){
        edgeList[i]->s = HexStatus::EMPTY;
    }
    blueEdgeList.clear();
    redEdgeList.clear();
    randomized.clear();
    occupied = 0;
}

// Print detailed information about each hexagon cell of a specific player in their edgeList.
void HexBoard::printPlayerEdgeList(const Player& p){
    if (p == Player::BLUE){
        for (auto& e : blueEdgeList){
            (e.second)->info();
        }
    } else {
        for (auto& e : redEdgeList){
            (e.second)->info();
        }
    }
}

// Check if a player has won the game by connecting their respective sides.
bool HexBoard::check(const Player& p){

    std::unordered_set<unsigned> end_nodes;
    std::stack<unsigned> Q;

    unsigned B = static_cast<unsigned>(p == Player::BLUE);
    for (int i = 0; i < size; ++i){

        if (B) {
            if (blueEdgeList.count(size*(size - 1) + i))
                Q.push(size*(size - 1) + i);
            if (blueEdgeList.count(i))
                end_nodes.insert(i);
        } else {
            if (redEdgeList.count(i*size))
                Q.push(i*size);
            if (redEdgeList.count(i*size + size - 1))
                end_nodes.insert(i*size + size - 1);
        }
    }

    bool w = false;
    HexStatus I = (B != 0) ? HexStatus::BLUE : HexStatus::RED;
    std::unordered_set<unsigned> visited, queued;
    if (!Q.empty() && (end_nodes.size() > 0)){

        while (!Q.empty() && !w){

            auto top = Q.top();
            Q.pop();
            visited.insert(top);
            queued.erase(top);

            for (const auto& e : edgeList[top]->edges){
                if ((e->s == I) && !visited.count(e->n) && !queued.count(e->n)){
                    if (end_nodes.count(e->n)){
                        w = !w;
                        break;
                    }
                    Q.push(e->n);
                    queued.insert(e->n);
                }
            }
        }
    }

    return w;
}

// Get a move from the human player.
void HexBoard::getHumanMove(unsigned& x, unsigned& y){

        std::cout << std::endl;
        std::cout << "> " << "Insert move: " << std::endl;
        std::cout << "> X: ";
        std::cin >> x;
        std::cout << "> Y: ";
        std::cin >> y;
}

// Get a move from the AI player based on specified difficulty and number of simulations.
void HexBoard::getAIMove(const Player& p, unsigned& x, unsigned& y, const unsigned& N){

    unsigned tx, ty;
    double wins = 0;
    gwins = 0;
    for (int i = 0; i < size*size; ++i){
        
        tx = i / size + 1;
        ty = i % size + 1;

        if (isLegal(tx, ty)){

            wins = 0;
            move(p, tx, ty);
            for (int j = 0; j < N; ++j){
                randomize();
                if (check(p))
                    wins++;
            }
            wins /= N;
            if (gwins < wins){
                gwins = wins;
                x = tx;
                y = ty;
            }
            revertRandom();
            undo(p, tx, ty);
        }
    }
}

// Play a turn of the game, alternating between human and AI players.
int HexBoard::playTurn(const Player& human, const Player& ai, const unsigned& N){

    unsigned x, y, aix, aiy;

    if (human == Player::BLUE){

        getHumanMove(x, y);
        while (!isLegal(x, y)){
            getHumanMove(x, y);
        }
        move(human, x, y, true);
        if (check(human))
            return 0;

        getAIMove(ai, aix, aiy, N);
        move(ai, aix, aiy, true);
        printAIConf();
        if (check(ai))
            return 1;

    } else {

        getAIMove(ai, aix, aiy, N);
        move(ai, aix, aiy, true);
        printAIConf();
        if (check(ai))
            return 1;

        getHumanMove(x, y);
        while (!isLegal(x, y)){
            getHumanMove(x, y);
        }
        move(human, x, y, true);
        check(human);
        if (check(human))
            return 0;

    }

    return 2;
}

// Main function for playing the HexBoard game.
void HexBoard::Play(){

    std::cout << "> " << "Choose Player:" << std::endl;
    std::cout << "> " << Player::BLUE << " [1]" << std::endl;
    std::cout << "> " << Player::RED << " [2]" << std::endl;

    int n;
    std::cin >> n;
    std::cout << std::endl;

    Player human = static_cast<Player>(n-1);
    Player ai = (human == Player::BLUE) ? Player::RED : Player::BLUE;

    int diff;
    std::cout << "> " << "Choose difficuly [Easy 1, Medium 2, Hard 3]:" << std::endl;
    std::cin >> diff;
    std::cin.clear();

    unsigned N = 1025;
    switch (diff){
        case (1):
            N = 257;
            break;
        case (2):
            N = 513;
            break;
    }

    print();

    int result = 2;
    while (result == 2){
        result = playTurn(human, ai, N);
    }
    
    std::cout << std::endl;
    Player winner = static_cast<Player>(result);    
    std::cout << "> " << winner << " has won!" << std::endl;

}

// Print the confidence of winning for the AI.
void HexBoard::printAIConf(){

    std::cout << "> AI Confidence: ";
    std::cout << std::setprecision(2) << gwins << ' ';

    if (gwins < 0.3){
        std::cout << "(ç_ç)";
    } else if (gwins >= 0.3 && gwins < 0.6){
        std::cout << "(o_o)";
    } else if (gwins >= 0.6 && gwins < 0.85){
        std::cout << "(ù_ù)";
    } else {
        std::cout << "\\($_$)/";
    }
    std::cout << std::endl;

}


int main(void){

    int size;
    std::cout << "> " << "Choose the HexBoard size [size x size]: ";
    std::cin >> size;
    std::cout << std::endl;

    HexBoard HB(std::max(3, size));
    HB.Play();

    return 0;
}