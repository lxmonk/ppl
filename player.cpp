/*
 Copyright © 2010 Intel Corporation.  All rights reserved
 
 The source code, information and material ("Material") contained herein is owned by Intel Corporation or its suppliers or licensors, and title to such Material remains with Intel Corporation or its suppliers or licensors. The Material contains proprietary information of Intel or its suppliers and licensors. The Material is protected by worldwide copyright laws and treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted, transmitted, distributed or disclosed in any way without Intel's prior express written permission. No license under any patent, copyright or other intellectual property rights in the Material is granted to or conferred upon you, either expressly, by implication, inducement, estoppel or otherwise. Any license under such intellectual property rights must be express and approved by Intel in writing.
 Other names and brands may be claimed as the property of others. Third Party trademarks are the property of their respective owners.
 Unless otherwise agreed by Intel in writing, you may not remove or alter this notice or any other notice embedded in Materials by Intel or Intel’s suppliers or licensors in any way
 INTEL ASSUMES NO LIABILITY WHATSOEVER AND INTEL DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTY, RELATING TO SALE AND/OR USE OF INTEL MATERIAL INCLUDING LIABILITY OR WARRANTIES RELATING TO FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABILITY, OR INFRINGEMENT OF ANY PATENT, COPYRIGHT OR OTHER INTELLECTUAL PROPERTY RIGHT.

 UNLESS OTHERWISE AGREED IN WRITING BY INTEL, THE INTEL MATERIAL ARE NOT DESIGNED NOR INTENDED FOR ANY APPLICATION IN WHICH THE FAILURE OF THE INTEL MATERIAL COULD CREATE A SITUATION WHERE PERSONAL INJURY OR DEATH MAY OCCUR.

 Intel may make changes to specifications and descriptions at any time, without notice. Designers must not rely on the absence or characteristics of any features or instructions marked "reserved" or "undefined." Intel reserves these for future definition and shall have no responsibility whatsoever for conflicts or incompatibilities arising from future changes to them. The information here is subject to change without notice. Do not finalize a design with this information.

 The material described in this document may contain design defects or errors known as errata which may cause the material to deviate from actual specifications. Current characterized errata are available on request

 Any software source code reprinted in this document is furnished under a software license and may only be used or copied in accordance with the terms of that license.

 */

///////////////////////////////////////////////////////////
// Slinga Game. designed and written by Gyuszi Suto 2009 //
///////////////////////////////////////////////////////////

// This is a reference implementation provided for the Programming Contest
// Feel free to use it, copy it, change it at your will

// compilation command:
// g++ -O3 -o slinga -finline-functions -finline-functions-called-once player.cpp -lc

#include <iostream>
#include <fstream> 
#include <string>
#include <cstring>
#include <cassert>
#include <limits>
#include <cstdlib>
#include <sstream>
#include <tr1/array>
#include <bitset>

using namespace std;
using namespace std::tr1;
// TR1 (technical report 1) part of new C++0x standard

/////////////////
// typedefs
/////////////////

typedef pair<unsigned int, unsigned int> Point;

/////////////////////////////////////////////
// global variables  (the fewer, the better)
/////////////////////////////////////////////

size_t const BOARD_SIZE = 10; // fixed for the 2009 contest to 10

////////////////////////
// forward declaration
////////////////////////

class Square;

/////////////////////////
// typedefs after globals
/////////////////////////

typedef array<array<Square, BOARD_SIZE> , BOARD_SIZE> Board; // the board type

///////////////////////////////////////////////////////////
// Score - represents the score of the board at some depth
///////////////////////////////////////////////////////////
class Score {
private:
	double score_; // the score, integer may suffice at this point, but double may be needed for fine-tuning
	unsigned int depth_; // the depth that this score was evaluated at
	int rand_; // randomizes equal scores to avoid repetitive behavior
public:
	Score(double sc = 0., int d = 0) :
		score_(sc), depth_(d), rand_(rand()) {
	}
	Score(Score const & rs) :
		score_(rs.score_), depth_(rs.depth_), rand_(rs.rand_) {
	}
	double get_score_value(void) const {
		return score_;
	}
	void set_depth(unsigned int d) {
		depth_ = d;
	}

