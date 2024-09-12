
/* Copyright (c) 1996, 1997 Craig Schneiderwent */
/*
Program: flower
File:    printrpt.c
Author:  Craig Schneiderwent
         74631.165@compuserve.com
Date:    20-Apr-1996

Print cross-reference reports, which
functions call which functions, which
functions are called from which functions
and a call tree.

*/

#define INDENT_VALUE    "    \0"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mydebug.h"
#include "structs.h"
#include "mniplist.h"
#include "printrpt.h"

void printBanner( void );
void printSubTree( struct fileInfo *
                 , struct funcInfo *
                 , short
                 , short );
void printFunction( struct funcInfo * );
void printFunctionIndented( struct funcInfo *
                          , short );

void printCalledFuncReport( struct fileInfo *psFileInfo )
{
    struct fileInfo *nextFile = NULL;
    struct funcInfo *nextFunc = NULL;
    struct funcInfo *nextCalledFunc = NULL;
    short           isRecursive = 0;

    nextFile = psFileInfo;

    while ( nextFile != NULL ) {
        printBanner();
        printf( "\n\n---------- %s ----------\n", nextFile->fileNm );
        nextFunc = nextFile->funcListStart;
        while ( nextFunc != NULL ) {
            printf( "\n%s - ", nextFunc->funcNm );
            if ( nextFunc->callsFromMe > 1 || nextFunc->callsFromMe == 0 ) {
                printf( "%ld function calls\n"
                      , nextFunc->callsFromMe );
            } else {
                printf( "%ld function call\n"
                      , nextFunc->callsFromMe );
            } /* endif */
            nextCalledFunc = nextFunc->calledFuncListStart;
            while ( nextCalledFunc != NULL ) {
                printFunctionIndented( nextCalledFunc, 1 );
                isRecursive = !strcmp( nextCalledFunc->funcNm
                                     , nextFunc->funcNm );
                if ( isRecursive ) {
                    printf( " recursive" );
                } /* endif */
                printf( "\n" );
                nextCalledFunc = nextCalledFunc->next;
            } /* endwhile */
            nextFunc = nextFunc->next;
        } /* endwhile */
        nextFile = nextFile->next;
    } /* endwhile */

    return;
}

void printCallingFuncReport( struct isCalledByFuncInfo *psFuncInfo )
{
    struct isCalledByFuncInfo *nextCalledFunc = NULL;
    struct isCalledByFuncInfo *nextCallingFunc = NULL;

    printBanner();

    nextCalledFunc = psFuncInfo;
    if ( nextCalledFunc == NULL ) {
        return;
    } /* endif */
    nextCallingFunc = psFuncInfo->callingFuncListStart;

    while ( nextCalledFunc != NULL ) {
        printf( "\n%s", nextCalledFunc->funcNm );
        if ( strlen( nextCalledFunc->fileNm ) > 0 ) {
            printf( " [%s]", nextCalledFunc->fileNm );
        } /* endif */
        printf( " is called" );
        if ( nextCalledFunc->count > 1 ) {
            printf( " a total of %ld times", nextCalledFunc->count );
        } /* endif */
        printf( " from:\n" );
        while ( nextCallingFunc != NULL ) {
            printf( "%s%s"
                  , INDENT_VALUE
                  , nextCallingFunc->funcNm );
            if ( strlen( nextCallingFunc->fileNm ) > 0 ) {
                printf( " [%s]", nextCallingFunc->fileNm );
            } /* endif */
            if ( nextCallingFunc->count > 1 ) {
                printf( " %ld times", nextCallingFunc->count );
            } /* endif */
            printf( "\n" );
            nextCallingFunc = nextCallingFunc->next;
        } /* endwhile */
        nextCalledFunc = nextCalledFunc->next;
        if ( nextCalledFunc != NULL ) {
            nextCallingFunc = nextCalledFunc->callingFuncListStart;
        } /* endif */
    } /* endwhile */

    return;
}

