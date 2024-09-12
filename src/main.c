
/* Copyright (c) 1996, 1997 Craig Schneiderwent */
/*
Program: flower
File:    main.c
Author:  Craig Schneiderwent
         74631.165@compuserve.com
Date:    06-Apr-1996

Mainline processing, algorithm to find
function declarations and calls.

*/


/* My apologies to the non-ASCII folks */
#define CARRIAGE_RETURN 13
#define LINE_FEED       10

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include "mydebug.h"
#include "structs.h"
#include "mniplist.h"
#include "updtlist.h"
#include "memfunc.h"
#include "printrpt.h"

struct exFunc {
    char funcNm[ MAXFUNCNAMESIZE ];
    struct exFunc *next;
    struct exFunc *prev;
} ;

struct flowerOptions {
    short  rqstTreeRpt;
    short  rqstCalledRpt;
    short  rqstCalledByRpt;
    int    file;
    int    fromFile;
    int    nbFiles;
    int    exFuncStrt;
    char   listOfFilesFileNm[ MAXFILENAMESIZE ];
    struct exFunc *excludedFuncs;
} ;

int sussOptions( struct flowerOptions *
               , int
               , char *[] );
int setExFuncListFromFile( struct flowerOptions *
                         , char * );
int addToExFuncList( struct flowerOptions *
                   , char * );
void freeExFuncList( struct flowerOptions * );
int processFiles( FILE *
                , struct fileInfo *
                , struct flowerOptions * );
void cleanUpFileName( char * );
void processTheText( char *
                   , char **
                   , char * 
                   , int  *
                   , struct fileInfo *
                   , struct flowerOptions * );
char *getNextToken( char *, char *, char * );
char *getPrevToken( char *, char * );
int getCalledFunction( char *
                     , char *
                     , char *
                     , struct flowerOptions * );
int getCurrentFunction( char *
                      , char *
                      , char *
                      , struct flowerOptions * );
int fooledByStructOrUnion( char *, char * );
int isReservedWord( char * );
int isReservedSymbol( char * );
int isExcludeFunc( char *, struct flowerOptions * );
int isExcludedFunc( char *
                  , struct flowerOptions * );
void cleanUpFuncName( char * );
int isEmbeddedSymbol( char );

