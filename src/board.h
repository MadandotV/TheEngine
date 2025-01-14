#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

#include <string>
#include "basics.h"

namespace Bitboards {
    void init();
    const std::string print(Bitboard bb);
}

constexpr Bitboard AllSqrs = ~Bitboard(0);
constexpr Bitboard BlackSqrs = 0xAA55AA55AA55AA55ULL;
constexpr Bitboard WhiteSqrs = ~BlackSqrs;

constexpr Bitboard FileA_BB = 0x0101010101010101ULL;
constexpr Bitboard FileB_BB = FileA_BB << 1;
constexpr Bitboard FileC_BB = FileB_BB << 1;
constexpr Bitboard FileD_BB = FileC_BB << 1;
constexpr Bitboard FileE_BB = FileD_BB << 1;
constexpr Bitboard FileF_BB = FileE_BB << 1;
constexpr Bitboard FileG_BB = FileF_BB << 1;
constexpr Bitboard FileH_BB = FileG_BB << 1;

constexpr Bitboard Rank1_BB = 0xFF;
constexpr Bitboard Rank2_BB = Rank1_BB << 8;
constexpr Bitboard Rank3_BB = Rank2_BB << 8;
constexpr Bitboard Rank4_BB = Rank3_BB << 8;
constexpr Bitboard Rank5_BB = Rank4_BB << 8;
constexpr Bitboard Rank6_BB = Rank5_BB << 8;
constexpr Bitboard Rank7_BB = Rank6_BB << 8;
constexpr Bitboard Rank8_BB = Rank7_BB << 8;

constexpr Bitboard KingSide = FileE_BB | FileF_BB | FileG_BB | FileH_BB;
constexpr Bitboard QueenSide = FileA_BB | FileB_BB | FileC_BB | FileD_BB;
constexpr Bitboard CenterFile = FileC_BB | FileD_BB | FileE_BB | FileF_BB;
constexpr Bitboard Center = (FileD_BB | FileE_BB) & (Rank4_BB | Rank5_BB);

extern Bitboard Square_BB[SQUARE_NB]; // Returns a bitboard with just the index of that particular square set (eg. Square_BB[0]= 00000...1 )
extern Bitboard LineOfAttack_BB[SQUARE_NB][SQUARE_NB]; // Returns a bitboard with the bits between the two squares set to 1
extern Bitboard PosAttacks[PIECE_TYPE_SIZE][SQUARE_NB]; // Returns all the squares a piece can attack from a particular given square
extern Bitboard PawnAttacks[COLOR_NB][SQUARE_NB]; // Returns all the the squares a particular colored pawn can attack from a particular given square

extern uint8_t SqrDist[SQUARE_NB][SQUARE_NB]; // Returns manhattan distance between two squares
extern uint8_t PopCount[1 << 16];

struct MagicBit {
    Bitboard mask;
    Bitboard magic;
    Bitboard * attacks;
    unsigned shift;

    unsigned index(Bitboard occupied) const {
        if (HasPext){
            return unsigned(pext(occupied,mask));
        }
        if(Is64Bit){
            return unsigned(((occupied & mask) * magic) >> shift);
        }
        return 0;
    }
};

extern MagicBit RookMagic[SQUARE_NB];
extern MagicBit BishopMagic[SQUARE_NB];

inline Bitboard square_bb(Square s){
    if(inside(s)){
        return Square_BB[s];
    }
    return 0;
}

/// Operator overloading for Square types with Bitboard types

inline Bitboard operator&(Bitboard bb, Square s) { return bb & square_bb(s); }
inline Bitboard operator|(Bitboard bb, Square s) { return bb | square_bb(s); }
inline Bitboard operator^(Bitboard bb, Square s) { return bb ^ square_bb(s); }
inline Bitboard& operator&=(Bitboard& bb, Square s) { return bb &= square_bb(s); }
inline Bitboard& operator|=(Bitboard& bb, Square s) { return bb |= square_bb(s); }
inline Bitboard& operator^=(Bitboard& bb, Square s) { return bb ^= square_bb(s); }

inline Bitboard operator&(Square s, Bitboard bb) { return bb & s; }
inline Bitboard operator|(Square s, Bitboard bb) { return bb | s; }
inline Bitboard operator^(Square s, Bitboard bb) { return bb ^ s; }

inline Bitboard operator|(Square s1, Square s2) { return s1 | square_bb(s2); }

constexpr bool opp_color(Square s1, Square s2) {
  return (static_cast<int>(s1) + static_cast<int>(return_rank(s1)) + static_cast<int>(s2) + static_cast<int>(return_rank(s2))) & 1;
}

constexpr Bitboard rank_bb(Rank rank) { return Rank1_BB << (8 * rank); }
constexpr Bitboard rank_bb(Square s) { return rank_bb(return_rank(s)); }

constexpr Bitboard file_bb(File file) { return FileA_BB << (file); }
constexpr Bitboard file_bb(Square s) { return file_bb(return_file(s)); }

