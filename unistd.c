#include <unistd.h>
#include "Arduino.h"

unsigned int sleep(unsigned int seconds)
{
    delay(seconds * 1000);
    return 0;
}