	bool operator <(Score const & rs) const {
		if (score_ < rs.score_)
			return true;
		if (score_ > rs.score_)
			return false;
		if (depth_ > rs.depth_)
			return true; // same score at deeper depth is worse
		if (depth_ < rs.depth_)
			return false; // same score at shallower depth is better
		return rand_ < rs.rand_; // final tie-breaker using the random component
	}

	bool operator ==(Score const & rs) const {
		return !(*this < rs) && !(rs < *this);
	}
	friend ostream & operator <<(ostream & o, Score const & rs);
};

ostream & operator <<(ostream & o, Score const & rs) {
	return o << rs.score_ << ":" << rs.depth_;
}

Score const MIN_SCORE(-1., 0);
Score const MAX_SCORE(10000., 0);

////////////////////////////////////////////////////////////////////////////////
// Player, used to identify the player and the opponent - does not keep a state
////////////////////////////////////////////////////////////////////////////////
class Player {
private:
	char c_; // identifies the player ('P' - player, or 'O' - opponent)
public:
	explicit Player(char c) :
		c_(c) {
	} // avoiding accidental casting
	Player(Player const & rs) :
		c_(rs.c_) {
	}
	Player & operator =(Player const & rs) {
		assert(c_ != '#'); // never assign to off board square
		c_ = rs.c_;
		return *this;
	}

	bool operator ==(Player const & rs) const {
		return c_ == rs.c_;
	}
	size_t index(void) const {
		return c_ == 'P' ? 0 : 1;
	} // needed for accessing a few tables

	friend bool operator ==(Square const & sq, Player const & p);
	friend bool operator !=(Square const & sq, Player const & p);
	friend bool operator ==(Player const & p, Square const & sq);
	friend bool operator !=(Player const & p, Square const & sq);
	friend ostream & operator <<(ostream & o, Player const & rs);
	friend class Square;
};

ostream & operator <<(ostream & o, Player const & rs) {
	return o << rs.c_;
}

/////////////////////////////////////////////////////
// Square: one square on the board - may host a stone
/////////////////////////////////////////////////////
class Square {
private:
	char c_; // represents one square of the game board
	static char const EMPTY_ = '.';
	static char const OFF_BOARD_ = '#';
public:
	Square(char c = OFF_BOARD_) :
		c_(c) {
	}
	Square(Square const & rs) :
		c_(rs.c_) {
	}
	~Square() {
	}
	Square & operator =(Square const & rs) {
		c_ = rs.c_;
		return *this;
	}
	bool operator ==(Square const & rs) const {
		return c_ == rs.c_;
	}
	bool is_empty() const {
		return c_ == EMPTY_;
	}
	void set_to_empty() {
		c_ = EMPTY_;
	}
	void set_to(Player const & s) {
		c_ = s.c_;
	}
	bool is_player(Player const & s) const {
		return c_ == s.c_;
	}
	void set_to_off_board(void) {
		c_ = OFF_BOARD_;
	}
	bool is_on_board() const {
		return c_ != OFF_BOARD_;
	}
	bool is_off_board() const {
		return c_ == OFF_BOARD_;
	}

	friend istream & operator >>(istream &, Square &);
	friend ostream & operator <<(ostream &, Square const &);
	friend bool operator ==(Square const & sq, Player const & p);
	friend bool operator !=(Square const & sq, Player const & p);
	friend bool operator ==(Player const & p, Square const & sq);
	friend bool operator !=(Player const & p, Square const & sq);
};

inline bool operator ==(Square const & sq, Player const & p) {
	return sq.c_ == p.c_;
}
inline bool operator !=(Square const & sq, Player const & p) {
	return !(sq == p);
}
inline bool operator ==(Player const & p, Square const & sq) {
	return sq == p;
}
inline bool operator !=(Player const & p, Square const & sq) {
	return sq != p;
}
istream & operator >>(istream & i, Square & s) {
	return i >> s.c_;
}
ostream & operator <<(ostream & o, Square const & s) {
	return o << s.c_;
}