template <Direction D> constexpr Bitboard shift_bb(Bitboard bb) {
  if (D == NORTH) {
    return bb << 8;
  } else if (D == SOUTH) {
    return bb >> 8;
  } else if (D == EAST) {
    return (bb & ~FileH_BB) << 1;
  } else if (D == WEST) {
    return (bb & ~FileA_BB) >> 1;
  } else if (D == NORTH + NORTH) {
    return (bb << 16);
  } else if (D == SOUTH + SOUTH) {
    return (bb >> 16);
  } else if (D == NORTH_EAST) {
    return (bb & ~FileH_BB) << 9;
  } else if (D == NORTH_WEST) {
    return (bb & ~FileA_BB) << 7;
  } else if (D == SOUTH_EAST) {
    return (bb & ~FileH_BB) >> 7;
  } else if (D == SOUTH_WEST) {
    return (bb & ~FileA_BB) >> 9;
  }
  return 0;
}

template <Color c> constexpr Bitboard pawn_attack(Bitboard bb) {
  if (c == WHITE) {
    return shift_bb<NORTH_WEST>(bb) | shift_bb<NORTH_EAST>(bb);
  } else if (c == BLACK) {
    return shift_bb<SOUTH_WEST>(bb) | shift_bb<SOUTH_EAST>(bb);
  }
  return 0;
}

inline Bitboard pawn_attack(Color c, Square s) { return PawnAttacks[c][s]; }

template <Color c> constexpr Bitboard double_pawn_attack(Bitboard bb) {
  if (c == WHITE) {
    return shift_bb<NORTH_WEST>(bb) & shift_bb<NORTH_EAST>(bb);
  } else if (c == BLACK) {
    return shift_bb<SOUTH_WEST>(bb) & shift_bb<SOUTH_EAST>(bb);
  }
  return 0;
}

constexpr Bitboard adj_files(Square s) {
  return (shift_bb<EAST>(file_bb(s)) | shift_bb<WEST>(file_bb(s)));
}

constexpr Bitboard adj_ranks(Square s) {
  return (shift_bb<NORTH>(rank_bb(s)) | shift_bb<SOUTH>(rank_bb(s)));
}

inline Bitboard lineOfatt(Square s1, Square s2) {
    return LineOfAttack_BB[s1][s2];
}

inline Bitboard between(Square s1, Square s2) {
    Bitboard bb = lineOfatt(s1,s2) & (AllSqrs<<s1) ^ (AllSqrs<<s2);
    return bb & (bb-1);
}

template <typename T1 = Square> inline int Distance(Square s1, Square s2);
template <> inline int Distance<Rank>(Square s1, Square s2) {
  return std::abs(return_rank(s1) - return_rank(s2));
}
template <> inline int Distance<File>(Square s1, Square s2) {
  return std::abs(return_file(s1) - return_file(s2));
}
template <> inline int Distance<Square>(Square s1, Square s2) {
  return SqrDist[s1][s2];
}

template <PieceType piecetype> 
inline Bitboard attacks_bb(Square s){
    if(piecetype!=PAWN){
        return PosAttacks[piecetype][s];
    }
    return 0;
}

template <PieceType piecetype>
inline Bitboard attacks_bb(Square s, Bitboard occupied) {
    if (piecetype !=PAWN){
        switch(piecetype){
            case ROOK:
                return RookMagic[s].attacks[RookMagic[s].index(occupied)];
            case BISHOP:
                return BishopMagic[s].attacks[BishopMagic[s].index(occupied)];
            case QUEEN:
                return RookMagic[s].attacks[RookMagic[s].index(occupied)] | BishopMagic[s].attacks[BishopMagic[s].index(occupied)];
            default:
                return PosAttacks[piecetype][s];
        }
    }
}

inline Bitboard attacks_bb(PieceType piecetype, Square s, Bitboard occupied) {
    switch(piecetype){
            case ROOK:
                return RookMagic[s].attacks[RookMagic[s].index(occupied)];
            case BISHOP:
                return BishopMagic[s].attacks[BishopMagic[s].index(occupied)];
            case QUEEN:
                return RookMagic[s].attacks[RookMagic[s].index(occupied)] | BishopMagic[s].attacks[BishopMagic[s].index(occupied)];
            default:
                return PosAttacks[piecetype][s];
        }
}

inline int popCount(Bitboard bb){
    #ifndef USE_POPCNT
        union { Bitboard bitboard; uint16_t u[4]; }
        v= {bb};
        return PopCount[v.u[0]] + PopCount[v.u[1]] + PopCount[v.u[2]] + PopCount[v.u[3]];
    #elif defined(_MSC_VER) || defined(__INTEL_COMPILER)
        return (int)_mm_popcount_u64(bb);
    #else 
        return __builtin_popcountll(bb);
    #endif
}

#endif // BOARD_H_INCLUDED