#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

int main (void)
{
  if(wiringPiSetup() == -1)
  printf("ERROR");

for (;;) {
    if ((digitalRead(5) != 1) | (digitalRead(4) != 1))  {
        printf ("Roof Closed!\n") ;
    }else {
        printf ("Roof Opened! \n") ;
    }
  return 0 ;
}
}