int main( int argc, char *argv[] )
{
    #ifdef MY_DEBUG
    char                      *thisFuncNm = "main\0";
    char                      debug_msg[ MY_DEBUG_MSG_SZ ];
    int                       debug_arg_index = 0;
    #endif
    int                       rc = 0;
    int                       limit = 0;
    int                       file = 0;
    FILE                      *input;
    FILE                      *listOfFiles;
    char                      aFileNm[ MAXFILENAMESIZE ];
    char                      holder[ MAXFILENAMESIZE ];
    struct fileInfo           fileList;
    struct fileInfo           *currentFile = NULL;
    struct isCalledByFuncInfo calledByList;
    struct flowerOptions      options = { 1, 1, 1, 1, 0, 0, 0, {"\0"}, NULL };

    #ifdef MY_DEBUG
    TRACE_ENTRY( thisFuncNm );
    for ( debug_arg_index=0; debug_arg_index < argc; debug_arg_index++ ) {
        sprintf( debug_msg, "argv[ %d ] = %s"
               , debug_arg_index, argv[ debug_arg_index ] );
        TRACE_MSG( thisFuncNm, debug_msg );
    } /* endfor */
    #endif

    if ( argc < 2 ) {
        showSyntax();
        return( 0 );
    } /* endif */

    initFileInfo( &fileList );
    initIsCalledByFuncInfo( &calledByList );

    rc = sussOptions( &options, argc, argv );
    if ( rc != 0 ) {
        fprintf( stderr
               , "\n***** error %d occurred during initialization *****\n"
               , rc );
        return( rc );
    } /* endif */

    strcpy( holder, argv[ options.file ] );

    if ( options.fromFile ) {
        listOfFiles = fopen( options.listOfFilesFileNm, "r" );
        if ( listOfFiles == NULL ) {
            fprintf( stderr, "%s unable to fopen() %s\n"
                   , MYNAME, options.listOfFilesFileNm );
            return( 1 );
        } else {
            while ( !feof( listOfFiles ) ) {
                fgets( aFileNm, MAXFILENAMESIZE, listOfFiles );
                if ( feof( listOfFiles ) ) {
                    break;
                } /* endif */
                options.nbFiles++;
            } /* endwhile */
            rewind( listOfFiles );
        } /* endif */
    } /* endif */

    limit = options.file + options.nbFiles;
    for ( file = options.file; file < limit; file++ ) {
        if ( options.fromFile ) {
            fgets( aFileNm, MAXFILENAMESIZE, listOfFiles );
            cleanUpFileName( aFileNm );
        } else {
            strncpy( aFileNm, argv[ file ], MAXFILENAMESIZE );
        } /* endif */
        input = fopen( aFileNm, "rb" );
        if ( input == NULL ) {
            fprintf( stderr
                   , "\n***** error on fopen() for %s *****\n"
                   , aFileNm );
            return( 1 );
        } /* endif */
        rc = addToFileList( &fileList, aFileNm );
        if ( rc != 0 ) {
            rc = 3;
            fprintf( stderr
                   , "\n***** error %ld occurred processing %s *****\n"
                   , rc, aFileNm );
            break;
        } /* endif */
        currentFile = endOfFileList( &fileList );
        fprintf( stderr, "processing %s\n", aFileNm );
        #ifdef MY_DEBUG
        sprintf( debug_msg, "\tprocessing %s", aFileNm );
        TRACE_MSG( thisFuncNm, debug_msg );
        #endif
        rc = processFiles( input, currentFile, &options );
        if ( rc != 0 ) {
            fprintf( stderr
                   , "\n***** error %ld occurred processing %s *****\n"
                   , rc
                   , aFileNm );
            break;
        } /* endif */
        if ( fclose( input ) ) {
            fprintf( stderr
                   , "\n***** error on fclose() for %s *****\n"
                   , aFileNm );
        } /* endif */
    } /* endfor */

    if ( options.fromFile ) {
        if ( fclose( listOfFiles ) ) {
            fprintf( stderr
                   , "\n***** error on fclose() for %s *****\n"
                   , options.listOfFilesFileNm );
            rc = 1;
        } /* endif */
    } /* endif */

    if ( rc == 0 ) {
        fprintf( stderr, "updating called function file names\n" );
        updateAllCalledFuncFileNm( &fileList );
        fprintf( stderr, "updating called function base pointers\n" );
        updateCalledFuncBase( &fileList );
        fprintf( stderr, "checking for circular references\n" );
        updateCircularReferences( &fileList );
        if ( options.rqstCalledRpt ) {
            fprintf( stderr, "printing called function report\n" );
            printCalledFuncReport( &fileList );
        } /* endif */
        rc = 0;
        if ( options.rqstCalledByRpt || options.rqstTreeRpt ) {
            fprintf( stderr, "creating called-by function list\n" );
            rc = createCalledByFuncList( &fileList, &calledByList );
        } /* endif */
        if ( rc == 0 ) {
            if ( options.rqstCalledByRpt ) {
                fprintf( stderr, "printing calling function report\n" );
                printCallingFuncReport( &calledByList );
            } /* endif */
            fprintf( stderr, "updating not called functions\n" );
            updateNotCalledFuncList( &fileList, &calledByList );
            if ( options.rqstTreeRpt ) {
                fprintf( stderr, "printing call tree report\n" );
                printCallTreeReport( &fileList );
            } /* endif */
            freeExFuncList( &options );
            freeLists( &fileList );
            freeIsCalledByList( &calledByList );
        } else {
            fprintf( stderr
                   , "\n***** error %ld occurred *****\n"
                   , rc );
        } /* endif */
    } /* endif */

    #ifdef __DEBUG_ALLOC__
    _dump_allocated( MAXFUNCNAMESIZE + MAXFILENAMESIZE );
    #endif

    #ifdef MY_DEBUG
    TRACE_EXIT( thisFuncNm );
    #endif

    return( rc );
}

