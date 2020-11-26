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

typedef struct {
  int line;
  int collumn;
} Coordinate;

Coordinate firstCell = {0, 0};
Coordinate lastCell = {size - 1, size - 1};

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
  if(from >= 0 && from < size * size){
    return moveInMemory(from, to);
  }
  return 0;
}

int verifyLeft(Coordinate coordinate) {
  return coordinate.collumn < size - 1;
}

void increaseLeft(Coordinate *coordinate) {
  coordinate->line += 1;
  if(coordinate->line == size) {
    coordinate->line = 0;
    coordinate->collumn += 1;
  }
}

int verifyRight(Coordinate coordinate) {
  return coordinate.collumn >= 1;
}

void increaseRight(Coordinate *coordinate) {
  coordinate->line -= 1;
  if(coordinate->line == -1) {
    coordinate->line = size - 1;
    coordinate->collumn -= 1;
  }
}

int verifyUp(Coordinate coordinate) {
  return coordinate.line < size - 1;
}

void increaseUp(Coordinate *coordinate) {
  coordinate->collumn += 1;
  if(coordinate->collumn == size) {
    coordinate->collumn = 0;
    coordinate->line += 1;
  }
}

int verifyDown(Coordinate coordinate) {
  return coordinate.line >= 1;
}

void increaseDown(Coordinate *coordinate) {
  coordinate->collumn -= 1;
  if(coordinate->collumn == -1) {
    coordinate->collumn = size - 1;
    coordinate->line -= 1;
  }
}

int moveCommand(int reverseMovement, Coordinate coordinate, int (*verify)(Coordinate), void (*increase)(Coordinate*)) {
  int moved = 0;
  for(; (*verify)(coordinate); (*increase)(&coordinate)){
    int toCell = coordinate.line*size + coordinate.collumn;
    if (memory[toCell/size][toCell%size] == 0){
      moved |= moveInMemoryBypassingEmptyCells(toCell+reverseMovement, toCell);
    }
    moved |= moveInMemoryBypassingEmptyCells(toCell+reverseMovement, toCell);
  }
  return moved;
}

int moveInit(int horizontal, int vertical){
  int moved = 0;
  if (horizontal == -1){
    moved = moveCommand(1, firstCell, &verifyLeft, &increaseLeft);
  }
  if (horizontal == 1){
    moved = moveCommand(-1, lastCell, &verifyRight, &increaseRight);
  }
  if (vertical == -1){
    moved = moveCommand(size, firstCell, &verifyUp, &increaseUp);
  }
  if (vertical == 1){
    moved = moveCommand(-size, lastCell, &verifyDown, &increaseDown);
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
