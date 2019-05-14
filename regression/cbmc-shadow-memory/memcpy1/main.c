#include <string.h>
int a [10];

int main(){
   int b[10];
   b[9]=1;
   memcpy(a,b,10*sizeof(int));
   assert(b[9]==0); 
}