int sussOptions( struct flowerOptions *pOptions
               , int nbParms
               , char *cmdLineOptions[] )
{
    int index = 0;
    int rc = 0;

    if ( cmdLineOptions[ 1 ] [ 0 ] == '-' ) {
        pOptions->rqstTreeRpt = 0;
        pOptions->rqstCalledRpt = 0;
        pOptions->rqstCalledByRpt = 0;
        pOptions->file = 2;
        while ( cmdLineOptions[ 1 ] [ index ] != '\0' ) {
            switch ( cmdLineOptions[ 1 ] [ index ] ) {
            case 'a':
                pOptions->rqstTreeRpt = 1;
                pOptions->rqstCalledRpt = 1;
                pOptions->rqstCalledByRpt = 1;
                break;
            case 't':
                pOptions->rqstTreeRpt = 1;
                break;
            case 'c':
                pOptions->rqstCalledRpt = 1;
                break;
            case 'b':
                pOptions->rqstCalledByRpt = 1;
                break;
            default:
                break;
            } /* endswitch */
            index++;
        } /* endwhile */
    } /* endif */

    pOptions->nbFiles = 0;

    for ( index = 1; index < nbParms; index++ ) {
        switch ( cmdLineOptions[ index ] [ 0 ] ) {
        case '-':
            if ( cmdLineOptions[ index ] [ 1 ] == 'x' ) {
                pOptions->exFuncStrt = index + 1;
            } /* endif */
            break;
        case '@':
            pOptions->file = index;
            pOptions->fromFile = 1;
            strcpy( pOptions->listOfFilesFileNm
                  , ( cmdLineOptions[ index ] ) + 1 );
            break;
        default: 
            if ( !pOptions->fromFile ) {
                pOptions->nbFiles++;
            } /* endif */
            break;
        } /* endswitch */
        if ( pOptions->exFuncStrt ) {
            break;
        } /* endif */
    } /* endfor */

    if ( pOptions->exFuncStrt ) {
        index = pOptions->exFuncStrt;
        if ( index < nbParms ) {
            if ( cmdLineOptions[ index ] [ 0 ] == '@' ) {
                rc = setExFuncListFromFile( pOptions
                                          , cmdLineOptions[ index ] + 1 );
            } else {
                for ( ; index < nbParms; index++ ) {
                    rc = addToExFuncList( pOptions
                                        , cmdLineOptions[ index ] );
                    if ( rc != 0 ) {
                        break;
                    } /* endif */
                } /* endfor */
            } /* endif */
        } /* endif */
    } /* endif */

    return( rc );
}

int setExFuncListFromFile( struct flowerOptions *pOptions
                         , char *aFileNm )
{
    FILE *input = NULL;
    char funcNm[ MAXFUNCNAMESIZE ];
    int  rc = 0;

    input = fopen( aFileNm, "r" );
    if ( input == NULL ) {
        fprintf( stderr
               , "\n***** error on fopen() for %s *****\n"
               , aFileNm );
        return( 1 );
    } /* endif */

    while ( !feof( input ) ) {
        fgets( funcNm, MAXFUNCNAMESIZE, input );
        if ( feof( input ) ) {
            break;
        } /* endif */
        strtok( funcNm, "\n\0" );
        rc = addToExFuncList( pOptions, funcNm );
        if ( rc != 0 ) {
            break;
        } /* endif */
    } /* endwhile */

    fclose( input );
    return( rc );
}

int addToExFuncList( struct flowerOptions *pOptions
                   , char *exFuncNm )
{
    #ifdef MY_DEBUG
    char                      *thisFuncNm = "addToExFuncList\0";
    char                      debug_msg[ MY_DEBUG_MSG_SZ ];
    #endif
    struct exFunc *pExFunc = NULL;
    struct exFunc *pList = NULL;
    struct exFunc *pHold = NULL;

    #ifdef MY_DEBUG
    TRACE_ENTRY( thisFuncNm );
    sprintf( debug_msg, " exFuncNm = %s", exFuncNm );
    TRACE_MSG( thisFuncNm, exFuncNm );
    #endif

    pExFunc = (struct exFunc *) malloc( sizeof( struct exFunc ) );
    if ( pExFunc == NULL ) {
        return( 3 );
    } else {
        memset( pExFunc, '\0', sizeof( struct exFunc ) );
        pExFunc->next = NULL;
        pExFunc->prev = NULL;
        strcpy( pExFunc->funcNm, exFuncNm );
    } /* endif */

    if ( pOptions->excludedFuncs == NULL ) {
        pOptions->excludedFuncs = pExFunc;
        return( 0 );
    } /* endif */

    pList = pOptions->excludedFuncs;

    while ( pList != NULL ) {
        pHold = pList;
        pList = pList->next;
    } /* endwhile */

    pHold->next = pExFunc;
    pExFunc->prev = pHold;

    #ifdef MY_DEBUG
    TRACE_EXIT( thisFuncNm );
    #endif

    return( 0 );
}

