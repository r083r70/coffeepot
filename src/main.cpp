#include "core/log.h"

int main(void)
{
    coffeepot::Log::Init();
    CP_DEBUG("Hello world!");
    return 0;
}