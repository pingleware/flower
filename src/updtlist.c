
/* Copyright (c) 1996, 1997 Craig Schneiderwent */
/*
Program: flower
File:    updtlist.c
Author:  Craig Schneiderwent
         74631.165@compuserve.com
Date:    04-Dec-1996

These are helper functions for flower.  They
update the linked lists which hold the
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
#include "updtlist.h"

void updateNotCalledFuncList( struct fileInfo *psFileInfo
                            , struct isCalledByFuncInfo *psCalledByList )
{
    struct fileInfo           *nextFile = NULL;
    struct funcInfo           *nextFunc = NULL;
    struct isCalledByFuncInfo *isCalledBy = NULL;

    nextFile = startOfFileList( psFileInfo );
    nextFunc = nextFile->funcListStart;

    while ( nextFile != NULL ) {
        while ( nextFunc != NULL ) {
            isCalledBy = funcInCalledByFuncList( nextFunc->funcNm
                                               , psCalledByList );
            if ( isCalledBy != NULL ) {
                nextFunc->isCalled = 1;
            } /* endif */
            nextFunc = nextFunc->next;
        } /* endwhile */
        nextFile = nextFile->next;
        if ( nextFile != NULL ) {
            nextFunc = nextFile->funcListStart;
        } /* endif */
    } /* endwhile */

    return;
}

void updateAllCalledFuncFileNm( struct fileInfo *psFileInfo )
{
    struct fileInfo *nextFile = NULL;
    struct funcInfo *nextFunc = NULL;
    char            fileNm [ MAXFILENAMESIZE ];
    char            funcNm [ MAXFUNCNAMESIZE ];

    nextFile = startOfFileList( psFileInfo );
    nextFunc = nextFile->funcListStart;

    while ( nextFile != NULL ) {
        strncpy( fileNm, nextFile->fileNm, MAXFILENAMESIZE );
        while ( nextFunc != NULL ) {
            strncpy( funcNm, nextFunc->funcNm, MAXFUNCNAMESIZE );
            updateOneCalledFuncFileNm( nextFile, funcNm, fileNm );
            nextFunc = nextFunc->next;
        } /* endwhile */
        nextFile = nextFile->next;
        if ( nextFile != NULL ) {
            nextFunc = nextFile->funcListStart;
        } /* endif */
    } /* endwhile */

    return;
}

void updateOneCalledFuncFileNm( struct fileInfo *psFileInfo
                              , char *funcNm
                              , char *fileNm )
{
    struct fileInfo *nextFile = NULL;
    struct funcInfo *nextFunc = NULL;
    struct funcInfo *nextCalledFunc = NULL;
    int             thisIsIt = 0;

    nextFile = startOfFileList( psFileInfo );

    while ( nextFile != NULL ) {
        nextFunc = nextFile->funcListStart;
        while ( nextFunc != NULL ) {
            nextCalledFunc = nextFunc->calledFuncListStart;
            while ( nextCalledFunc != NULL ) {
                thisIsIt = strncmp( nextCalledFunc->funcNm
                                  , funcNm
                                  , MAXFUNCNAMESIZE );
                if ( thisIsIt == 0 ) {
                      strncpy( nextCalledFunc->fileNm
                             , fileNm
                             , MAXFILENAMESIZE );
                } /* endif */
                nextCalledFunc = nextCalledFunc->next;
            } /* endwhile */
            nextFunc = nextFunc->next;
        } /* endwhile */
        nextFile = nextFile->next;
    } /* endwhile */

    return;
}

void updateCalledFuncBase( struct fileInfo *pFileStart )
{
    struct fileInfo *nextFile = NULL;
    struct funcInfo *nextFunc = NULL;
    struct funcInfo *nextCalledFunc = NULL;

    nextFile = pFileStart;

    while ( nextFile != NULL ) {
        nextFunc = nextFile->funcListStart;
        while ( nextFunc != NULL ) {
            nextCalledFunc = nextFunc->calledFuncListStart;
            while ( nextCalledFunc != NULL ) {
                nextCalledFunc->calledFuncBase = 
                       funcInFileList( nextCalledFunc->funcNm
                                     , pFileStart );
                nextCalledFunc = nextCalledFunc->next;
            } /* endwhile */
            nextFunc = nextFunc->next;
        } /* endwhile */
        nextFile = nextFile->next;
    } /* endwhile */

    return;
}