void freeExFuncList( struct flowerOptions *pOptions )
{
    struct exFunc *nextFunc = NULL;
    struct exFunc *thisFunc = NULL;

    thisFunc = pOptions->excludedFuncs;

    while ( thisFunc != NULL ) {
        nextFunc = thisFunc->next;
        free( thisFunc );
        thisFunc = nextFunc;
    } /* endwhile */

    return;
}

void cleanUpFileName( char *fileName )
{
    char tempFileName[ MAXFILENAMESIZE ];
    int  i = 0;
    int  j = 0;

    memset( tempFileName, '\0', MAXFILENAMESIZE );

    while ( fileName[ i ] ) {
        if ( !isspace( fileName[ i ] ) ) {
            tempFileName[ j ] = fileName[ i ];
            j++;
        } /* endif */
        i++;
    } /* endwhile */

    strncpy( fileName, tempFileName, MAXFILENAMESIZE );

    return;
}

int processFiles( FILE *input
                , struct fileInfo *currentFile 
                , struct flowerOptions *pOptions )
{
    char          *inputContents;
    char          *currentPosition = NULL;
    int           inFunction = 0;
    long          filePosition;
    unsigned long fileSize;
    char          *endOfContents;

    if ( fseek( input, 0, SEEK_END ) ) {
        fprintf( stderr
               , "%s error attempting fseek( input, 0, SEEK_END )\n"
               , MYNAME );
        return( 2 );
    }

    filePosition = ftell( input );

    if ( fseek( input, 0, SEEK_SET ) ) {
        fprintf( stderr
               , "%s error attempting fseek( input, 0, SEEK_SET )\n"
               , MYNAME );
        return( 2 );
    }

    if ( filePosition > 0 ) {
        fileSize = filePosition;
        inputContents = ( char * ) malloc( fileSize + 1 );
        if ( inputContents == NULL ) {
            fprintf( stderr
                   , "%s error attempting ( char * ) malloc( %ld )\n"
                   , MYNAME
                   , fileSize );
            return( 3 );
        } /* endif */
    } else {
        fprintf( stderr
               , "%s error EOF position ( %ld ) < 0\n"
               , MYNAME
               , filePosition );
        return( 4 );
    } /* endif */

    memset( inputContents, '\0', fileSize + 1 );
    fread( inputContents, sizeof( char ), fileSize, input );
    endOfContents = inputContents + fileSize;
    currentPosition = inputContents;

    while ( currentPosition != NULL && currentPosition < endOfContents ) {
        processTheText( inputContents
                      , &currentPosition
                      , endOfContents
                      , &inFunction
                      , currentFile
                      , pOptions );
    };

    free( inputContents );

    return( 0 );
}

void processTheText( char *inputContents
                   , char **pCurrentPosition
                   , char *endOfContents
                   , int  *pInFunction
                   , struct fileInfo *currentFile
                   , struct flowerOptions *pOptions )
{
    char          *next = NULL;
    char          currentFuncName [ MAXFUNCNAMESIZE ];
    char          calledFuncName [ MAXFUNCNAMESIZE ];
    int           rc = 0;

    next = getNextToken( *pCurrentPosition
                       , inputContents
                       , endOfContents );

    if ( next == NULL ) {
        *pCurrentPosition = next;
        return;
    } /* endif */

    switch ( *next ) {
    case '(':
        if ( *pInFunction == 0 ) {
            break;
        } /* endif */
        if ( getCalledFunction( inputContents
                              , next
                              , calledFuncName
                              , pOptions ) ) {
            if ( strlen( calledFuncName ) > 0 ) {
                addToCalledFuncList( currentFile->funcListEnd
                                   , calledFuncName );
            } /* endif */
        } /* endif */
        break;
    case ')':
        break;
    case '{':
        (*pInFunction)++;
        if ( *pInFunction == 1 ) {
            rc = getCurrentFunction( inputContents
                                   , next
                                   , currentFuncName
                                   , pOptions );
            if ( rc == 0 ) {
                /* we didn't really find a function */
                *pInFunction = 0;
            } else {
                addToFuncList( currentFile, currentFuncName );
            } /* endif */
        } /* endif */
        break;
    case '}':
        if ( *pInFunction > 0 ) {
            (*pInFunction)--;
        } /* endif */
        break;
    default :
        break;
    } /* endswitch */

    *pCurrentPosition = next;

    return;
}

