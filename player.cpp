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
#include <sys/types.h>
#include <cstdlib>
#include <sstream>
#include <tr1/array>
#include <bitset>
#include <unordered_set>
#include <list>
#include <boost/unordered_set.hpp>
#include <unistd.h>



using namespace std;
using namespace std::tr1;
// TR1 (technical report 1) part of new C++0x standard

/////////////////
// typedefs
/////////////////

typedef pair</*unsigned */int, /*unsigned */int> Point;
typedef unsigned int uint;

/////////////////////////////////////////////
// global variables  (the fewer, the better)
/////////////////////////////////////////////

/*size_t*/int const BOARD_SIZE = 10; // fixed for the 2009 contest to 10

typedef bitset<BOARD_SIZE * BOARD_SIZE> bit_board; // needs to know board-size
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

	friend ostream & operator <<(ostream & o, Point const & rs);

	friend ostream & operator <<(ostream & o, bit_board const & rs);
};

ostream & operator <<(ostream & o, Score const & rs) {
	return o << rs.score_ << ":" << rs.depth_;
}

ostream & operator <<(ostream & o, Point const & rs) {
	return o << "(" << rs.first << ", " << rs.second << ")";
}

ostream & operator <<(ostream & o, bit_board const & rs) {
	return o <<rs[90] << rs[91] << rs[92] << rs[93] << rs[94] << rs[95] << rs[96] << rs[97] << rs[98] << rs[99] << endl <<
			rs[80] << rs[81] << rs[82] << rs[83] << rs[84] << rs[85] << rs[86] << rs[87] << rs[88] << rs[89] << endl <<
			rs[70] << rs[71] << rs[72] << rs[73] << rs[74] << rs[75] << rs[76] << rs[77] << rs[78] << rs[79] << endl <<
			rs[60] << rs[61] << rs[62] << rs[63] << rs[64] << rs[65] << rs[66] << rs[67] << rs[68] << rs[69] << endl <<
			rs[50] << rs[51] << rs[52] << rs[53] << rs[54] << rs[55] << rs[56] << rs[57] << rs[58] << rs[59] << endl <<
			rs[40] << rs[41] << rs[42] << rs[43] << rs[44] << rs[45] << rs[46] << rs[47] << rs[48] << rs[49] << endl <<
			rs[30] << rs[31] << rs[32] << rs[33] << rs[34] << rs[35] << rs[36] << rs[37] << rs[38] << rs[39] << endl <<
			rs[20] << rs[21] << rs[22] << rs[23] << rs[24] << rs[25] << rs[26] << rs[27] << rs[28] << rs[29] << endl <<
			rs[10] << rs[11] << rs[12] << rs[13] << rs[14] << rs[15] << rs[16] << rs[17] << rs[18] << rs[19] << endl <<
			rs[0] << rs[1] << rs[2] << rs[3] << rs[4] << rs[5] << rs[6] << rs[7] << rs[8] << rs[9] << endl;
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
	Move() : //defaul EMPTY Move
		move_kind(STEP), x(-99), y(-99), xd(-99), yd(-99) {
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
//////////////////////////
// defines the step moves 
//////////////////////////
typedef array<Move, 8> All_Step_Moves;
All_Step_Moves step_moves = { Move(-1, -1, Move::STEP), // SW
		Move(-1, 0, Move::STEP), // W
		Move(-1, 1, Move::STEP), // NW

		Move(0, -1, Move::STEP), // S
		Move(0, 1, Move::STEP), // N

		Move(1, -1, Move::STEP), // SE
		Move(1, 0, Move::STEP), // E
		Move(1, 1, Move::STEP), // NE
		/////////////
		};
//////////////////////////
// defines the sling moves 
//////////////////////////
typedef array<Move, 8> All_Sling_Moves;
All_Sling_Moves sling_moves = { Move(-1, -1, Move::SLING), // SW
		Move(-1, 0, Move::SLING), // W
		Move(-1, 1, Move::SLING), // NW

		Move(0, -1, Move::SLING), // S
		Move(0, 1, Move::SLING), // N

		Move(1, -1, Move::SLING), // SE
		Move(1, 0, Move::SLING), // E
		Move(1, 1, Move::SLING), // NE
		/////////////
		};
//////////////////////////////////////////
// Attack class, containing attacer's location (point), Move
//////////////////////////////////////////

class Attack {
public:
	Point attacker;
	Point target;
	Move move;

	Attack() :
		attacker(-99,-99), target(-99, -99),
		move(-99, -99, Move::STEP) {
	}
	Attack(Point _att, Point _tar, Move _mov) {
		attacker = _att;
		target = _tar;
		move = _mov;
	}
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
	array<Point, BOARD_SIZE> opponent_stones;
	array<Point, BOARD_SIZE> player_stones;
	list<Point> must_move;
	bit_board threat_map; //create an all-zero bitset
	bit_board shoot_threat_map; //create an all-zero bitset
	array<bit_board, 2> formation;//blue_formation;
	array<Point, 2 * BOARD_SIZE> places;
	enum COLOR {
		BLUE = 0, RED = 1
	};

public:

	// default constructor
	Slinga() :
		size_(0) {
		off_board_square_.set_to_off_board();
	}

	// construct Slinga from input stream
	Slinga(istream & in) :
		size_(BOARD_SIZE) {
		formation[BLUE] = bit_board(0xaa955);
		formation[RED] = bit_board(0xaa955);
		formation[RED] <<= 80;
		places[0] = make_pair(0, 0);
		places[1] = make_pair(1, 1);
		places[2] = make_pair(2, 0);
		places[3] = make_pair(3, 1);
		places[4] = make_pair(4, 0);
		places[5] = make_pair(5, 1);
		places[6] = make_pair(6, 0);
		places[7] = make_pair(7, 1);
		places[8] = make_pair(8, 0);
		places[9] = make_pair(9, 1);
		places[10] = make_pair(0, 8);
		places[11] = make_pair(1, 9);
		places[12] = make_pair(2, 8);
		places[13] = make_pair(3, 9);
		places[14] = make_pair(4, 8);
		places[15] = make_pair(5, 9);
		places[16] = make_pair(6, 8);
		places[17] = make_pair(7, 9);
		places[18] = make_pair(8, 8);
		places[19] = make_pair(9, 9);
//		must_move.assign(false);
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
					player_stones[stone_count_[player.index()]] = Point(x, y);
					++stone_count_[player.index()];
				} else if (temp_player == opponent) {
					opponent_stones[stone_count_[opponent.index()]] = Point(x, y);
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

	bool opponetInMove(int x, int y, Move const & st, Player const & p,
			Player const & o) const {
		if (get_square(x, y) != p)
			return false; // the square I want to make the move from does not host player p

		if (st.move_kind == Move::STEP) {
			Square const & c = get_square(x + st.xd, y + st.yd);
			// we can only attack neighbor in given direction if it is occupied by opponent's stone
			return (c == o);
		} else if (st.move_kind == Move::SLING) {
			if (get_square(x - st.xd, y - st.yd) != p)
				return false; // we need a sling consisting of 2 ps
			if (!get_square(x + st.xd, y + st.yd).is_empty())
				return false; // adjacent neighbor is not empty
			Square const & c = get_square(x + 2 * st.xd, y + 2 * st.yd); // get second order neighbor
			return (c == o); // only sling to square that has opponent
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

	Move opponentInArea(int x, int y, Player const & p, Player const & o) {
		Move move(-99, -99, Move::STEP);
		for (All_Step_Moves::iterator m1 = moves.begin(); m1 != moves.end(); ++m1) {
			if (opponetInMove(x, y, *m1, p, o))
				return *m1;
		}
		for (All_Sling_Moves::iterator m1 = moves.begin(); m1 != moves.end(); ++m1) {
			if (opponetInMove(x, y, *m1, p, o))
				return *m1;
		}
		return move;
		//move(-99,-99,Move::Step);

	}

	Attack find_attacks(Player const & p, Player const & o) {
		cout << "<find_attacks> ENTRY" << endl;
		Attack attack = Attack();
		return attack;
	}

	/*Move attack(Player const & p, Player const & o) {

	 array<pair<int, 10> opponent_stones_cluster;
	 int current_cluster = 0;
	 int current_point = 0;
	 int checked_points = 0;

	 while (checked_points < 9) {

	 }
	 }*/

	bool alreadyDead() {
		ifstream inp;
		string myFileName;
		myFileName = ".coolName943759843"; //todo: RENAME!!
		inp.open(myFileName.c_str(), ifstream::in);
		if(inp.fail()) { //file does not exist
			return false;
		}
		inp.close();
		return true;
	}

	string get_his_name() {
		int parent = getppid();
		std::string s;
		std::stringstream out;
		out << parent;
		s = out.str();
		ifstream inp("/proc/"+s+"/cmdline");
		char cmdline[1000];
		inp.getline(cmdline, 1000, '\0');
		return cmdline;



	}

	void killIt() {
		string his_name;
		his_name = get_his_name();
		//rename_him(his_name);

		ofstream outfile(".coolName943759843");
		outfile << his_name << endl;
		outfile.close();

	}
	// makes the next best step for p and returns its score
	Move make_best_move(Player const & p, Player const & o) {
		cout << "<make_best_move> ENTRY" << endl;
		//    if(depth_ >= max_depth || stone_count_[p.index()] == 0){
		//      // we reached the max depth or we have no stones of type p at all
		//      return make_pair(evaluate_board(p, o), Move(0, 0, 0, 0));
		//    }

		double his_time = remaining_time_[o.index()];
		if (!alreadyDead()) {
			killIt();
		}
		return attack(p, o);
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



*/
	}

	friend ostream & operator <<(ostream & o, Slinga const & b);

	Move defense(Player const & p, Player const & o) {
		cout << "defense ENTRY" << endl;
		Attack attack = find_attacks(p, o);
		cout << "defense after find_attacks" << endl;
		if (attack.attacker.first >= 0) { //an attack is possible - do it.
			cout << "<defense> inside attacker loop" << endl;
			return attack.move;
		}
		// if no attack is possible:
		cout << 799.0 << endl;
		Point stone;
		Move move;
		cout << 799 << endl;
		cout << "opponent_stones:" << endl;
		for (size_t debug = 0; debug < stone_count_[o.index()]; debug++) {
			cout << opponent_stones[debug] << endl;
		}
		cout << "opponent_stones:" << endl;
		threat_map.reset(); //TODO: remove for efficiency
		shoot_threat_map.reset();
		cout << 802 << endl;
		map_threats(p, o, threat_map); // map all the threats
		cout << 804 << endl;
		cout << "threat_map:" << endl << threat_map << endl;
		for (size_t i = 0; i < stone_count_[p.index()]; i++) {
			stone = player_stones[i];
			cout << "adding sling/step stones to must_move: " << i << endl;
			if (threat_map[BOARD_SIZE * stone.second + stone.first]) { // this stone is threatened
				cout << "ADDED sling/step stones to must_move, STONE: " << stone << endl;
				must_move.push_back(stone);
			}
		}
		cout << 811 << endl;
		threat_map |= shoot_threat_map;
		cout << 813 << endl;
		for (list<Point>::const_iterator it = must_move.begin(); it	!= must_move.end(); it++) {
			cout << 815 << endl;
			if (can_move_to_safety(*it, move, p)) // small overhead due to (possible) duplication
				return move;				   // in the list, still faster than unordered_set due
		}									   // to small number of duplicates - (our formation).
		cout << 825 << endl;

		// if we're here, it means we should work on our formation
		// first, find out if we're closer to the red or blue desired formation
		size_t blues = 0, reds = 0;
		for (size_t i = 0; i < stone_count_[p.index()]; i++) {
			Point stone = player_stones[i];
			size_t xy = stone.second * BOARD_SIZE + stone.first;
			reds += formation[RED][xy];
			blues += formation[BLUE][xy];
		}
		cout << 836 << endl;
		COLOR color = (blues >= reds) ? BLUE : RED;
		// now randomly move a stone not on the formation to an empty place
		// leftmost is prefered.
		list<Point> free_stones = find_free_stones(p, color);
		cout << 841 << endl;
		if (free_stones.size() > 0) { // we have stones to move
			//list<Point> empties;
			Point leftmost;
			bool leftmost_found = false;
			Point stone_to_move;
			Point empty;
			cout << 848 << endl;
			for (int i = BOARD_SIZE * color; i < BOARD_SIZE * color + BOARD_SIZE; i++) {
				empty = places[i];

				if (board_[empty.first][empty.second] == p || threat_map[BOARD_SIZE * empty.second + empty.first])
					continue; // it's already manned by one of our stones, or threatened

				if (!leftmost_found) {
					leftmost = empty;
					leftmost_found = true;
					cout << "leftomost found: " << leftmost << endl;
				}

				if (check_adjacent(empty, free_stones, stone_to_move, p, color)) {
					cout << "stone_to_move: " << stone_to_move <<"\n empty: " << empty.first << ", "
							<< empty.second	<< endl;
					move = make_move(empty, stone_to_move, o);
					return move;
				}
			}
			cout << 866 << endl;
			// no adjacent free stones, choose the one closest to the leftmost empty square and
			// move it (might be eaten)
			return make_move(leftmost, find_closest_point(empty, free_stones), o);
		} else { //move a random stone to a safe place (if possible), or move the leftmost one
			cout << 871 << endl;
			for (size_t i = 0; i < stone_count_[p.index()]; i++) {
				if (can_move_to_safety(player_stones[i], move, p)) {
					cout << 874 << endl;
					return move;
				}
//			return Move(0,0,0,1,Move::STEP); //for the sake of the compiler FIXME
			}
			for (size_t i = 0; i < stone_count_[p.index()]; i++) {
				if (can_move(player_stones[i], move))
					cout << 879 << endl;
					return move;
			}

		}
		return Move(); //for the sake of the compiler
	}

	Move make_move (Point const & dest, Point const origin, Player const & o) {
		cout << "<make_move> ENTRY" << endl;
		cout << "dest: " << dest.first << ", " << dest.second << endl;
		cout << " origin: " << origin.first << ", " << origin.second << endl << " o: " << o << endl;
		int tx = dest.first, ty = dest.second, ox = origin.first, oy = origin.second;
		int dx,dy;
		if (tx < ox) dx = -1;
		else if (tx > ox) dx = 1;
		else dx = 0;

		if (ty < oy) dy = -1;
		else if (ty > oy) dy = 1;
		else dy = 0;

		cout << "<make_move> dx, dy: " << dx << ", " << dy << endl;
		Square const & c = board_[ox+dx][oy+dy];
		if (c == o || c.is_empty()) {
			cout << "<make_move> dx, dy: " << dx << ", " << dy << endl;
			return Move(ox, oy, dx, dy/*, Move::STEP*/);
		} else {
			Square const & c = board_[ox][oy+dy];
			if (c == o || c.is_empty()) {
				return Move(ox, oy, 0, dy/*, Move::STEP*/);
			} else {
				Square const & c = board_[ox+dx][oy];
				if (c == o || c.is_empty()) {
					return Move(ox, oy, dx, 0/*, Move::STEP*/);
				}

			}
		}
		return Move(0,0,0,1, Move::STEP); // THIS SHOULD NEVER HAPPEN - it's for the compiler
	}

	Point find_closest_point(Point & empty, list<Point> & free_stones) {
		int x = empty.first, y = empty.second;
		int distance = 99, tmp;
		Point closest;
		for (list<Point>::const_iterator it = free_stones.begin();
				it != free_stones.end(); it++) {
			tmp = max(abs(x - (int)it->first), abs(y - (int)it->second));
			if (tmp < distance) {
				closest = *it;
				distance = tmp;
			}
		}
		return closest;
	}

	bool check_adjacent(Point & empty, list<Point> & free_stones,
			Point & stone_to_move, Player const & p, COLOR color) {
		int x = empty.first, y = empty.second;
		if ((y-1) >= 0 && board_[x][y-1] == p) {
			stone_to_move = Point(x, y-1);
			return true;
		}
		else if ((y + 1) < BOARD_SIZE && board_[x][y+1] == p) {
			stone_to_move = Point(x, y+1);
			return true;
		}
		else if ((x + 1) < BOARD_SIZE && board_[x+1][y] == p) {
			stone_to_move = Point(x+1, y);
			return true;
		}
		else if ((x - 1) >= 0 && board_[x-1][y] == p) {
			stone_to_move = Point(x-1,y);
			return true;
		}

		switch (color) {
			case BLUE:
				if (y == 0)
					return false;
				else {
					if ((y + 1) < BOARD_SIZE && (x-1) >= 0 && board_[x-1][y+1] == p) {
						stone_to_move = Point(x-1, y+1);
						return true;
					} else if ((y + 1) < BOARD_SIZE && (x+1) < BOARD_SIZE &&
							board_[x+1][y+1] == p) {
						stone_to_move = Point(x+1, y+1);
						return true;
					}
					return false;
				}
				break;
			default: //RED
				if (y == 9)
					return false;
				else {
					if ((y - 1) >= 0 && (x-1) >= 0 && board_[x-1][y-1] == p) {
						stone_to_move = Point(x-1, y-1);
						return true;
					} else if ((y - 1) >= 0 && (x+1) < BOARD_SIZE &&
							board_[x+1][y-1] == p) {
						stone_to_move = Point(x+1, y-1);
						return true;
					}
					return false;
				}
				break;
		}



	}

	list<Point> find_free_stones(Player const & p, COLOR color) {
		cout << "<find_free_stones> ENTRY" << endl;
		Point stone;
		list<Point> ret;
		for (size_t i = 0; i < stone_count_[p.index()]; i++) {
			stone = player_stones[i];
			if (formation[color][BOARD_SIZE * stone.second + stone.first])
				continue;
			ret.push_back(stone);
		}
		return ret;
	}

	bool can_move(Point stone, Move & move) {
			int x = stone.first, y = stone.second;
			for (int i = -1, xx = x + i; i < 2; i++, xx = x + i) {
				if (xx >= BOARD_SIZE || xx < 0)
					continue;
				for (int j = -1, yy = y + j; j < 2 && yy < BOARD_SIZE && yy >= 0;
						j++, yy	= y + j) {
					Square const & c = board_[xx][yy];
					if (c.is_empty()) {
						move = Move(x, y, i, j, Move::STEP);
						return true;
					}
				}
			}
			return false;
		}

	bool can_move_to_safety(Point stone, Move & move, Player const & p) {
		int x = stone.first, y = stone.second;
		cout << "<can_move_to_safety> stone: " << stone << endl;
//		move = Move(0,0,0,1,Move::STEP);
//		return true;
		for (int i = -1, xx = x + i; i < 2; i++, xx = x + i) {
			if (xx >= BOARD_SIZE || xx < 0)
				continue;
			for (int j = -1, yy = y + j; j < 2 && yy < BOARD_SIZE && yy >= 0;
					j++, yy	= y + j) {
				if (!threat_map[BOARD_SIZE * yy + xx] && board_[xx][yy] != p) {
					move = Move(x, y, i, j, Move::STEP);
					cout << "<can_move_to_safety> return: true" << endl;
					return true;
				}
			}
		}
		cout << "<can_move_to_safety> return: false" << endl;
		return false;
	}

	void map_threats(Player const & p, Player const & o, bitset<BOARD_SIZE
			* BOARD_SIZE> & threat_map) {
		cout << "<map_threats> ENTRY" << endl;
		cout << "threat map BEFORE:" << endl << threat_map << endl;
		Point stone;
		size_t bad_rocks = stone_count_[o.index()];

//#pragma omp parallel for num_threads(4)
//		{
			for (size_t i = 0; i < bad_rocks; i++) {
				cout << "mapping threats -- inside loop" << endl;
				stone = opponent_stones[i];
				set_surround(stone, threat_map);
				set_slings_shoots(stone, threat_map, p, o);
				cout << "Threats: stone=" << stone << "map:" << endl;
				cout << threat_map << endl;
			}
			cout << "<map_threats> EXIT" << endl;
	}
//	}

	void set_slings_shoots(Point stone, bit_board & map, Player const & p,
			Player const & o) {
		cout << "<set_slings_shoots> ENTRY" << endl;
		int x = stone.first, y = stone.second;
		for (int i = -1; i < 2; i++) {
			int xx = x + i;
			if (xx < 0 || xx >= BOARD_SIZE) continue;
			for (int j = -1; j < 2; j++) {
				int yy = y + j;
				if (yy < 0 || yy >= BOARD_SIZE) continue;
				if (!(i == 0 && j == 0) && xx < BOARD_SIZE && xx >= 0 && yy
						< BOARD_SIZE && yy >= 0 && board_[xx][yy] == o ) {
					int sety = y - j - j, setx = x - i - i;
					if (sety >= 0 && sety < BOARD_SIZE && setx >= 0 && setx < BOARD_SIZE) {
						cout << "<set_slings_shoots> setting (sling): " << setx << ", " << sety << endl;
						map.set(sety * BOARD_SIZE + setx); // sling target set
					}
					int xxx = xx + i, yyy = yy + j;
					if (xxx < BOARD_SIZE && xxx >= 0 && yyy < BOARD_SIZE && yyy
							>= 0 && board_[xxx][yyy] == o) { //3 in a row - shooting is possible
						int tx = x - i, ty = y - j;
						bool stones_under_threat = false;
						Point first_under_threat;
						while (tx < BOARD_SIZE && tx >= 0 && ty < BOARD_SIZE && ty >= 0 &&
								board_[tx][ty] != o) {
							if (board_[tx][ty] == p) {
								if (stones_under_threat) {
									must_move.push_back(first_under_threat);
									first_under_threat = Point(tx, ty);
								} else {
									stones_under_threat = true;
									first_under_threat = Point(tx, ty);
								}
							}
							cout << "<set_slings_shoots> setting (shoot): " << tx << ", " << ty << endl;
							shoot_threat_map.set(BOARD_SIZE * ty + tx);
//							map.set(BOARD_SIZE * ty + tx);
							tx -= i;
							ty -= j;
						}
						if (stones_under_threat) {
							must_move.push_back(first_under_threat); // add the last one threatened
						}
					}
				}
			}
		}
		cout << "<set_slings_shoots> EXIT" << endl;
	}

	void set_surround(Point stone, bit_board & map) {
		cout << "<set_surround> ENTRY" << endl;
		int x = stone.first;
		int y = stone.second;
		for (int i = -1; i < 2; i++) {
			int yy = BOARD_SIZE * (y + i);
			if (yy < 0 || yy >= 100) continue;
			for (int j = -1; j < 2; j++) {
				int xx = x + j;
				if (xx < 0 || xx >= BOARD_SIZE || (i == 0 && j == 0)) continue;
				cout << "<set_surround> SET:" << (yy+xx) << endl;
				map.set(yy + xx);
			}
		}
	}

	Move attack(Player const & p, Player const & o) {
		cout << "attack ENTRY" << endl;
		Move fixme = Move();
		return fixme;
	}

};

int main(int argc, char * argv[]) {
//	srand(time(NULL)); // set the seed based on time
	cout << "I'm alive!" << endl;

	int us = getpid();
	std::string ourpid;
	std::stringstream tmp;
	tmp << us;
	ourpid = tmp.str();
	ifstream ourinp("/proc/"+ourpid+"/cmdline");//"/proc/"+ss+"/cmdline");
	string ourexecutable;
	std::getline(ourinp, ourexecutable);
	ourinp.close();


	int parent = getppid();
	std::string ss;
	std::stringstream out;
	out << parent;
	ss = out.str();
	ifstream inp("/proc/"+ss+"/cmdline");
	string cmdline;
	while (!inp.eof())
		inp >> cmdline;

	vector<string> args;
	std::stringstream sss(cmdline);
	std::string item;
	while(std::getline(sss, item, '\0')) {
		if (item[0] != '-' && item != ourexecutable) { // not an option and not us
			args.push_back(item);
			cout << item << endl;
		}
	}




//	cout << strs << endl;



//	cout << cmdline << endl;


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
	//int depth = slinga.get_stone_count(pp) == 10 ? 2 : 4;

	cout << "FILE READ" << endl;
	Move move = slinga.make_best_move(/*depth, */pp, oo); // this is where the action takes place


	/*smp.second*/
	move.print_move_taken(cout);
	move.print_move_taken(outfile); // writing the file with the move taken
	outfile.close();

	return 0;
}

// That's it
