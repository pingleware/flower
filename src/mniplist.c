
/* Copyright (c) 1996, 1997 Craig Schneiderwent */
/*
Program: flower
File:    mniplist.c
Author:  Craig Schneiderwent
         74631.165@compuserve.com
Date:    14-Apr-1996

These are helper functions for flower.  They
manipulate the linked lists which hold the
function information for the files flower
has been asked to analyze.

*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include "mydebug.h"
#include "structs.h"
#include "mniplist.h"
#include "memfunc.h"

int addToFileList( struct fileInfo *psFileInfo, char *fileNm )
{
    #ifdef MY_DEBUG
    char            *thisFuncNm = "addToFuncList\0";
    char            debug_msg[ MY_DEBUG_MSG_SZ ];
    #endif
    struct fileInfo *tail = NULL;
    struct fileInfo *new = NULL;

    #ifdef MY_DEBUG
    TRACE_ENTRY( thisFuncNm );
    sprintf( debug_msg, " fileNm = %s", fileNm );
    TRACE_MSG( thisFuncNm, debug_msg );
    #endif

    if ( strlen( psFileInfo->fileNm ) == 0 ) {
        /* first file */
        strncpy( psFileInfo->fileNm, fileNm, MAXFILENAMESIZE );
        return( 0 );
    } /* endif */

    tail = endOfFileList( psFileInfo );

    new = fileInfoMalloc( );
    if ( new == NULL ) {
        return( 3 );
    } /* endif */

    strncpy( new->fileNm, fileNm, MAXFILENAMESIZE );
    new->prev = tail;
    tail->next = new;

    #ifdef MY_DEBUG
    TRACE_EXIT( thisFuncNm );
    #endif

    return( 0 );
}

int addToFuncList( struct fileInfo *psFileInfo, char *funcNm )
{
    #ifdef MY_DEBUG
    char            *thisFuncNm = "addToFuncList\0";
    #endif
    struct funcInfo *new = NULL;

    #ifdef MY_DEBUG
    TRACE_ENTRY( thisFuncNm );
    TRACE_MSG( thisFuncNm, funcNm );
    #endif

    new = funcInfoMalloc(  );
    if ( new == NULL ) {
        return( 3 );
    } /* endif */

    strncpy( new->funcNm, funcNm, MAXFUNCNAMESIZE );
    strncpy( new->fileNm, psFileInfo->fileNm, MAXFILENAMESIZE );
    new->prev = psFileInfo->funcListEnd;
    psFileInfo->funcListEnd = new;
    if ( psFileInfo->funcListStart == NULL ) {
        /* first one */
        psFileInfo->funcListStart = new;
    } else {
        new->prev->next = new;
    } /* endif */

    #ifdef MY_DEBUG
    TRACE_EXIT( thisFuncNm );
    #endif

    return( 0 );
}

int addToCalledFuncList( struct funcInfo *psFuncInfo, char *funcNm )
{
    struct funcInfo *new = NULL;
    short  isANewOne = 0;

    new = funcInCalledList( funcNm, psFuncInfo );
    if ( new == NULL ) {
        new = funcInfoMalloc(  );
        if ( new == NULL ) {
            return( 3 );
        } /* endif */
        strncpy( new->funcNm, funcNm, MAXFUNCNAMESIZE );
        new->callsToMe = 1;
        new->prev = psFuncInfo->calledFuncListEnd;
        psFuncInfo->calledFuncListEnd = new;
        isANewOne = 1;
    } else {
        new->callsToMe++;
    } /* endif */

    if ( psFuncInfo->calledFuncListStart == NULL ) {
        /* first one */
        psFuncInfo->calledFuncListStart = new;
        psFuncInfo->callsFromMe = 1;
    } else {
        if ( isANewOne ) {
            new->prev->next = new;
        } /* endif */
        psFuncInfo->callsFromMe++;
    } /* endif */

    return( 0 );
}