void updateCircularReferences( struct fileInfo *pFileStart )
{
    #ifdef MY_DEBUG
    char            *thisFuncNm = "updateCircularReferences\0";
    char            debug_msg[ MY_DEBUG_MSG_SZ ];
    #endif
    struct fileInfo *nextFile = NULL;
    struct funcInfo *nextFunc = NULL;
    struct funcInfo *calledBase = NULL;
    struct funcInfo *nextCalled = NULL;
    struct funcInfo *nextSubCalled = NULL;
    struct funcInfo *upTree = NULL;

    #ifdef MY_DEBUG
    TRACE_ENTRY( thisFuncNm );
    #endif

    nextFile = pFileStart;

    while ( nextFile != NULL ) {
        #ifdef MY_DEBUG
        sprintf( debug_msg, "\tFILE: %s", nextFile->fileNm );
        TRACE_MSG( thisFuncNm, debug_msg );
        #endif
        nextFunc = nextFile->funcListStart;
        while ( nextFunc != NULL ) {
            #ifdef MY_DEBUG
            sprintf( debug_msg, "\t\tFUNC: %s", nextFunc->funcNm );
            TRACE_MSG( thisFuncNm, debug_msg );
            #endif
            nextCalled = nextFunc->calledFuncListStart;
            while ( nextCalled != NULL ) {
                #ifdef MY_DEBUG
                sprintf( debug_msg, "\t\t\tCALLED FUNC: %s"
                       , nextCalled->funcNm );
                TRACE_MSG( thisFuncNm, debug_msg );
                #endif
                calledBase = nextCalled->calledFuncBase;
                if ( calledBase == NULL ) {
                    nextCalled = nextCalled->next;
                    continue;
                } /* endif */
                nextSubCalled = calledBase->calledFuncListStart;
                doTheCircularReferenceUpdate( nextFunc
                                            , nextCalled
                                            , nextSubCalled
                                            , upTree );
                freeUpTreeList( upTree );
                upTree = NULL;
                nextCalled = nextCalled->next;
            } /* endwhile */
            nextFunc = nextFunc->next;
        } /* endwhile */
        nextFile = nextFile->next;
    } /* endwhile */

    #ifdef MY_DEBUG
    TRACE_EXIT( thisFuncNm );
    #endif

    return;
}

void doTheCircularReferenceUpdate( struct funcInfo *baseFunc
                                 , struct funcInfo *calledFunc
                                 , struct funcInfo *subCalledFunc
                                 , struct funcInfo *upTree )
{
    #ifdef MY_DEBUG
    char            *thisFuncNm = "doTheCircularReferenceUpdate\0";
    char            debug_msg[ MY_DEBUG_MSG_SZ ];
    #endif
    struct funcInfo *nextSubCalled = NULL;
    struct funcInfo *subCalledBase = NULL;
    struct funcInfo *nextUpTree = NULL;

    #ifdef MY_DEBUG
    TRACE_ENTRY( thisFuncNm );
    #endif

    if ( subCalledFunc == NULL || baseFunc == NULL || calledFunc == NULL ) {
        #ifdef MY_DEBUG
        if ( baseFunc == NULL ) {
            sprintf( debug_msg, "             baseFunc = NULL" );
        } else {
            sprintf( debug_msg, "     baseFunc->funcNm = %s", baseFunc->funcNm );
        } /* endif */
        TRACE_MSG( thisFuncNm, debug_msg );
        if ( calledFunc == NULL ) {
            sprintf( debug_msg, "           calledFunc = NULL" );
        } else {
            sprintf( debug_msg, "   calledFunc->funcNm = %s", calledFunc->funcNm );
        } /* endif */
        TRACE_MSG( thisFuncNm, debug_msg );
        if ( subCalledFunc == NULL ) {
            sprintf( debug_msg, "        subCalledFunc = NULL" );
        } else {
            sprintf( debug_msg, "subCalledFunc->funcNm = %s", subCalledFunc->funcNm );
        } /* endif */
        TRACE_MSG( thisFuncNm, debug_msg );
        TRACE_EXIT( thisFuncNm );
        #endif
        return;
    } /* endif */

    #ifdef MY_DEBUG
    sprintf( debug_msg, "     baseFunc->funcNm = %s [%p]", baseFunc->funcNm, baseFunc );
    TRACE_MSG( thisFuncNm, debug_msg );
    sprintf( debug_msg, "   calledFunc->funcNm = %s [%p]", calledFunc->funcNm, calledFunc );
    TRACE_MSG( thisFuncNm, debug_msg );
    #endif
    nextSubCalled = subCalledFunc;

    while ( nextSubCalled != NULL ) {
        subCalledBase = nextSubCalled->calledFuncBase;
        #ifdef MY_DEBUG
        sprintf( debug_msg, "nextSubCalled->funcNm = %s", nextSubCalled->funcNm );
        TRACE_MSG( thisFuncNm, debug_msg );
        sprintf( debug_msg, "subCalledBase = %p", subCalledBase );
        TRACE_MSG( thisFuncNm, debug_msg );
        #endif
        if ( subCalledBase == baseFunc ) {
            nextSubCalled->isCircularReference = 1;
            #ifdef MY_DEBUG
            sprintf( debug_msg, "%s subCalledBase == baseFunc [%s]"
                   , subCalledBase->funcNm
                   , baseFunc->funcNm );
            TRACE_MSG( thisFuncNm, debug_msg );
            #endif
            nextSubCalled = nextSubCalled->next;
            continue;
        } /* endif */
        if ( funcIsInUpTreeList( upTree, subCalledBase ) ) {
            nextSubCalled->isCircularReference = 1;
            #ifdef MY_DEBUG
            sprintf( debug_msg, "%s is in upTree list"
                   , nextSubCalled->funcNm );
            TRACE_MSG( thisFuncNm, debug_msg );
            #endif
            nextSubCalled = nextSubCalled->next;
            continue;
        } /* endif */
        if ( subCalledBase != NULL ) {
            nextUpTree = funcInfoMalloc( );
            if ( nextUpTree == NULL ) {
                return; /* punt, sorry */
            } /* endif */
            copyFuncInfo( subCalledBase, nextUpTree );
            if ( upTree != NULL ) {
                upTree->next = nextUpTree;
                nextUpTree->prev = upTree;
            } /* endif */
            doTheCircularReferenceUpdate( baseFunc
                                    , calledFunc
                                    , subCalledBase->calledFuncListStart
                                    , nextUpTree );
            if ( nextUpTree != NULL ) {
                if ( nextUpTree->prev != NULL ) {
                    nextUpTree->prev->next = NULL;
                } /* endif */
                free( nextUpTree );
            } /* endif */
            #ifdef MY_DEBUG
            sprintf( debug_msg, "RESUMING:" );
            TRACE_MSG( thisFuncNm, debug_msg );
            sprintf( debug_msg, "     baseFunc->funcNm = %s [%p]"
                   , baseFunc->funcNm, baseFunc );
            TRACE_MSG( thisFuncNm, debug_msg );
            sprintf( debug_msg, "   calledFunc->funcNm = %s [%p]"
                   , calledFunc->funcNm, calledFunc );
            TRACE_MSG( thisFuncNm, debug_msg );
            #endif
        } /* endif */
        nextSubCalled = nextSubCalled->next;
    } /* endwhile */

    #ifdef MY_DEBUG
    sprintf( debug_msg, "RETURNING: subCalledFunc->funcNm = %s"
           , subCalledFunc->funcNm );
    TRACE_MSG( thisFuncNm, debug_msg );
    TRACE_EXIT( thisFuncNm );
    #endif

    return;
}

