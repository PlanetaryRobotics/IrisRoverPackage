#ifndef HEATSHRINK_CONFIG_H
#define HEATSHRINK_CONFIG_H

/* Should functionality assuming dynamic allocation be used? */
#ifndef HEATSHRINK_DYNAMIC_ALLOC
#define HEATSHRINK_DYNAMIC_ALLOC 0
#endif

#if HEATSHRINK_DYNAMIC_ALLOC
/* Optional replacement of malloc/free */
#define HEATSHRINK_MALLOC(SZ) malloc(SZ)
#define HEATSHRINK_FREE(P, SZ) free(P)
#else
/* Required parameters for static configuration */
#define HEATSHRINK_STATIC_INPUT_BUFFER_SIZE 32
// Heatshrink settings, shown by heatshrink Jupyter test sandbox to be optimal
// values for our images:
// NOTE: Lesser values for WINDOW_BITS are acceptable if necessary to save
// space.
#define HEATSHRINK_STATIC_WINDOW_BITS 10
#define HEATSHRINK_STATIC_LOOKAHEAD_BITS 4
#endif

/* Turn on logging for debugging. */
#define HEATSHRINK_DEBUGGING_LOGS 0

/* Use indexing for faster compression. (This requires additional space.) */
#define HEATSHRINK_USE_INDEX 1

#endif