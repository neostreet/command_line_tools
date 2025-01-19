#include <stdio.h>

static char coins_str[] = "123456";
#define NUM_COINS (sizeof (coins_str) - 1)

void print_coins(char *coins);
void do_transform(char *coins);

int main(int argc,char **argv)
{
  int n;
  int num_transforms;
  char coins[NUM_COINS];

  if (argc != 2) {
    printf("usage: coins num_transforms\n");
    return 1;
  }

  sscanf(argv[1],"%d",&num_transforms);

  if (num_transforms < 1) {
    printf("num_transforms must be >= 1\n");
    return 2;
  }

  for (n = 0; n < NUM_COINS; n++)
    coins[n] = coins_str[n];

  print_coins(coins);

  for (n = 0; n < num_transforms; n++) {
    do_transform(coins);

    print_coins(coins);
  }

  return 0;
}

void print_coins(char *coins)
{
  int n;
  char work[NUM_COINS * 2];

  for (n = 0; n < NUM_COINS; n++) {
    work[n * 2] = coins[n];

    if (n < NUM_COINS - 1)
      work[(n * 2) + 1] = ' ';
    else
      work[(n * 2) + 1] = 0;
  }

  printf("%s\n",work);
}

void do_transform(char *coins)
{
  int n;
  char coin_spots[NUM_COINS + 4];

  for (n = 0; n < NUM_COINS; n++)
    coin_spots[n] = coins[n];

  coin_spots[NUM_COINS] = coin_spots[0];
  coin_spots[NUM_COINS + 1] = coin_spots[1];

  coin_spots[NUM_COINS + 2] = coin_spots[NUM_COINS - 1];
  coin_spots[NUM_COINS + 3] = coin_spots[NUM_COINS];

  coin_spots[NUM_COINS - 1] = coin_spots[2];
  coin_spots[NUM_COINS] = coin_spots[3];

  for (n = 0; n < NUM_COINS; n++)
    coins[n] = coin_spots[n + 4];
}