/////////////////////////////////////////////////
// Move: defines one potential move for a player
/////////////////////////////////////////////////
class Move {
public:
	enum MOVE_KIND {
		STEP = 1, SLING = 2, SHOOT = 3
	};
	MOVE_KIND move_kind;
	int x, y; // making move from this square
	int xd, yd; // values either -1,0,1 specifies one of the 8 angles of legal move
	// e.g. -1,1 would specify NW
	void set_square(int xx, int yy) {
		x = xx;
		y = yy;
	}
	Move(int xx, int yy, int xxd, int yyd, MOVE_KIND m = STEP) :
		move_kind(m), x(xx), y(yy), xd(xxd), yd(yyd) {
	}
	Move(int xxd, int yyd, MOVE_KIND m = STEP) :
		move_kind(m), x(0), y(0), xd(xxd), yd(yyd) {
	}

	void print_move_taken(ostream & o) {
		static array<string, 4> const sss = { "no_move", "step", "sling",
				"shoot" };
		o << sss[move_kind];
		o << " " << x << "," << y;
		static array<string, 3> const dir0 = { "SW", "S", "SE" };
		static array<string, 3> const dir1 = { "W", "NO_MOVE", "E" };
		static array<string, 3> const dir2 = { "NW", "N", "NE" };
		static array<array<string, 3> , 3> const directions = { dir0, dir1,
				dir2 };
		o << " " << directions[1 + yd][1 + xd] << "\n";
	}
};

//////////////////////////
// defines the legal moves 
//////////////////////////
typedef array<Move, 24> All_Moves;
All_Moves moves = { Move(-1, -1, Move::STEP), // SW
		Move(-1, 0, Move::STEP), // W
		Move(-1, 1, Move::STEP), // NW

		Move(0, -1, Move::STEP), // S
		Move(0, 1, Move::STEP), // N

		Move(1, -1, Move::STEP), // SE
		Move(1, 0, Move::STEP), // E
		Move(1, 1, Move::STEP), // NE
		/////////////
		Move(-1, -1, Move::SLING), // SW
		Move(-1, 0, Move::SLING), // W
		Move(-1, 1, Move::SLING), // NW

		Move(0, -1, Move::SLING), // S
		Move(0, 1, Move::SLING), // N

		Move(1, -1, Move::SLING), // SE
		Move(1, 0, Move::SLING), // E
		Move(1, 1, Move::SLING), // NE
		/////////////
		Move(-1, -1, Move::SHOOT), // SW
		Move(-1, 0, Move::SHOOT), // W
		Move(-1, 1, Move::SHOOT), // NW

		Move(0, -1, Move::SHOOT), // S
		Move(0, 1, Move::SHOOT), // N

		Move(1, -1, Move::SHOOT), // SE
		Move(1, 0, Move::SHOOT), // E
		Move(1, 1, Move::SHOOT) // NE
		};

//////////////////////////////////////////
// Attack class, containing attacer's location (point), Move
//////////////////////////////////////////

class Attack {
public:
	Point attacker;
	Move move;

	Attack() : move(-1,-1, Move::STEP) {}
};

//////////////////////////////////////////
// main class of game Slinga
//////////////////////////////////////////
class Slinga {
private:
	Board board_; // a 2D array of Squares
	Square off_board_square_; // an extra squre needed to return reference to in case we step off
	int size_; // size of the board - may be superfluous if we fix the board at size 10
	array<size_t, 2> stone_count_; // storing the stone count for the 2 players
	array<double, 2> remaining_time_; // remaining time for each of the players
	array<double, 2> used_time_; // used time so far for the 2 players
	array<size_t, 2> moves_made_; // moves made so far for the 2 players
	//	Depth depth_; // depth in exploration of a step, must be even number to be meaningul
	array<pair<int, int> , 10> opponent_stones;
	array<pair<int, int> , 10> player_stones;

public:

	// default constructor
	Slinga() :
		size_(0) {
		off_board_square_.set_to_off_board();
	}

