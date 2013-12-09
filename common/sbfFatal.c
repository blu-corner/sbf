#include "sbfFatal.h"

void
sbfFatal (const char* s)
{
    fprintf (stderr, "%s\n", s);
    abort ();
    exit (1);
}

