#include "sbfFatal.h"

void
sbfFatal_die (const char* msg)
{
    fprintf (stderr, "%s\n", msg);
    fflush (stderr);
    abort ();
    exit (1);
}