	// construct Slinga from input stream
	Slinga(istream & in) :
		size_(BOARD_SIZE)/*, depth_(0)*/{
		stone_count_[0] = 0;
		stone_count_[1] = 0;
		off_board_square_.set_to_off_board();
		string s;
		char c[100]; // temporary

		// reading
		// PlayerTimeRemain=55.40
		in.getline(c, 100, '=');
		in >> remaining_time_[0];

		// reading
		// PlayerTimeUsed=4.60
		in.getline(c, 100, '=');
		in >> used_time_[0];

		// reading
		// PlayerMovesMade=5
		in.getline(c, 100, '=');
		in >> moves_made_[0];

		// reading
		// OpponentTimeRemain=35
		in.getline(c, 100, '=');
		in >> remaining_time_[1];

		// reading
		// OpponentTimeUsed=24
		in.getline(c, 100, '=');
		in >> used_time_[1];

		// reading
		// OpponentMovesMade=6
		in.getline(c, 100, '=');
		in >> moves_made_[1];

		string ts;
		in >> ts;
		assert(ts == "[board]");

		Player player('P');
		Player opponent('O');

		// reading the board
		for (int y = size_ - 1; y >= 0; --y) {
			for (int x = 0; x < size_; ++x) {
				char tc;
				in >> tc;
				Player temp_player(tc);
				if (temp_player == player) {
					player_stones[stone_count_[player.index()]] = make_pair(x,
							y);
					++stone_count_[player.index()];
				} else if (temp_player == opponent) {
					opponent_stones[++stone_count_[opponent.index()]]
							= make_pair(x, y);
					++stone_count_[opponent.index()];
				}
				board_[x][y].set_to(temp_player);
			}
		}
	}

	Slinga(Slinga const & rs) :
		board_(rs.board_), off_board_square_(rs.off_board_square_), size_(
				rs.size_), stone_count_(rs.stone_count_), remaining_time_(
				rs.remaining_time_), used_time_(rs.used_time_), moves_made_(
				rs.moves_made_)/*, depth_(rs.depth_ + 1) */// !!! depth is incremented with every copy
	{
	}

	Slinga & operator =(Slinga const & rs) {
		assert(false); // not allowed to assign one Slinga to another
		return *this;
	}

	size_t get_stone_count(Player const & p) const {
		return stone_count_[p.index()];
	}

	// const version
	Square const & get_square(int x, int y) const {
		if (0 <= x && x < size_ && 0 <= y && y < size_) {
			return board_[x][y];
		}
		return off_board_square_;
	}

	// non-const version
	Square & get_square(int x, int y) {
		if (0 <= x && x < size_ && 0 <= y && y < size_) {
			return board_[x][y];
		}
		return off_board_square_;
	}

	bool is_move_legal(int x, int y, Move const & st, Player const & p,
			Player const & o) const {
		if (get_square(x, y) != p)
			return false; // the square I want to make the move from does not host player p

		if (st.move_kind == Move::STEP) {
			Square const & c = get_square(x + st.xd, y + st.yd);
			// we can only step on neighbor in given direction
			// if it is occupied by opponent's stone or is empty
			return (c == o || c.is_empty());
		} else if (st.move_kind == Move::SLING) {
			if (get_square(x - st.xd, y - st.yd) != p)
				return false; // we need a sling consisting of 2 ps
			if (!get_square(x + st.xd, y + st.yd).is_empty())
				return false; // adjacent neighbor is not empty
			Square const & c = get_square(x + 2 * st.xd, y + 2 * st.yd); // get second order neighbor
			return c.is_empty() || (c == o); // only sling to empty square or one that has opponent
		} else if (st.move_kind == Move::SHOOT) {
			Square const & c = get_square(x + st.xd, y + st.yd);
			// we need to have a neighbor in the given direction that is either empty or has and opp't stone
			return ((c == o || c.is_empty()) && // neighbor square is opp't or empty
					(get_square(x - st.xd, y - st.yd) == p) && // 2 p's next to each other
					(get_square(x - 2 * st.xd, y - 2 * st.yd) == p)); // 3 p's next to each other, we have a rifle
		}
		return false; // just to get rid of compiler warning
	}

private:
	Score compute_score_(int x, int y) const {
		//
		// This is one implementation of the scoring function
		// It is by far not the best, feel free to change it as you desire
		//
		// let x be the number of ps, y the number of os
		// Score(x, y) = M * x + N * y + K
		// Score(0, size_) = 0              -> p has no stones, o kept all, worst case for p
		// Score(x, y) > Score(x-1, y)      -> p losing a stone, o keeping, worse for p
		// Score(x, y) < Score(x, y-1)      -> p keeping, o losing a stone, better for p
		// Score(x, y) < Score(x-1, y-1)    -> p losing one, o losing one is good for p (questionable,
		// may depend on how many stones p has, but if we want the game to be alive,
		// we need to keep killing stones)

		//
		// from these inequations came up with
		// M > 0
		// N < 0
		// M < -N
		// K = -N*size_
		//
		// we picked M=2, N=-3
		//
		// for example, for a 5x5 board, the cost table is as follows
		//
		//   ---- number of y stones
		//  /
		// /   -----------------------------------------------
		// 5   |    0       2       4       6       8       10
		// 4   |    3       5       7       9       11      13
		// 3   |    6       8       10      12      14      16
		// 2   |    9       11      13      15      17      19
		// 1   |    12      14      16      18      20      22
		// 0   |    15      17      19      21      23      25
		//     ------------------------------------------------
		//
		//          0       1       2       3       4       5 <- number of x stones
		//
		// Note 1: (5, 0) is the highest score, this is when x has all 5 stones, y has none
		// Note 2: (0, 5) is the lowes score (0), x has no stone, y has all 5
		// Note 3: (5, 2) < (4, 1) - we do favor stone swap (both x and y lose a stone)
		// Note 4: (5, 2) > (4, 2) - if x loses a stone and y stays the same, it's a lower score

		return 0;//Score((2 * x + 3 * (size_ - y)), depth_);
	}

public:
	Score evaluate_board(Player p, Player o) const {
		return compute_score_(stone_count_[p.index()], stone_count_[o.index()]);
	}

