#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

int main (void)
{
  if(wiringPiSetup() == -1)
  printf("ERROR");

  for (;;)
{
    if (digitalRead(5) != 0) {
        printf ("Button Released!\n") ;
    }else {
        printf ("Button Pressed \n") ;
    }
  return 0 ;
}
}
