
/* Copyright (c) 1996, 1997 Craig Schneiderwent */
/*
Program: flower
File:    memfunc.c
Author:  Craig Schneiderwent
         74631.165@compuserve.com
Date:    01-Dec-1996

These are helper functions for flower.  They
allocate structures used by the linked lists
and initialize those structures.

*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include "mydebug.h"
#include "structs.h"
#include "memfunc.h"


struct funcInfo *funcInfoMalloc( void )
{
    struct funcInfo *new = NULL;

    new = ( struct funcInfo * ) malloc( sizeof( _funcInfo ) );
    if ( new != NULL ) {
        initFuncInfo( new );
    } /* endif */

    return( new );
}

struct fileInfo *fileInfoMalloc( void )
{
    struct fileInfo *new = NULL;

    new = ( struct fileInfo * ) malloc( sizeof( _fileInfo ) );
    if ( new != NULL ) {
        initFileInfo( new );
    } /* endif */

    return( new );
}

struct isCalledByFuncInfo *isCalledByMalloc( void )
{
    struct isCalledByFuncInfo *new = NULL;

    new = ( struct isCalledByFuncInfo * ) 
                malloc( sizeof( _isCalledByFuncInfo ) );
    if ( new != NULL ) {
         initIsCalledByFuncInfo( new );
    } /* endif */

    return( new );
}

void initFileInfo( struct fileInfo *psFileInfo )
{
    memset( psFileInfo, '\0', sizeof( _fileInfo ) );
    psFileInfo->next = NULL;
    psFileInfo->prev = NULL;
    psFileInfo->funcListStart = NULL;
    psFileInfo->funcListEnd = NULL;

    return;
}

void initFuncInfo( struct funcInfo *psFuncInfo )
{
    memset( psFuncInfo, '\0', sizeof( _funcInfo ) );
    psFuncInfo->callsFromMe = 0;
    psFuncInfo->callsToMe = 0;
    psFuncInfo->isCalled = 0;
    psFuncInfo->isCircularReference = 0;
    psFuncInfo->next = NULL;
    psFuncInfo->prev = NULL;
    psFuncInfo->calledFuncBase = NULL;
    psFuncInfo->calledFuncListStart = NULL;
    psFuncInfo->calledFuncListEnd = NULL;

    return;
}

void initIsCalledByFuncInfo( struct isCalledByFuncInfo *psFuncInfo )
{
    memset( psFuncInfo, '\0', sizeof( _isCalledByFuncInfo ) );
    psFuncInfo->next = NULL;
    psFuncInfo->prev = NULL;
    psFuncInfo->callingFuncListStart = NULL;
    psFuncInfo->callingFuncListEnd = NULL;
    psFuncInfo->count = 0;

    return;
}

void copyFuncInfo( struct funcInfo *pIn
                 , struct funcInfo *pOut )
{
    memset( pOut, '\0', sizeof( _funcInfo ) );
    strcpy( pOut->funcNm, pIn->funcNm );
    strcpy( pOut->fileNm, pIn->fileNm );
    pOut->callsFromMe = pIn->callsFromMe;
    pOut->callsToMe = pIn->callsToMe;
    pOut->isCalled = pIn->isCalled;

    return;
}