	// this modifies the state of the board, hence non-const member function
	void execute_move(size_t x, size_t y, Move const & m, Player const & p,
			Player const & o) {
		Square & current = get_square(x, y);
		assert(&current != &off_board_square_);
		current.set_to_empty(); // set current square to empty

		if (m.move_kind == Move::STEP) {
			Square & c = get_square(x + m.xd, y + m.yd); // making one step to adjacent square
			if (c == o)
				--stone_count_[o.index()]; // just killed an opponent
			c.set_to(p);
		} else if (m.move_kind == Move::SLING) {
			Square & c = get_square(x + 2 * m.xd, y + 2 * m.yd); // we're hopping 2 steps
			if (c == o)
				--stone_count_[o.index()]; // just killed an opponent
			c.set_to(p);
		} else if (m.move_kind == Move::SHOOT) {
			--stone_count_[p.index()]; // we're losing one player when we shoot the rifle
			int opponents_shot_down = 0;
			for (int mv = 1;; ++mv) {
				Square & c = get_square(x + mv * m.xd, y + mv * m.yd);
				if (c.is_off_board())
					break; // we stepped off the board
				if (c == p)
					break; // we stepped on one of our stones
				if (c == o) {
					++opponents_shot_down;
					--stone_count_[o.index()]; // decrement number of opponents
					assert(&c != &off_board_square_);
					c.set_to_empty();
				}
			}
		}
	}

