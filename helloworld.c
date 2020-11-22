/*
 * 2048 by Petre Leonard Macamete
 */

#include <stdio.h>
#include <stdlib.h>
#include <emscripten.h>
#include <time.h>

int memory[4][4];
int score = 0;

int moveInMemory(int from, int to){
  if(memory[from/4][from%4] == 0) return 0;
  if(memory[to/4][to%4] == 0){
    memory[to/4][to%4] = memory[from/4][from%4];
    memory[from/4][from%4] = 0;
    return 1;
  }
  if(memory[to/4][to%4] == memory[from/4][from%4]){
    memory[to/4][to%4] *= 2;
    score += memory[to/4][to%4];
    memory[from/4][from%4] = 0;
    return 1;
  }
  return 0;
}

int moveInMemoryBypassingEmptyCells(int from,  int to){
  int difference = to - from;
  while(!memory[from/4][from%4] && from - difference >= 0 && from - difference < 16 && (difference==1||difference==-1?from/4==(from-difference)/4:1)) {
    from -= difference;
  }
  if(from >= 0 && from < 16){
    return moveInMemory(from, to);
  }
  return 0;
}

int moveInit(int horizontal, int vertical){
  int moved = 0;
  if(horizontal){
    if (horizontal == -1){
      for(int collumn = 0; collumn < 3; collumn ++){
        for(int line = 0; line < 4; line ++){
          int toCell = line*4 + collumn;
          if (memory[toCell/4][toCell%4] == 0){
            moved |= moveInMemoryBypassingEmptyCells(toCell+1, toCell);
          }
          moved |= moveInMemoryBypassingEmptyCells(toCell+1, toCell);
        }
      }
    }
    if (horizontal == 1){
      for(int collumn = 3; collumn >= 1; collumn --){
        for(int line = 3; line >= 0; line --){
          int toCell = line*4 + collumn;
          if (memory[toCell/4][toCell%4] == 0){
            moved |= moveInMemoryBypassingEmptyCells(toCell-1, toCell);;
          }
          moved |= moveInMemoryBypassingEmptyCells(toCell-1, toCell);
        }
      }
    }
  }
  if(vertical){
    if (vertical == -1){
      for(int line = 0; line < 4; line ++){
        for(int collumn = 0; collumn < 4; collumn ++){
          int toCell = line*4 + collumn;
          if (memory[toCell/4][toCell%4] == 0){
            moved |= moveInMemoryBypassingEmptyCells(toCell+4, toCell);
          }
          moved |= moveInMemoryBypassingEmptyCells(toCell+4, toCell);
        }
      }
    }
    if (vertical == 1){
      for(int line = 3; line >= 0; line --){
        for(int collumn = 3; collumn >= 0; collumn --){
          int toCell = line*4 + collumn;
          if (memory[toCell/4][toCell%4] == 0){
            moved |= moveInMemoryBypassingEmptyCells(toCell-4, toCell);
          }
          moved |= moveInMemoryBypassingEmptyCells(toCell-4, toCell);
        }
      }
    }
  }
  return moved;
}

int number(){
  return rand()%5?2:4;
}

EMSCRIPTEN_KEEPALIVE
void init(){
  srand(time(NULL));
  memory[rand()%4][rand()%4] = number();
}

EMSCRIPTEN_KEEPALIVE
void move(char move) {
  printf("%c\n", move);
  int emptyCellsCount = 0;
  int emptyCells[16];

  int moved = 1;
  switch (move)
  {
  case 'r':
    moved = moveInit(1,0);
    break;
  case 'l':
    moved = moveInit(-1,0);
    break;
  case 'u':
    moved = moveInit(0,-1);
    break;
  case 'd':
    moved = moveInit(0,1);
    break;
  
  default:
    break;
  }
  if (moved) {
    for (size_t index = 0; index < 16; index++)
    {
      /* code */
      if ( !memory[index/4][index%4] ){
        emptyCells[emptyCellsCount] = index;
        emptyCellsCount ++;
      }
    }
    
    int cellToBeFilled = emptyCells[rand() % emptyCellsCount];
    memory[cellToBeFilled/4][cellToBeFilled%4] = number();
  }
}

EMSCRIPTEN_KEEPALIVE
int movable(){
  for(int iterator = 0; iterator < 16; iterator++){
    if(!memory[iterator / 4][iterator % 4]){
      return 1;
    }
  }
  for(int row = 0; row < 3; row ++){
    for(int collumn = 0; collumn < 4; collumn ++) {
      if(memory[row][collumn] == memory[row + 1][collumn]) return 1;
    }
  }for(int row = 0; row < 4; row ++){
    for(int collumn = 0; collumn < 3; collumn ++) {
      if(memory[row][collumn] == memory[row][collumn + 1]) return 1;
    }
  }
  return 0;
}

int won(){
  for(int row = 0; row < 4; row ++){
    for(int collumn = 0; collumn < 4; collumn ++) {
      if(memory[row][collumn] >= 2048) return 1;
    }
  }
  return 0;
}

EMSCRIPTEN_KEEPALIVE
void read() {
  if( won() ) printf("You won! ");
  if( !won() && !movable() ) printf("You lost! ");
  printf("Score: %d\n", score);
  for(int line = 0; line < 4; line++){
    for(int collumn = 0; collumn < 4; collumn++){
      printf(" %6d", memory[line][collumn]);
    }
    printf("\n");
  }
  printf("\n");
}