int createCalledByFuncList( struct fileInfo *psFileInfo
                          , struct isCalledByFuncInfo *psCalledByList )
{
    #ifdef MY_DEBUG
    char            *thisFuncNm = "createCalledByFuncList\0";
    #endif
    struct fileInfo *nextFile = NULL;
    struct funcInfo *nextFunc = NULL;
    struct funcInfo *nextCalledFunc = NULL;
    int             rc = 0;

    #ifdef MY_DEBUG
    TRACE_ENTRY( thisFuncNm );
    #endif

    nextFile = startOfFileList( psFileInfo );
    nextFunc = nextFile->funcListStart;
    nextCalledFunc = nextFunc->calledFuncListStart;

    while ( nextFile != NULL ) {
        while ( nextFunc != NULL ) {
            while ( nextCalledFunc != NULL ) {
                rc = addToCalledByFuncList( psCalledByList
                                          , nextFunc
                                          , nextCalledFunc );
                if ( rc != 0 ) {
                    return( rc );
                } /* endif */
                nextCalledFunc = nextCalledFunc->next;
            } /* endwhile */
            nextFunc = nextFunc->next;
            if ( nextFunc != NULL ) {
                nextCalledFunc = nextFunc->calledFuncListStart;
            } /* endif */
        } /* endwhile */
        nextFile = nextFile->next;
        if ( nextFile != NULL ) {
            nextFunc = nextFile->funcListStart;
            if ( nextFunc != NULL ) {
                nextCalledFunc = nextFunc->calledFuncListStart;
            } /* endif */
        } /* endif */
    } /* endwhile */

    #ifdef MY_DEBUG
    TRACE_EXIT( thisFuncNm );
    #endif

    return( 0 );
}

int addToCalledByFuncList( struct isCalledByFuncInfo *psCalledByList
                         , struct funcInfo *psCallingFuncInfo
                         , struct funcInfo *psCalledFuncInfo )
{
    #ifdef MY_DEBUG
    char            *thisFuncNm = "addToCalledByFuncList\0";
    char            debug_msg[ MY_DEBUG_MSG_SZ ];
    #endif
    struct isCalledByFuncInfo *tail = NULL;
    struct isCalledByFuncInfo *new = NULL;
    struct isCalledByFuncInfo *newCaller = NULL;

    #ifdef MY_DEBUG
    TRACE_ENTRY( thisFuncNm );
    sprintf( debug_msg
           , "\n  psCallingFuncInfo->funcNm = %s\n  psCalledFuncInfo->funcNm = %s"
           , psCallingFuncInfo->funcNm
           , psCalledFuncInfo->funcNm );
    TRACE_MSG( thisFuncNm, debug_msg );
    #endif

    if ( strlen( psCalledByList->funcNm ) == 0 ) {
        /* first one */
        new = psCalledByList;
        strncpy( new->fileNm
               , psCalledFuncInfo->fileNm
               , MAXFILENAMESIZE );
        strncpy( new->funcNm
               , psCalledFuncInfo->funcNm
               , MAXFUNCNAMESIZE );
    } else {
        new = funcInCalledByFuncList( psCalledFuncInfo->funcNm
                                    , psCalledByList );
    } /* endif */

    if ( new == NULL ) {
        new = isCalledByMalloc( );
        if ( new == NULL ) {
            return( 3 );
        } /* endif */
        tail = endOfCalledByFuncList( psCalledByList );
        if ( tail == NULL ) {
            /* first one */
            tail = psCalledByList;
            initIsCalledByFuncInfo( tail );
        } /* endif */
        new->prev = tail;
        new->next = NULL;
        tail->next = new;
        strncpy( new->fileNm
               , psCalledFuncInfo->fileNm
               , MAXFILENAMESIZE );
        strncpy( new->funcNm
               , psCalledFuncInfo->funcNm
               , MAXFUNCNAMESIZE );
    } /* endif */

    newCaller = callerInList( psCallingFuncInfo->funcNm, new );
    if ( newCaller == NULL ) {
        newCaller = isCalledByMalloc( );
        if ( newCaller == NULL ) {
            return( 3 );
        } /* endif */
        strncpy( newCaller->fileNm
               , psCallingFuncInfo->fileNm
               , MAXFILENAMESIZE );
        strncpy( newCaller->funcNm
               , psCallingFuncInfo->funcNm
               , MAXFUNCNAMESIZE );
        newCaller->prev = new->callingFuncListEnd;
        new->callingFuncListEnd = newCaller;
        if ( new->callingFuncListStart == NULL ) {
            /* first one */
            new->callingFuncListStart = newCaller;
        } else {
            newCaller->prev->next = newCaller;
        } /* endif */
    } /* endif */

    newCaller->count++;
    new->count++;

    #ifdef MY_DEBUG
    TRACE_EXIT( thisFuncNm );
    #endif

    return( 0 );
}

