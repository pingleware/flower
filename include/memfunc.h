
struct funcInfo *funcInfoMalloc( void );
struct fileInfo *fileInfoMalloc( void );
struct isCalledByFuncInfo *isCalledByMalloc( void );
void initFileInfo( struct fileInfo * );
void initFuncInfo( struct funcInfo * );
void initIsCalledByFuncInfo( struct isCalledByFuncInfo * );
void copyFuncInfo( struct funcInfo *
                 , struct funcInfo * );
