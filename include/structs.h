
/* Copyright (c) 1996, 1997 Craig Schneiderwent */
/*
Program: flower
File:    structs.h
Author:  Craig Schneiderwent
         74631.165@compuserve.com
Date:    14-Apr-1996

structs and #defines for flower.  The
version number was just to keep track of
who was testing which iteration.

*/


#define MYNAME          "flower\0"
#define MYVERSION       "04.00\0"

/* feel free to adjust these to your needs */
#define MAXFUNCNAMESIZE 65
#define MAXFILENAMESIZE 129

struct funcInfo {
    char            funcNm [ MAXFUNCNAMESIZE ];
    char            fileNm [ MAXFILENAMESIZE ];
    unsigned long   callsFromMe;
    unsigned long   callsToMe;
    short           isCalled;
    short           isCircularReference;
    struct funcInfo *calledFuncBase;
    struct funcInfo *calledFuncListStart;
    struct funcInfo *calledFuncListEnd;
    struct funcInfo *next;
    struct funcInfo *prev;
} _funcInfo;

struct fileInfo {
    char            fileNm [ MAXFILENAMESIZE ];
    struct funcInfo *funcListStart;
    struct funcInfo *funcListEnd;
    struct fileInfo *next;
    struct fileInfo *prev;
} _fileInfo;

struct isCalledByFuncInfo {
    char                      funcNm [ MAXFUNCNAMESIZE ];
    char                      fileNm [ MAXFILENAMESIZE ];
    unsigned long             count;
    struct isCalledByFuncInfo *callingFuncListStart;
    struct isCalledByFuncInfo *callingFuncListEnd;
    struct isCalledByFuncInfo *next;
    struct isCalledByFuncInfo *prev;
} _isCalledByFuncInfo;
