/*
 * 2048 by Petre Leonard Macamete
 */

#include <stdio.h>
#include <stdlib.h>
#include <emscripten.h>
#include <time.h>
const size = 4;
int memory[size][size];
int score = 0;

int moveInMemory(int from, int to){
  int* _from = &memory[from/size][from%size];
  int* _to = &memory[to/size][to%size];
  if(*_from == 0) return 0;
  if(*_to == 0){
    *_to = memory[from/size][from%size];
    *_from = 0;
    return 1;
  }
  if(*_to == *_from){
    *_to *= 2;
    score += *_to;
    *_from = 0;
    return 1;
  }
  return 0;
}

int moveInMemoryBypassingEmptyCells(int from,  int to){
  int difference = to - from;
  while(
    !memory[from/size][from%size]
    && from - difference >= 0 && from - difference < 16
    && (
      difference == 1 || difference == -1?
      from / size == (from - difference) / size:
      1
    )
  ) {
    from -= difference;
  }
  return moveInMemory(from, to);
}

int moveInit(int horizontal, int vertical){
  int moved = 0;
  if(horizontal){
    if (horizontal == -1){
      for(int collumn = 0; collumn < 3; collumn ++){
        for(int line = 0; line < size; line ++){
          int toCell = line*size + collumn;
          if (memory[toCell/size][toCell%size] == 0){
            moved |= moveInMemoryBypassingEmptyCells(toCell+1, toCell);
          }
          moved |= moveInMemoryBypassingEmptyCells(toCell+1, toCell);
        }
      }
    }
    if (horizontal == 1){
      for(int collumn = 3; collumn >= 1; collumn --){
        for(int line = 3; line >= 0; line --){
          int toCell = line*size + collumn;
          if (memory[toCell/size][toCell%size] == 0){
            moved |= moveInMemoryBypassingEmptyCells(toCell-1, toCell);;
          }
          moved |= moveInMemoryBypassingEmptyCells(toCell-1, toCell);
        }
      }
    }
  }
  if(vertical){
    if (vertical == -1){
      for(int line = 0; line < size; line ++){
        for(int collumn = 0; collumn < size; collumn ++){
          int toCell = line*size + collumn;
          if (memory[toCell/size][toCell%size] == 0){
            moved |= moveInMemoryBypassingEmptyCells(toCell+size, toCell);
          }
          moved |= moveInMemoryBypassingEmptyCells(toCell+size, toCell);
        }
      }
    }
    if (vertical == 1){
      for(int line = 3; line >= 0; line --){
        for(int collumn = 3; collumn >= 0; collumn --){
          int toCell = line*size + collumn;
          if (memory[toCell/size][toCell%size] == 0){
            moved |= moveInMemoryBypassingEmptyCells(toCell-size, toCell);
          }
          moved |= moveInMemoryBypassingEmptyCells(toCell-size, toCell);
        }
      }
    }
  }
  return moved;
}

int number(){
  return rand()%5?2:size;
}

EMSCRIPTEN_KEEPALIVE
void init(){
  srand(time(NULL));
  memory[rand()%size][rand()%size] = number();
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
      if ( !memory[index/size][index%size] ){
        emptyCells[emptyCellsCount] = index;
        emptyCellsCount ++;
      }
    }
    
    int cellToBeFilled = emptyCells[rand() % emptyCellsCount];
    memory[cellToBeFilled/size][cellToBeFilled%size] = number();
  }
}

EMSCRIPTEN_KEEPALIVE
int movable(){
  for(int iterator = 0; iterator < 16; iterator++){
    if(!memory[iterator / size][iterator % size]){
      return 1;
    }
  }
  for(int row = 0; row < 3; row ++){
    for(int collumn = 0; collumn < size; collumn ++) {
      if(memory[row][collumn] == memory[row + 1][collumn]) return 1;
    }
  }for(int row = 0; row < size; row ++){
    for(int collumn = 0; collumn < 3; collumn ++) {
      if(memory[row][collumn] == memory[row][collumn + 1]) return 1;
    }
  }
  return 0;
}

int won(){
  for(int row = 0; row < size; row ++){
    for(int collumn = 0; collumn < size; collumn ++) {
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
  for(int line = 0; line < size; line++){
    for(int collumn = 0; collumn < size; collumn++){
      printf(" %6d", memory[line][collumn]);
    }
    printf("\n");
  }
  printf("\n");
}