char *getNextToken( char *currentPosition
                  , char *inputContents
                  , char *endOfContents )
{
    /*
    This function returns a pointer to the next
    valid token of interest within inputContents.
    It looks a little strange because we have to
    account for such tokens being inside comments,
    double quotes and single quotes.
    Keep in mind that in this function we're moving
    forward through inputContents.
    */

    char *next;
    int  done = 0;
    int  insideSingleQuotes = 0;
    int  insideDoubleQuotes = 0;
    int  insideComment = 0;

    next = currentPosition + 1;

    while ( !done && next < endOfContents ) {
        switch ( *next ) {
        case '{':
        case '}':
        case '(':
        case ')':
        case '[':
        case ']':
        case ' ':
        case CARRIAGE_RETURN:
        case LINE_FEED:
            if ( insideSingleQuotes || 
                 insideDoubleQuotes || 
                 insideComment ) {
                next++;
            } else {
                done++;
            } /* endif */
            break;
        case '\'':
            if ( !insideComment && !insideDoubleQuotes ) {
                if ( *( next - 1 ) != '\\' ) {
                    insideSingleQuotes = !insideSingleQuotes;
                } /* endif */
            } /* endif */
            next++;
            break;
        case '\"':
            if ( !insideComment && !insideSingleQuotes ) {
                if ( *( next - 1 ) != '\\' ) {
                    insideDoubleQuotes = !insideDoubleQuotes;
                } /* endif */
            } /* endif */
            next++;
            break;
        case '/':
            if ( !insideSingleQuotes && !insideDoubleQuotes ) {
                if ( next + 1 <= endOfContents ) {
                    if ( *( next + 1 ) == '*') {
                        insideComment = 1;
                    } /* endif */
                } /* endif */
            } /* endif */
            next++;
            break;
        case '*':
            if ( !insideSingleQuotes && !insideDoubleQuotes ) {
                if ( next + 1 < endOfContents ) {
                    if ( *( next + 1 ) == '/') {
                        insideComment = 0;
                    } /* endif */
                } /* endif */
                /*
                Added this 'if' to account for source
                code that starts with a comment in position
                one of the first line.  Also gets rid of
                an apparent bug with comments starting
                out (K&R only know why) with the sequence
                slash-splat-slash.
                */
                if ( next - 1 >= inputContents ) {
                    if ( *( next - 1 ) == '/' ) {
                        insideComment = 1;
                    } /* endif */
                } /* endif */
            } /* endif */
            next++;
            break;
        default:
            next++;
            break;
        } /* endswitch */
    } /* endwhile */

    if ( next >= endOfContents ) {
        next = NULL;
    } /* endif */

    return( next );
}

char *getPrevToken( char *inputContents
                  , char *currentPosition )
{
    /*
    This function returns a pointer to the previous
    valid token of interest within inputContents.
    It looks a little strange because we have to
    account for such tokens being inside comments,
    double quotes and single quotes.
    Keep in mind that in this function we're moving
    backward through inputContents.
    */

    char *prev;
    int  done = 0;
    int  insideSingleQuotes = 0;
    int  insideDoubleQuotes = 0;
    int  insideComment = 0;

    prev = currentPosition - 1;

    while ( !done && prev >= inputContents ) {
        switch ( *prev ) {
        case '(':
        case ')':
        case '{':
        case '}':
        case '[':
        case ']':
        case ' ':
        case ',':
        case ';':
        case '|':
        case '&':
        case '=':
        case CARRIAGE_RETURN:
        case LINE_FEED:
            if ( insideSingleQuotes || 
                 insideDoubleQuotes || 
                 insideComment ) {
                prev--;
            } else {
                done++;
            } /* endif */
            break;
        case '\'':
            if ( !insideComment && !insideDoubleQuotes ) {
                if ( prev != currentPosition ) {
                    if ( *( prev + 1 ) != '\\' ) {
                        insideSingleQuotes = !insideSingleQuotes;
                    } /* endif */
                } else {
                    insideSingleQuotes = !insideSingleQuotes;
                } /* endif */
            } /* endif */
            prev--;
            break;
        case '\"':
            if ( !insideComment && !insideSingleQuotes ) {
                if ( prev != currentPosition ) {
                    if ( *( prev + 1 ) != '\\' ) {
                        insideDoubleQuotes = !insideDoubleQuotes;
                    } /* endif */
                } else {
                    insideDoubleQuotes = !insideDoubleQuotes;
                } /* endif */
            } /* endif */
            prev--;
            break;
        case '/':
            if ( !insideSingleQuotes && !insideDoubleQuotes ) {
                if ( prev != currentPosition ) {
                    if ( *( prev - 1 ) == '*') {
                        insideComment = 1;
                    } /* endif */
                } /* endif */
            } /* endif */
            prev--;
            break;
        case '*':
            if ( !insideSingleQuotes && !insideDoubleQuotes ) {
                if ( prev != currentPosition ) {
                    if ( *( prev - 1 ) == '/') {
                        insideComment = 0;
                    } /* endif */
                } /* endif */
            } /* endif */
            prev--;
            break;
        default:
            prev--;
            break;
        } /* endswitch */
    } /* endfor */

    if ( prev < inputContents ) {
        prev = NULL;
    } /* endif */

    return( prev );
}

