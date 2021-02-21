// Chess engine based on: https://create.arduino.cc/projecthub/rom3/arduino-uno-micromax-chess-030d7c

/***************************************************************************/
/* Ported chess program to Arduino UNO by rom3                             */
/* Based on:         micro-Max,       version 4.8                          */
/* A chess program smaller than 2KB (of non-blank source), by H.G. Muller  */
/* Chessuino by Diego Cueva                                                */
/* Atmel ATMega644 and AVR GCC, by Andre Adrian                            */
/* May 12 2019        Germany                                              */
/***************************************************************************/

#include <stdint.h>
#include "micromax.h"

#define W while
#define M 0x88
#define S 128
#define I 8000

#define MYRAND_MAX 65535     /* 16bit pseudo random generator */
long  N, T;                  /* N=evaluated positions+S, T=recursion limit */
int Q, O, K, R, k=16;      /* k=moving side */
char *p, c[5], Z;            /* p=pointer to c, c=user input, computer output, Z=recursion counter */

char L,
w[]={0,2,2,7,-1,8,12,23},                             /* relative piece values    */
o[]={-16,-15,-17,0,1,16,0,1,16,15,17,0,14,18,31,33,0, /* step-vector lists */
     7,-1,11,6,8,3,6,                                 /* 1st dir. in o[] per piece*/
     6,3,5,7,4,5,3,6};                                /* initial piece setup      */
/* board is left part, center-pts table is right part, and dummy */

char chess_board[]={
  22, 19, 21, 23, 20, 21, 19, 22, 28, 21, 16, 13, 12, 13, 16, 21,
  18, 18, 18, 18, 18, 18, 18, 18, 22, 15, 10,  7,  6,  7, 10, 15,
   0,  0,  0,  0,  0,  0,  0,  0, 18, 11,  6,  3,  2,  3,  6, 11,
   0,  0,  0,  0,  0,  0,  0,  0, 16,  9,  4,  1,  0,  1,  4,  9,
   0,  0,  0,  0,  0,  0,  0,  0, 16,  9,  4,  1,  0,  1,  4,  9,
   0,  0,  0,  0,  0,  0,  0,  0, 18, 11,  6,  3,  2,  3,  6, 11,
   9,  9,  9,  9,  9,  9,  9,  9, 22, 15, 10,  7,  6,  7, 10, 15,
  14, 11, 13, 15, 12, 13, 11, 14, 28, 21, 16, 13, 12, 13, 16, 21, 0
};

unsigned int seed=0;
uint32_t  byteBoard[8];

char sym[17] = {".?pnkbrq?P?NKBRQ"};
int mn=1;
char lastH[5], lastM[5];
unsigned int ledv=1;

void micromax_input(char *input)
{
	c[0] = input[0];
	c[1] = input[1];
	c[2] = input[2];
	c[3] = input[3];
}

void micromax_init(void)
{
  lastH[0] = 0;
}

void micromax_update(void)
{
  int r;

  K = *c - 16 * c[1] + 799, L = c[2] - 16 * c[3] + 799; /* parse entered move */
  N = 0;
  T = 0x3F;                                 /* T=Computer Play strength */

  r = D(-I, I, Q, O, 1, 3);                 /* Check & do the human movement */
  if ( !(r > -I + 1) ) {
    //gameOver();
  }
  if (k == 0x10) {                          /* The flag turn must change to 0x08 */
    //noValidMove();
    return;
  }

  strcpy(lastH, c);                         /* Valid human movement */

  mn++;                                     /* Next move */

  K = I;
  N = 0;
  T = 0x3F;                                 /* T=Computer Play strength */
  r = D(-I, I, Q, O, 1, 3);                 /* Think & do*/
  if ( !(r > -I + 1) ) {
    //gameOver();
  }

  strcpy(lastM, c);                         /* Valid COMPUTER movement */
  r = D(-I, I, Q, O, 1, 3);
  if ( !(r > -I + 1) ) {
    //gameOver();
  }
}

unsigned int myrand(void) {
  unsigned int r = (unsigned int)(seed % MYRAND_MAX);
  return r = ((r << 11) + (r << 7) + r) >> 1;
}

