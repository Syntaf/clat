#include <stdlib.h>
#include <stdio.h>

#include "clib.h"

int main() 
{
    clib_init();

    request(400, NULL);
    request(2004, NULL);
    request(15832, NULL);
    request(4099, NULL);
}