void printCallTreeReport( struct fileInfo *psFileInfo )
{
    struct fileInfo *nextFile = NULL;
    struct funcInfo *nextFunc = NULL;
    struct funcInfo *nextCalledFunc = NULL;
    short           isRecursive = 0;

    printf( "\n\nCall Tree\n" );

    nextFile = psFileInfo;

    while ( nextFile != NULL ) {
        nextFunc = nextNotCalledFunc( nextFile->funcListStart );
        if ( nextFunc == NULL ) {
            nextFile = nextFile->next;
            continue;
        } /* endif */
        printBanner();
        printf( "\n\n---------- %s ----------\n", nextFile->fileNm );
        while ( nextFunc != NULL ) {
            printf( "\n%s\n", nextFunc->funcNm );
            nextCalledFunc = nextFunc->calledFuncListStart;
            if ( nextCalledFunc != NULL ) {
                isRecursive = !strcmp( nextCalledFunc->funcNm
                                     , nextFunc->funcNm );
                if ( isRecursive ) {
                    printFunctionIndented( nextCalledFunc, 1 );
                    printf( " recursive\n" );
                } else {
                    printSubTree( psFileInfo, nextCalledFunc, 1, 0 );
                } /* endif */
            } /* endif */
            nextFunc = nextNotCalledFunc( nextFunc->next );
        } /* endwhile */
        nextFile = nextFile->next;
    } /* endwhile */

    return;
}

void printSubTree( struct fileInfo *pFileListStart
                 , struct funcInfo *pFunc
                 , short level
                 , short endOfTreeLevel )
{
    struct funcInfo *pNextCalledFunc = NULL;
    struct funcInfo *pNextLevelFuncBase = NULL;
    struct funcInfo *pNextLevelCallListStart = NULL;
    short           endOfTheBranchLevel = 0;

    if ( level < 0 || pFunc == NULL ) {
        return;
    } /* endif */

    pNextCalledFunc = pFunc;

    while ( pNextCalledFunc != NULL ) {
        if ( pNextCalledFunc->next == NULL ) {
            endOfTheBranchLevel = endOfTreeLevel + 1;
        } else {
            endOfTheBranchLevel = 0;
        } /* endif */
        printFunctionIndented( pNextCalledFunc, level );
        pNextLevelFuncBase = pNextCalledFunc->calledFuncBase;
        if ( pNextLevelFuncBase == NULL ) {
            printf( "\n" );
            pNextCalledFunc = pNextCalledFunc->next;
            continue;
        } else {
            pNextLevelCallListStart = 
                   pNextLevelFuncBase->calledFuncListStart;
        } /* endif */
        if ( pNextLevelCallListStart == NULL ) {
            printf( "\n" );
        } else {
            if ( pNextCalledFunc->isCircularReference ) {
                printf( " <circular reference>\n" );
            } else {
                printf( "\n" );
                printSubTree( pFileListStart
                            , pNextLevelCallListStart
                            , (level + 1 )
                            , endOfTheBranchLevel );
            } /* endif */
        } /* endif */
        pNextCalledFunc = pNextCalledFunc->next;
    } /* endwhile */

    return;
}

void printFunction( struct funcInfo *pFunc )
{
    printf( "%s", pFunc->funcNm );
    if ( strlen( pFunc->fileNm ) > 0 ) {
        printf( " [%s]", pFunc->fileNm );
    } /* endif */
    if ( pFunc->callsToMe > 1 ) {
        printf( " %ld times", pFunc->callsToMe );
    } /* endif */

    return;
}

void printFunctionIndented( struct funcInfo *pFunc
                          , short level )
{
    short indent = 0;

    for ( indent = 0; indent < level; indent++ ) {
        printf( "%s", INDENT_VALUE );
    } /* endfor */

    printFunction( pFunc );

    return;
}

void printBanner( void )
{
    time_t aTime;

    time( &aTime );

    printf( "\n\n%s %s\t\t%s\n", MYNAME, MYVERSION, ctime( &aTime ) );

    return;
}

void showSyntax( void )
{
    printf( "%s %s\n", MYNAME, MYVERSION );
    printf( "Syntax:\n\n");
    printf( "\t%s [-abct] file1.c [file2.c] [...] [-x exFunc1 [exFunc2] [...]]\n"
          , MYNAME );
    printf( "  or\n\t%s [-abct] @filelist.txt [-x exFunc1 [exFunc2] [...]]\n"
          , MYNAME );
    printf( "  or\n\t%s [-abct] @filelist.txt [-x @exFileNm]\n\n", MYNAME );
    printf( "Writes a function cross reference to standard output\n" );
    printf( "\nOptions:\n" );
    printf( "\t-a all reports\n" );
    printf( "\t-b functions and which functions call them\n" );
    printf( "\t-c functions and which functions they call\n" );
    printf( "\t-t call tree\n" );
    printf( "\t-x exclude these functions\n\n" );
    printf( "Default is all three reports\n" );

    return;
}
