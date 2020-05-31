#include "driver.h"

int main(int argc, char* argv[])
{
    uint8_t com_port = 0;
    if (argc > 1) {
        com_port = strtol(argv[1], NULL, 10);
        printf("%s\n", argv[1]);
    }
    init(com_port);
    return loop();
}
