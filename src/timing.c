#include <time.h>

// `millis()` function, just like on the Arduino
long long millis(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long) (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}
