#include <stdio.h>
#include <string.h>

int main(){
  while(1){
    printf("\n$ ");
    char buffer[100];
    fgets(buffer, 100 , stdin);
    buffer[strlen(buffer)-1] = 0;
    printf("%s",buffer);
  }
  printf("\n");
  return 0;
}