	// makes the next best step for p and returns its score
	Move make_best_move(/*Depth max_depth, */Player const & p, Player const & o) {
		//    if(depth_ >= max_depth || stone_count_[p.index()] == 0){
		//      // we reached the max depth or we have no stones of type p at all
		//      return make_pair(evaluate_board(p, o), Move(0, 0, 0, 0));
		//    }

		if (remaining_time_[o.index()] == 0) {
			return attack(p, o);
		} else {
			return defense(p, o);
		}
		/*
		 Score best_score_so_far_after_player_move = MIN_SCORE;
		 Move best_move_so_far(0, 0);
		 pair<size_t, size_t> candidate_stone(0, 0);
		 Score current_score = evaluate_board(p, o);

		 /////////////// evaluate a move for p at depth+1
		 for (int x = 0; x < size_; ++x) {
		 for (int y = 0; y < size_; ++y) {
		 Square const & c = get_square(x, y);
		 if (c != p)
		 continue;

		 size_t msc = 0;
		 for (All_Moves::iterator m1 = moves.begin(); m1 != moves.end(); ++m1, ++msc) {
		 if (!is_move_legal(x, y, *m1, p, o))
		 continue;

		 Slinga copy_of_board(*this); // copy constructor increments depth

		 copy_of_board.execute_move(x, y, *m1, p, o);

		 //Score worst_score_after_opponent_move = copy_of_board.evaluate_board(p, o);
		 Score worst_score_after_opponent_move = MAX_SCORE;
		 bool opponent_can_make_legal_step = false;

		 //////////////// evalutate a move for o at depth+2
		 for (int x2 = 0; x2 < size_; ++x2) {
		 for (int y2 = 0; y2 < size_; ++y2) {
		 Square const & c2 =
		 copy_of_board.get_square(x2, y2);
		 if (c2 != o)
		 continue;
		 for (All_Moves::iterator m2 = moves.begin(); m2
		 != moves.end(); ++m2) {
		 if (!copy_of_board.is_move_legal(x2, y2, *m2,
		 o, p))
		 continue;

		 opponent_can_make_legal_step = true;

		 Slinga copy_of_board2(copy_of_board); // copy constructor increments depth

		 copy_of_board2.execute_move(x2, y2, *m2, o, p);

		 Score temp_score =
		 copy_of_board2.make_best_move(
		 max_depth, p, o).first;
		 if (temp_score
		 < worst_score_after_opponent_move) {
		 worst_score_after_opponent_move
		 = temp_score;
		 }
		 }
		 }
		 }

		 if (!opponent_can_make_legal_step) {
		 worst_score_after_opponent_move
		 = copy_of_board.evaluate_board(p, o);
		 }

		 if (best_score_so_far_after_player_move
		 < worst_score_after_opponent_move) {
		 //(worst_score_after_opponent_move == best_score_so_far_after_player_move && rand() % 3 == 0)){ // randomize to avoid cycles


		 best_score_so_far_after_player_move
		 = worst_score_after_opponent_move;
		 best_move_so_far = *m1;
		 best_move_so_far.set_square(x, y);
		 candidate_stone = make_pair(x, y);

		 // if going down a path guarantees best_score_so_far_after_player_move at the deepest depth
		 // the current board's score (at current depth)  may be better or worse than that
		 // if it is equal, then we assign current depth to best_score_so_far_after_player_move
		 // because it means that you can achieve the best score at the current depth, you don't need to go deeper
		 // which should be favored over other equal scores with deeper depth
		 if (
		 current_score.get_score_value()
		 == best_score_so_far_after_player_move.get_score_value()) {
		 best_score_so_far_after_player_move.set_depth(
		 depth_); // used for favoring shallower good steps over deeper ones
		 }
		 }
		 }
		 }
		 }

		 execute_move(candidate_stone.first, candidate_stone.second,
		 best_move_so_far, p, o);
		 return make_pair(best_score_so_far_after_player_move, best_move_so_far);
		 */
	}

	friend ostream & operator <<(ostream & o, Slinga const & b);

	Move defense(Player const & p, Player const & o) {
		Attack attack = find_attacks(p, o, true); //true -> stop when attack found
		if (attack.attacker.first == -1 ) { //not attacks possible
			bitset<BOARD_SIZE*BOARD_SIZE> threat;

			Attack threatened;
			if ((threatened = threatened_by_sling_or_shoot(p,o)) != NULL) {
				// our stone at Point threatened. is threatened by a sling or a shoot attack
				// that cannot be attacked by us

			}
		}



	}

	Move attack(Player const & p, Player const & o) {

	}
};

int main(int argc, char * argv[]) {
	srand(time(NULL)); // set the seed based on time
	ifstream infile(argv[1]);
	if (!infile) {
		cout << "could not open file " << argv[1] << " for reading\n";
		return 1;
	}
	char s[100];
	infile.getline(s, 100, '\n');

	// reading
	// OutputFile=move0012-1.rmv
	infile.getline(s, 100, '=');
	infile.getline(s, 100, '\n');

	// opening output file
	ofstream outfile(s);
	if (!outfile) {
		cout << "could not open file " << s << " for writing\n";
		return 1;
	}

	Slinga slinga(infile); // initialize a slinga from infile
	Player const pp('P'); // me, the player
	Player const oo('O'); // opponent

	// if I have all 10 stones, do a depth 2 exploration, otherwise do depth 4
	int depth = slinga.get_stone_count(pp) == 10 ? 2 : 4;

	Move move = slinga.make_best_move(/*depth, */pp, oo); // this is where the action takes place

	/*smp.second*/
	move.print_move_taken(outfile); // writing the file with the move taken
	outfile.close();

	return 0;
}

// That's it
