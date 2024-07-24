#include <stdlib.h>
#include "processo.h"
#include "cpu.h"

inline void cpuDesaloca(CPU *objeto)
{
    free(objeto);
}