int getCalledFunction( char *inputContents
                     , char *next
                     , char *calledFuncName
                     , struct flowerOptions *pOptions )
{
    /*
    This function gets a string which may be
    a called function name.  It then matches the
    string against a list of reserved words which
    look like functions to the basic algorithm.  If
    the string is not found or is a reserved word 
    0 is returned, otherwise 1 is returned.
    */

    #ifdef MY_DEBUG
    char            *thisFuncNm = "getCalledFunction\0";
    #endif
    char *prev = NULL;
    char *funcStart = NULL;
    int  rc = 0;

    #ifdef MY_DEBUG
    TRACE_ENTRY( thisFuncNm );
    #endif

    memset( calledFuncName, '\0', MAXFUNCNAMESIZE );
    prev = getPrevToken( inputContents
                       , next );
    if ( prev == NULL ) {
        return( 0 );
    } /* endif */

    funcStart = prev;

    if ( (next - prev) < 2 ) {
        while ( funcStart != NULL && *funcStart == ' ' ) {
            prev = funcStart;
            if ( *prev == ',' ||
                 *prev == '|' ||
                 *prev == '=' ||
                 *prev == '&' ) {
                break;
            } /* endif */
            funcStart = getPrevToken( inputContents
                                    , funcStart );
        } /* endwhile */
    } /* endif */

    if ( prev == NULL ) {
        return( 0 );
    } /* endif */

    if ( *prev == ',' ||
         *prev == '|' ||
         *prev == '=' ||
         *prev == '&' ) {
        /* inside if, function args, cast, for or while */
        prev++;
    } /* endif */

    if ( (next - prev) > 1 && (next - prev) < MAXFUNCNAMESIZE ) {
        memcpy( calledFuncName, prev + 1, (next - prev) - 1 );
        cleanUpFuncName( calledFuncName );
        if ( !isReservedWord( calledFuncName ) &&
             !isReservedSymbol( calledFuncName ) &&
             !isExcludedFunc( calledFuncName, pOptions ) ) {
            rc = 1;
        } /* endif */
    } /* endif */

    #ifdef MY_DEBUG
    TRACE_EXIT( thisFuncNm );
    #endif

    return( rc );
}

