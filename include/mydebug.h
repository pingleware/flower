
/* Copyright (c) 1996, 1997 Craig Schneiderwent */
#define MY_DEBUG
#define MY_DEBUG_MSG_SZ 257

#define TRACE_ENTRY( functionName ) \
    { \
        fprintf( stderr, "%s entry\n", functionName ); \
    }

#define TRACE_EXIT( functionName ) \
    { \
        fprintf( stderr, "%s exit\n", functionName ); \
    }

#define TRACE_MSG( functionName, message ) \
    { \
        fprintf( stderr, "%s: %s\n", functionName, message ); \
    }

#undef MY_DEBUG