int funcIsInUpTreeList( struct funcInfo *pTail
                      , struct funcInfo *pFunc )
{
    #ifdef MY_DEBUG
    char            *thisFuncNm = "funcIsInUpTreeList\0";
    char            debug_msg[ MY_DEBUG_MSG_SZ ];
    #endif
    struct funcInfo *prevFunc = NULL;
    int             rc = 0;

    #ifdef MY_DEBUG
    TRACE_ENTRY( thisFuncNm );
    #endif

    if ( pFunc == NULL ) {
        #ifdef MY_DEBUG
        sprintf( debug_msg, "pFunc == NULL" );
        TRACE_MSG( thisFuncNm, debug_msg );
        TRACE_EXIT( thisFuncNm );
        #endif
        return( rc );
    } /* endif */

    if ( pTail == NULL ) {
        #ifdef MY_DEBUG
        sprintf( debug_msg, "pTail == NULL" );
        TRACE_MSG( thisFuncNm, debug_msg );
        TRACE_EXIT( thisFuncNm );
        #endif
        return( rc );
    } /* endif */

    prevFunc = pTail->prev;

    while ( prevFunc != NULL ) {
        #ifdef MY_DEBUG
        sprintf( debug_msg, "strcmp( %s, %s)"
               , pFunc->funcNm, prevFunc->funcNm );
        TRACE_MSG( thisFuncNm, debug_msg );
        #endif
        rc = strcmp( pFunc->funcNm, prevFunc->funcNm );
        if ( rc == 0 ) {
            rc = 1;
            break;
        } /* endif */
        prevFunc = prevFunc->prev;
        rc = 0;
    } /* endwhile */

    #ifdef MY_DEBUG
    sprintf( debug_msg, "rc = %d", rc );
    TRACE_MSG( thisFuncNm, debug_msg );
    TRACE_EXIT( thisFuncNm );
    #endif

    return( rc );
}

void freeUpTreeList( struct funcInfo *pHead )
{
    struct funcInfo *thisFunc = NULL;
    struct funcInfo *nextFunc = NULL;

    thisFunc = pHead;

    while ( thisFunc != NULL ) {
        nextFunc = thisFunc->next;
        free( thisFunc );
        thisFunc = nextFunc;
    } /* endwhile */

    return;
}