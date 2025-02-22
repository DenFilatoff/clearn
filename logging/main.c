#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "logging.h"


int main()
{
    printf("Hello World!\n");


   error("signal");
//   debug("signal");
//   info("signal");
//   warn("signal");

return 0;
}