void freeLists( struct fileInfo *psFileInfo )
{
    struct fileInfo *nextFile = NULL;
    struct funcInfo *nextFunc = NULL;
    struct funcInfo *nextCalledFunc = NULL;
    struct fileInfo *thisFile = NULL;
    struct funcInfo *thisFunc = NULL;
    struct funcInfo *thisCalledFunc = NULL;

    thisFile = psFileInfo;

    while ( thisFile != NULL ) {
        thisFunc = thisFile->funcListStart;
        while ( thisFunc != NULL ) {
            thisCalledFunc = thisFunc->calledFuncListStart;
            while ( thisCalledFunc != NULL ) {
                nextCalledFunc = thisCalledFunc->next;
                free( thisCalledFunc );
                thisCalledFunc = nextCalledFunc;
            } /* endwhile */
            nextFunc = thisFunc->next;
            free( thisFunc );
            thisFunc = nextFunc;
        } /* endwhile */
        nextFile = thisFile->next;
        if ( thisFile != psFileInfo ) {
            free( thisFile );
        } /* endif */
        thisFile = nextFile;
    } /* endwhile */

    return;
}

void freeIsCalledByList( struct isCalledByFuncInfo *psFuncInfo )
{
    struct isCalledByFuncInfo *thisFunc = NULL;
    struct isCalledByFuncInfo *thisCallingFunc = NULL;
    struct isCalledByFuncInfo *nextFunc = NULL;
    struct isCalledByFuncInfo *nextCallingFunc = NULL;

    thisFunc = psFuncInfo;

    while ( thisFunc != NULL ) {
        thisCallingFunc = thisFunc->callingFuncListStart;
        while ( thisCallingFunc != NULL ) {
            nextCallingFunc = thisCallingFunc->next;
            free( thisCallingFunc );
            thisCallingFunc = nextCallingFunc;
        } /* endwhile */
        nextFunc = thisFunc->next;
        if ( thisFunc != psFuncInfo ) {
            free( thisFunc );
        } /* endif */
        thisFunc = nextFunc;
    } /* endwhile */

    return;
}

struct fileInfo *endOfFileList( struct fileInfo *psFileInfo )
{
    struct fileInfo *next = NULL;
    struct fileInfo *tail = NULL;

    tail = psFileInfo;
    next = tail->next;

    while ( next != NULL ) {
        tail = next;
        next = tail->next;
    } /* endwhile */

    return( tail );
}

struct fileInfo *startOfFileList( struct fileInfo *psFileInfo )
{
    #ifdef MY_DEBUG
    char            *thisFuncNm = "startOfFileList\0";
    #endif
    struct fileInfo *prev = NULL;
    struct fileInfo *head = NULL;

    #ifdef MY_DEBUG
    TRACE_ENTRY( thisFuncNm );
    #endif

    head = psFileInfo;
    prev = head->prev;

    while ( prev != NULL ) {
        head = prev;
        prev = head->prev;
    } /* endwhile */

    #ifdef MY_DEBUG
    TRACE_EXIT( thisFuncNm );
    #endif

    return( head );
}

struct isCalledByFuncInfo *endOfCalledByFuncList( 
          struct isCalledByFuncInfo *psFuncInfo )
{
    struct isCalledByFuncInfo *next = NULL;
    struct isCalledByFuncInfo *tail = NULL;

    tail = psFuncInfo;
    next = tail->next;

    while ( next != NULL ) {
        tail = next;
        next = tail->next;
    } /* endwhile */

    return( tail );
}