int getCurrentFunction( char *inputContents
                      , char *next
                      , char *currentFuncName
                      , struct flowerOptions *pOptions )
{
    /*
    This function gets a string which is the
    current function name.  If the string is
    longer than 1 byte and shorter than the
    maximum allowable length 1 is returned,
    otherwise 0 is returned.  0 is also returned if
    we've been fooled by a struct or union
    declaration.
    */

    #ifdef MY_DEBUG
    char            *thisFuncNm = "getCurrentFunction\0";
    char            debug_msg[ MY_DEBUG_MSG_SZ ];
    #endif
    char *prev = NULL;
    char *funcStart = NULL;
    int  rc = 0;
    int  inParens = 0;

    #ifdef MY_DEBUG
    TRACE_ENTRY( thisFuncNm );
    sprintf( debug_msg, " next = %p", next );
    TRACE_MSG( thisFuncNm, debug_msg );
    #endif

    memset( currentFuncName
          , '\0'
          , MAXFUNCNAMESIZE );

    prev = getPrevToken( inputContents
                       , next );

    do {
        /* get to the outermost parentheses */
        if ( *prev == ')' ) {
            if ( inParens == 0 ) {
                /* outermost closing paren */
                if ( fooledByStructOrUnion( next, prev ) ) {
                    #ifdef MY_DEBUG
                    TRACE_MSG( thisFuncNm, "fooled by struct or union\0" );
                    TRACE_EXIT( thisFuncNm );
                    #endif
                    return( 0 );
                } /* endif */
            } /* endif */
            inParens++;
        } /* endif */
        if ( *prev == '(' ) {
            if ( inParens ) {
                inParens--;
                if ( inParens == 0 ) {
                    break;
                } /* endif */
            } /* endif */
        } /* endif */
        prev = getPrevToken( inputContents, prev );
    } while ( prev != NULL ) ;

    if ( prev >= inputContents ) {
        funcStart = getPrevToken( inputContents, prev );
        if ( *funcStart == ',' ) {
            /*
            fooled by a structure initialization,
            probably some evil global variable
            */
            #ifdef MY_DEBUG
            TRACE_MSG( thisFuncNm, "fooled by structure initialization\0" );
            TRACE_EXIT( thisFuncNm );
            #endif
            return( 0 );
        } /* endif */
        while ( funcStart != NULL && (prev - funcStart == 1) ) {
            /* 
            This "while" is here because apparently
            SOME people think it's a good idea to put
            white space between the name of a function
            and the opening parenthesis denoting the
            beginning of the argument list.  You know
            who you are... and so do we.
            */
            if ( *funcStart == ',' ) {
                /* fooled by a structure initialization */
                #ifdef MY_DEBUG
                TRACE_MSG( thisFuncNm, "fooled by structure initialization\0" );
                TRACE_EXIT( thisFuncNm );
                #endif
                return( 0 );
            } /* endif */
            prev = funcStart;
            funcStart = getPrevToken( inputContents
                                    , funcStart );
        } /* endwhile */
        if ( funcStart != NULL && 
             (prev - funcStart) < MAXFUNCNAMESIZE ) {
            memcpy( currentFuncName
                  , funcStart + 1
                  , (prev - funcStart) - 1 );
            cleanUpFuncName( currentFuncName );
            if ( !isReservedWord( currentFuncName )  &&
                 !isReservedSymbol( currentFuncName ) &&
                 !isExcludedFunc( currentFuncName, pOptions ) ) {
                rc = 1;
            } /* endif */
            if ( strlen( currentFuncName ) == 0 ) {
                rc = 0;
            } /* endif */
        } /* endif */
    } /* endif */

    #ifdef MY_DEBUG
    TRACE_EXIT( thisFuncNm );
    #endif

    return( rc );
}

int fooledByStructOrUnion( char *curlyBrace, char *paren )
{
    /*
    This function indicates whether or not
    the algorithm has been fooled by a struct
    or union definition, indicated by there
    being something besides whitespace between
    the closing paren of the function declaration
    and the next semicolon.
    */

    char   *someText = NULL;
    char   *nextSemicolon = NULL;
    size_t someTextLen = 0;
    size_t i = 0;
    int    rc = 1;

    if ( paren == NULL ) {
        return( 0 );
    } /* endif */

    nextSemicolon = strstr( paren, ";\0" );

    if ( nextSemicolon == NULL ) {
        return( 0 );
    } /* endif */

    if ( curlyBrace < nextSemicolon ) {
        return( 0 );
    } /* endif */

    someTextLen = nextSemicolon - paren;

    if ( someTextLen < 2 ) {
        return( 1 );
    } /* endif */

    someText = ( char * ) malloc( someTextLen );
    if ( someText == NULL ) {
        return( 1 );
    } /* endif */

    memset( someText, '\0', someTextLen );
    memcpy( someText, paren + 1, someTextLen - 1 );

    while ( someText[ i ] ) {
        if ( !isspace( someText[ i ] ) ) {
            rc = 0;
            break;
        } /* endif */
        i++;
    } /* endwhile */

    free( someText );
    return( rc );
}

int isReservedWord( char *funcName )
{
    /*
    This function returns 1 if the passed
    funcName equals any of the reserved
    words listed.  Feel free to add your
    own or trade them with friends.
    */

    if ( !strcmp( "if\0", funcName ) ) {
        return( 1 );
    } /* endif */

    if ( !strcmp( "while\0", funcName ) ) {
        return( 1 );
    } /* endif */

    if ( !strcmp( "switch\0", funcName ) ) {
        return( 1 );
    } /* endif */

    if ( !strcmp( "return\0", funcName ) ) {
        return( 1 );
    } /* endif */

    if ( !strcmp( "for\0", funcName ) ) {
        return( 1 );
    } /* endif */

    if ( !strcmp( "exit\0", funcName ) ) {
        return( 1 );
    } /* endif */

    if ( !strcmp( "case\0", funcName ) ) {
        return( 1 );
    } /* endif */

    if ( !strcmp( "sizeof\0", funcName ) ) {
        return( 1 );
    } /* endif */

    if ( !strcmp( "offsetof\0", funcName ) ) {
        return( 1 );
    } /* endif */

    return( 0 );
}