/* recursive minimax search */
/* (q,l)=window, e=current eval. score, */
/* E=e.p. sqr.z=prev.dest, n=depth; return score */
int D(int q, int l, int e, int E, int z, int n) {
  int m, v, i, P, V, s;
  unsigned char t, p, u, x, y, X, Y, H, B, j, d, h, F, G, C;
  signed char r;
  if (++Z > 30) {                                   /* stack underrun check */
    --Z; return e;
  }
  q--;                                          /* adj. window: delay bonus */
  k ^= 24;                                      /* change sides             */
  d = Y = 0;                                    /* start iter. from scratch */
  X = myrand() & ~M;                            /* start at random field    */
  W(d++ < n || d < 3 ||                         /* iterative deepening loop */
    z & K == I && (N < T & d < 98 ||            /* root: deepen upto time   */
                   (K = X, L = Y & ~M, d = 3)))                /* time's up: go do best    */
  { x = B = X;                                   /* start scan at prev. best */
    h = Y & S;                                   /* request try noncastl. 1st*/
    P = d < 3 ? I : D(-l, 1 - l, -e, S, 0, d - 3); /* Search null move         */
    m = -P < l | R > 35 ? d > 2 ? -I : e : -P;   /* Prune or stand-pat       */
    ++N;                                         /* node count (for timing)  */
    do {
      u = chess_board[x];                                   /* scan board looking for   */
      if (u & k) {                                /*  own piece (inefficient!)*/
        r = p = u & 7;                             /* p = piece type (set r>0) */
        j = o[p + 16];                             /* first step vector f.piece*/
        W(r = p > 2 & r < 0 ? -r : -o[++j])        /* loop over directions o[] */
        { A:                                        /* resume normal after best */
          y = x; F = G = S;                         /* (x,y)=move, (F,G)=castl.R*/
          do {                                      /* y traverses ray, or:     */
            H = y = h ? Y ^ h : y + r;               /* sneak in prev. best move */
            if (y & M)break;                         /* board edge hit           */
            m = E - S & chess_board[E] && y - E < 2 & E - y < 2 ? I : m; /* bad castling             */
            if (p < 3 & y == E)H ^= 16;              /* shift capt.sqr. H if e.p.*/
            t = chess_board[H]; if (t & k | p < 3 & !(y - x & 7) - !t)break; /* capt. own, bad pawn mode */
            i = 37 * w[t & 7] + (t & 192);           /* value of capt. piece t   */
            m = i < 0 ? I : m;                       /* K capture                */
            if (m >= l & d > 1)goto C;               /* abort on fail high       */
            v = d - 1 ? e : i - p;                   /* MVV/LVA scoring          */
            if (d - !t > 1)                          /* remaining depth          */
            { v = p < 6 ? chess_board[x + 8] - chess_board[y + 8] : 0;    /* center positional pts.   */
              chess_board[G] = chess_board[H] = chess_board[x] = 0; chess_board[y] = u | 32;  /* do move, set non-virgin  */
              if (!(G & M))chess_board[F] = k + 6, v += 50;     /* castling: put R & score  */
              v -= p - 4 | R > 29 ? 0 : 20;           /* penalize mid-game K move */
              if (p < 3)                              /* pawns:                   */
              { v -= 9 * ((x - 2 & M || chess_board[x - 2] - u) + /* structure, undefended    */
                          (x + 2 & M || chess_board[x + 2] - u) - 1  /*        squares plus bias */
                          + (chess_board[x ^ 16] == k + 36))          /* kling to non-virgin King */
                     - (R >> 2);                       /* end-game Pawn-push bonus */
                V = y + r + 1 & S ? 647 - p : 2 * (u & y + 16 & 32); /* promotion or 6/7th bonus */
                chess_board[y] += V; i += V;                     /* change piece, add score  */
              }
              v += e + i; V = m > q ? m : q;          /* new eval and alpha       */
              C = d - 1 - (d > 5 & p > 2 & !t & !h);
              C = R > 29 | d < 3 | P - I ? C : d;     /* extend 1 ply if in check */
              do
                s = C > 2 | v > V ? -D(-l, -V, -v,     /* recursive eval. of reply */
                                       F, 0, C) : v;    /* or fail low if futile    */
              W(s > q&++C < d); v = s;
              if (z && K - I && v + I && x == K & y == L) /* move pending & in root:  */
              { Q = -e - i; O = F;                     /*   exit if legal & found  */
                R += i >> 7; --Z; return l;            /* captured non-P material  */
              }
              chess_board[G] = k + 6; chess_board[F] = chess_board[y] = 0; chess_board[x] = u; chess_board[H] = t; /* undo move,G can be dummy */
            }
            if (v > m)                               /* new best, update max,best*/
              m = v, X = x, Y = y | S & F;            /* mark double move with S  */
            if (h) {
              h = 0;  /* redo after doing old best*/
              goto A;
            }
            if (x + r - y | u & 32 |                 /* not 1st step,moved before*/
                p > 2 & (p - 4 | j - 7 ||             /* no P & no lateral K move,*/
                         chess_board[G = x + 3 ^ r >> 1 & 7] - k - 6     /* no virgin R in corner G, */
                         || chess_board[G ^ 1] | chess_board[G ^ 2])               /* no 2 empty sq. next to R */
               )t += p < 5;                           /* fake capt. for nonsliding*/
            else F = y;                              /* enable e.p.              */
          } W(!t);                                  /* if not capt. continue ray*/
        }
      }
    } W((x = x + 9 & ~M) - B);                 /* next sqr. of board, wrap */
C: if (m > I - M | m < M - I)d = 98;           /* mate holds to any depth  */
    m = m + I | P == I ? m : 0;                  /* best loses K: (stale)mate*/
    if (z && d > 2)
    { *c = 'a' + (X & 7); c[1] = '8' - (X >> 4); c[2] = 'a' + (Y & 7); c[3] = '8' - (Y >> 4 & 7); c[4] = 0;
    }
  }                                             /*    encoded in X S,8 bits */
  k ^= 24;                                      /* change sides back        */
  --Z; return m += m < e;                       /* delayed-loss bonus       */
}
