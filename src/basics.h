#ifndef BASICS_H_INCLUDED // terminates inclusion if already included to remover
                          // redundant running
#define BASICS_H_INCLUDED // defines the include if not already included

#include <algorithm> //gives access to many algos like sort and stuff
#include <cctype>    //gives access to isalpha() and similar functions
#include <cstdint>   //gives access to fixed width of int like int8_t or uint8_t
#include <cstdlib>   //gives access to many essential functions

#if defined(_WIN64) && defined(_MSC_VER) // check whether windows 64-bit
#include <intrin.h>                      //header for _BitScanForward64()
#define IS_64BIT
#endif

#if defined(USE_POPCNT) && (defined(__INTEL_COMPILER) || defined(_MSC_VER))
#include <nmmintrin.h> // Intel and Microsoft header for _mm_popcnt_u64()
#endif

#if !defined(NO_PREFETCH) && (defined(__INTEL_COMPILER) || defined(_MSC_VER))
#include <xmmintrin.h> // Intel and Microsoft header for _mm_prefetch()
#endif

#if defined(USE_PEXT)
#  include <immintrin.h> // header for _pext_u64() intrinsic
#  define pext(b, m) _pext_u64(b, m)
#else
#  define pext(b, m) 0
#endif

#ifdef USE_POPCNT
constexpr bool HasPopCnt = true;
#else
constexpr bool HasPopCnt = false;
#endif

#ifdef USE_PEXT
constexpr bool HasPext = true;
#else
constexpr bool HasPext = false;
#endif


#ifdef IS_64BIT
constexpr bool Is64Bit = true;
#else
constexpr bool Is64Bit = false;
#endif

typedef uint64_t Bitboard;

/// We will be using a 16-bit to store a particular move 
/// bits 0-5 will store the destination square (these 6 bits can define from 0-63)
/// bits 6-11 will store the start square (these 6 bits can define from 0-63)
/// bits 12-13 promotion type of pieces in order of knight(0),bishop(1),rook(2),queen(3)
/// bits 14-15 flag for special moves like promotion(1), en passant(2) , castling(3)
/// Some exception cases for MOVE_NONE and MOVE_NULL when computer generates
/// start square and destination square to be same

enum Move : int { MOVE_NONE, MOVE_NULL = 65 };

enum MoveType {
  NORMAL,
  PROMOTION = 1 << 14,
  EN_PASSANT = 2 << 14,
  CASTLING = 3 << 14
};

enum Color { WHITE, BLACK, NO_COLOR,COLOR_NB=2 };

// Castling Rights is of 4-bits and checks whether there is a possible castling opportunity
enum CastlingRights {
  NO_Castling,
  WHITE_OO = 1,
  WHITE_OOO = 2,
  BLACK_OO = 4,
  BLACK_OOO = 8,

  KING_SIDE = WHITE_OO | BLACK_OO,
  QUEEN_SIDE = WHITE_OOO | BLACK_OOO,
  WHITE_CASTLING = WHITE_OO | WHITE_OOO,
  BLACK_CASTLING = BLACK_OO | BLACK_OOO,
  ALL_CASTLING = WHITE_OO | WHITE_OOO | BLACK_OO | BLACK_OOO,

  CASTLING_RIGHTS_SIZE = 16
};

enum PieceType {
    NO_PIECE_TYPE,PAWN,KNIGHT,BISHOP,ROOK,QUEEN,KING,
    ALL_PIECES=0,
    PIECE_TYPE_SIZE=8
};

enum Piece{
    NO_PIECE,
    WPAWN=PAWN, WKNIGHT, WBISHOP, WROOK, WQUEEN, WKING,
    BPAWN=PAWN + 8, BKNIGHT, BBISHOP, BROOK, BQUEEN, BKING,
    PIECE_SIZE=16
};

enum Square : int {
  SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
  SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
  SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
  SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
  SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
  SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
  SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
  SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
  SQ_NONE,

  SQUARE_ZERO = 0,
  SQUARE_NB   = 64
};

enum Direction : int {
    NORTH =8,
    EAST = 1,
    SOUTH = -8,
    WEST = -1,
    NORTH_EAST = NORTH + EAST,
    NORTH_WEST = NORTH + WEST,
    SOUTH_EAST = SOUTH + EAST,
    SOUTH_WEST = SOUTH + WEST
};

enum File : int {
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H,
    FILE_NB = 8
};

enum Rank : int {
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8,RANK_NB
};