int isReservedSymbol( char *funcName )
{
    if ( !strcmp( "*\0", funcName ) ) {
        return( 1 );
    } /* endif */

    if ( !strcmp( "!\0", funcName ) ) {
        return( 1 );
    } /* endif */

    if ( !strcmp( "&\0", funcName ) ) {
        return( 1 );
    } /* endif */

    if ( !strcmp( "&&\0", funcName ) ) {
        return( 1 );
    } /* endif */

    if ( !strcmp( "||\0", funcName ) ) {
        return( 1 );
    } /* endif */

    if ( !strcmp( "[\0", funcName ) ) {
        return( 1 );
    } /* endif */

    if ( !strcmp( "]\0", funcName ) ) {
        return( 1 );
    } /* endif */

    if ( !strcmp( "-\0", funcName ) ) {
        return( 1 );
    } /* endif */

    if ( !strcmp( "--\0", funcName ) ) {
        return( 1 );
    } /* endif */

    if ( !strcmp( "+\0", funcName ) ) {
        return( 1 );
    } /* endif */

    if ( !strcmp( "++\0", funcName ) ) {
        return( 1 );
    } /* endif */

    return( 0 );
}

int isExcludedFunc( char *funcName
                  , struct flowerOptions *pOptions )
{
    #ifdef MY_DEBUG
    char            *thisFuncNm = "isExcludedFunc\0";
    #endif
    struct exFunc *pList = NULL;

    #ifdef MY_DEBUG
    TRACE_ENTRY( thisFuncNm );
    TRACE_MSG( thisFuncNm, funcName );
    #endif

    pList = pOptions->excludedFuncs;

    while ( pList != NULL ) {
        if ( !strcmp( pList->funcNm, funcName ) ) {
            #ifdef MY_DEBUG
            TRACE_MSG( thisFuncNm, "rc = 1\0" );
            TRACE_EXIT( thisFuncNm );
            #endif
            return( 1 );
        } /* endif */
        pList = pList->next;
    } /* endwhile */

    #ifdef MY_DEBUG
    TRACE_MSG( thisFuncNm, "rc = 0\0" );
    TRACE_EXIT( thisFuncNm );
    #endif

    return( 0 );
}

void cleanUpFuncName( char *funcName )
{
    /*
    This function gets rid of characters
    that may be "stuck to" the function
    name.  For instance, a developer may
    write "if ( !strncmp..." and we don't
    want to include the ! character when
    outputting the function names.
    */

    #ifdef MY_DEBUG
    char            *thisFuncNm = "cleanUpFuncName\0";
    #endif
    char cleanedFuncName[ MAXFUNCNAMESIZE ];
    int  i = 0;
    int  j = 0;

    #ifdef MY_DEBUG
    TRACE_ENTRY( thisFuncNm );
    TRACE_MSG( thisFuncNm, funcName );
    #endif

    memset( cleanedFuncName
          , '\0'
          , MAXFUNCNAMESIZE );

    while ( funcName[ i ] != 0 ) {
        if ( !isspace( funcName[ i ] ) && 
             !isEmbeddedSymbol( funcName[ i ] ) ) {
            cleanedFuncName[ j ] = funcName[ i ];
            j++;
        } /* endif */
        i++;
    } /* endwhile */

    strncpy( funcName, cleanedFuncName, MAXFUNCNAMESIZE );

    #ifdef MY_DEBUG
    TRACE_MSG( thisFuncNm, cleanedFuncName );
    TRACE_EXIT( thisFuncNm );
    #endif

    return;
}

int isEmbeddedSymbol( char aChar )
{
    /*
    This function returns a true if aChar is a
    character that may be "stuck to" the function 
    name, but aren't actually part of the 
    function name.
    */

    char *symbolList = ".!%^&*=+|/? \'\"\0";

    if ( strchr( symbolList, aChar ) != NULL ) {
        return( 1 );
    } /* endif */

    return( 0 );
}
