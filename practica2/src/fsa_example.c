#include <stdio.h>
#include <stdlib.h>
/* Create a random int i,  min_value <= i <= max_value */
int DiscreteRand(int min_value, int max_value) {
  float v = (float) rand()/ (float) RAND_MAX;
  return (int) (min_value + v * (max_value-min_value+1));
}

/* Define possible states */
typedef enum {ST_INI, ST_COIN, ST_DICE, ST_END} STATE;

int main(int argc, const char *argv[]) {
  STATE state = ST_INI;
  int count=0, n_heads, n_six;
  while (state != ST_END) {
    ++count;
    switch (state) {
    case ST_INI:
      count =  n_heads = n_six = 0;
      state = ST_COIN;
      break;
    case ST_COIN: 
      if (DiscreteRand(0,1) == 0)
        if (++n_heads == 3) state = ST_DICE;
      break;
    case ST_DICE:
      if (DiscreteRand(1,6) == 6)
        if (++n_six == 3) state = ST_END;
      break;
    case ST_END:
      break;
    }
    fprintf(stdout, "State: %d; cnt=%d, nHeads=%d, nSixs=%d\n", 
	    state, count, n_heads, n_six);
  }
  return 0;
}