#define ENABLE_BASE_OPERATORS_ON(T)                                            \
  constexpr T operator+(T d1, int d2) { return T(int(d1) + d2); }              \
  constexpr T operator-(T d1, int d2) { return T(int(d1) - d2); }              \
  constexpr T operator-(T d) { return T(-int(d)); }                            \
  inline T &operator+=(T &d1, int d2) { return d1 = d1 + d2; }                 \
  inline T &operator-=(T &d1, int d2) { return d1 = d1 - d2; }

#define ENABLE_INCR_OPERATORS_ON(T)                                            \
  inline T &operator++(T &d) { return d = T(int(d) + 1); }                     \
  inline T &operator--(T &d) { return d = T(int(d) - 1); }

#define ENABLE_FULL_OPERATORS_ON(T)                                            \
  ENABLE_BASE_OPERATORS_ON(T)                                                  \
  ENABLE_INCR_OPERATORS_ON(T)                                                  \
  constexpr T operator*(int i, T d) { return T(i * int(d)); }                  \
  constexpr T operator*(T d, int i) { return T(int(d) * i); }                  \
  constexpr T operator/(T d, int i) { return T(int(d) / i); }                  \
  constexpr int operator/(T d1, T d2) { return int(d1) / int(d2); }            \
  inline T &operator*=(T &d, int i) { return d = T(int(d) * i); }              \
  inline T &operator/=(T &d, int i) { return d = T(int(d) / i); }

ENABLE_FULL_OPERATORS_ON(Square)
ENABLE_FULL_OPERATORS_ON(PieceType)
ENABLE_FULL_OPERATORS_ON(Piece)
ENABLE_FULL_OPERATORS_ON(Direction)
ENABLE_FULL_OPERATORS_ON(File)
ENABLE_FULL_OPERATORS_ON(Rank)

#undef ENABLE_FULL_OPERATORS_ON
#undef ENABLE_INCR_OPERATORS_ON
#undef ENABLE_BASE_OPERATORS_ON

constexpr Square operator+(Square s,Direction d) { return Square(int(s) + int(d)); }
constexpr Square operator-(Square s, Direction d) { return Square(int(s) - int(d)); }
inline Square operator+=(Square s,Direction d) { return s=s+d; }  
inline Square operator-=(Square s,Direction d) { return s=s-d; }

constexpr Color operator~(Color c){ return Color(c ^ BLACK);}

constexpr Square flip_rank(Square s) { return Square(s ^ SQ_A8); }
constexpr Square flip_file(Square s) { return Square(s ^ SQ_H1); }
constexpr Square make_square(File file, Rank rank) {
  return Square((rank << 3) + file);
}

constexpr CastlingRights operator&(Color color, CastlingRights castlingRights) {
  return CastlingRights((color == WHITE ? WHITE_CASTLING : BLACK_CASTLING) ^
                        castlingRights);
}
constexpr Piece make_piece(Color color, PieceType piecetype) {
  return Piece((color << 3) + piecetype);
}
constexpr PieceType type(Piece piece) { return PieceType(piece & 7); }
constexpr Color color(Piece piece) { return Color(piece >> 3); }

constexpr bool inside(Square s) { return s >= SQ_A1 && s <= SQ_H8; }

constexpr File return_file(Square s) { return File(s & 7); }
constexpr Rank return_rank(Square s) { return Rank(s >> 3); }

constexpr Square color_square(Square s, Color color) {
  return Square(s ^ (color * 56));
}
constexpr Rank color_rank(Rank rank, Color color) {
  return Rank(rank ^ (color * 7));
}
constexpr Rank color_rank(Square s, Color color) {
  return color_rank(return_rank(s), color);
}

constexpr Direction pawn_push(Color color) {
  return color == WHITE ? NORTH : SOUTH;
}

constexpr Square from_sq(Move move) { return Square((move >> 6) & 0x3f); }
constexpr Square to_sq(Move move) { return Square(move & 0x3f); }
constexpr int from_to(Move move) { return move & 0xfff; }
constexpr MoveType return_type(Move move) { return MoveType((move >> 14) & 3); }
constexpr PieceType return_promotion_type(Move move) {
  return PieceType(((move >> 12) & 3) + KNIGHT);
}
constexpr Move make_move(Square from, Square to) {
  return Move((from << 6) + to);
}
constexpr Move reverse_move(Square from, Square to) {
  return Move((to << 6) + from);
}
constexpr bool move_ok(Move move) { return from_sq(move) != to_sq(move); }

#endif
