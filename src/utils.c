#include <utils.h>

uint16_t checksum(const char *data, int len) {
    uint32_t sum = 0;
    for (int i = 0; i < len; i++) {
        sum += data[i];
    }
    return (uint16_t)sum;
}