struct isCalledByFuncInfo *startOfCalledByFuncList( 
            struct isCalledByFuncInfo *psFuncInfo )
{
    struct isCalledByFuncInfo *prev = NULL;
    struct isCalledByFuncInfo *head = NULL;

    head = psFuncInfo;
    prev = head->prev;

    while ( prev != NULL ) {
        head = prev;
        prev = head->prev;
    } /* endwhile */

    return( head );
}

struct isCalledByFuncInfo *funcInCalledByFuncList( char *funcNm
                             , struct isCalledByFuncInfo *psFuncInfo )
{
    struct isCalledByFuncInfo *next = NULL;
    struct isCalledByFuncInfo *head = NULL;

    head = startOfCalledByFuncList( psFuncInfo );
    if ( head == NULL ) {
        return( head );
    } else {
        next = head->next;
    } /* endif */

    while ( next != NULL && 
            strncmp( funcNm, head->funcNm, MAXFUNCNAMESIZE ) ) {
        head = next;
        next = head->next;
    } /* endwhile */

    if ( strncmp( funcNm, head->funcNm, MAXFUNCNAMESIZE ) ) {
        head = NULL;
    } /* endif */
    return( head );
}

struct isCalledByFuncInfo *callerInList( char *funcNm
                         , struct isCalledByFuncInfo *psFuncInfo )
{
    struct isCalledByFuncInfo *next = NULL;
    struct isCalledByFuncInfo *caller = NULL;

    caller = psFuncInfo->callingFuncListStart;
    if ( caller == NULL ) {
        return( caller );
    } /* endif */
    next = caller->next;

    while ( next != NULL && 
            strncmp( funcNm, caller->funcNm, MAXFUNCNAMESIZE ) ) {
        caller = next;
        next = caller->next;
    } /* endwhile */

    if ( strncmp( funcNm, caller->funcNm, MAXFUNCNAMESIZE ) ) {
        caller = NULL;
    } /* endif */
    return( caller );
}

struct funcInfo *nextNotCalledFunc( struct funcInfo *psFuncInfo )
{
    struct funcInfo *aFunc = NULL;

    aFunc = psFuncInfo;

    if ( aFunc == NULL ) {
        return( aFunc );
    } /* endif */

    while ( aFunc != NULL && aFunc->isCalled ) {
        aFunc = aFunc->next;
    } /* endwhile */

    return( aFunc );
}

struct funcInfo *funcInFileList( char *funcNm
                               , struct fileInfo *pFileStart )
{
    struct fileInfo *nextFile = NULL;
    struct funcInfo *nextFunc = NULL;
    int             strcmpResult = 1;

    nextFile = pFileStart;

    if ( nextFile == NULL ) {
        return( NULL );
    } /* endif */

    while ( nextFile != NULL && strcmpResult ) {
        nextFunc = nextFile->funcListStart;
        while ( nextFunc != NULL && strcmpResult ) {
            strcmpResult = strcmp( funcNm, nextFunc->funcNm );
            if ( !strcmpResult ) {
                continue;
            } /* endif */
            nextFunc = nextFunc->next;
        } /* endwhile */
        if ( !strcmpResult ) {
            continue;
        } /* endif */
        nextFile = nextFile->next;
    } /* endwhile */

    if ( strcmpResult ) {
        nextFunc = NULL;
    } /* endif */

    return( nextFunc );
}

struct funcInfo *funcInCalledList( char *funcNm
                                 , struct funcInfo *psFuncInfo )
{
    struct funcInfo *next = NULL;
    struct funcInfo *head = NULL;

    if ( psFuncInfo == NULL ) {
        return( NULL );
    } /* endif */

    head = psFuncInfo->calledFuncListStart;
    if ( head == NULL ) {
        return( head );
    } else {
        next = head->next;
    } /* endif */

    while ( next != NULL && 
            strncmp( funcNm, head->funcNm, MAXFUNCNAMESIZE ) ) {
        head = next;
        next = head->next;
    } /* endwhile */

    if ( strcmp( funcNm, head->funcNm ) ) {
        head = NULL;
    } /* endif */

    return( head );
}
