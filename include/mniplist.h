
int addToFileList ( struct fileInfo *, char * );
int addToFuncList( struct fileInfo *, char * );
int addToCalledFuncList( struct funcInfo *, char * );
int createCalledByFuncList( struct fileInfo *
                          , struct isCalledByFuncInfo * );
int addToCalledByFuncList( struct isCalledByFuncInfo *
                         , struct funcInfo *
                         , struct funcInfo * );
struct isCalledByFuncInfo *endOfCalledByFuncList( struct isCalledByFuncInfo * );
struct isCalledByFuncInfo *startOfCalledByFuncList( struct isCalledByFuncInfo * );
struct isCalledByFuncInfo *funcInCalledByFuncList( char *
                             , struct isCalledByFuncInfo * );
struct isCalledByFuncInfo *callerInList( char *
                         , struct isCalledByFuncInfo * );
void initIsCalledByFuncInfo( struct isCalledByFuncInfo * );
int addToNotCalledFuncList( struct funcInfo **
                          , char *
                          , char * );
void freeLists( struct fileInfo * );
void freeIsCalledByList( struct isCalledByFuncInfo * );
void freeNotCalledList( struct funcInfo * );
struct fileInfo *endOfFileList( struct fileInfo * );
struct fileInfo *startOfFileList( struct fileInfo * );
struct funcInfo *nextNotCalledFunc( struct funcInfo * );
struct funcInfo *funcInFileList( char *
                               , struct fileInfo * );
struct funcInfo *funcInCalledList( char *
                             , struct funcInfo * );
