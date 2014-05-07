#include "sbfFatal.h"

void
sbfFatal (const char* s)
{
    fprintf (stderr, "%s\n", s);
    fflush (stderr);
    abort ();
    exit (1);
}

