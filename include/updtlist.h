
void updateNotCalledFuncList( struct fileInfo *
                            , struct isCalledByFuncInfo * );
void updateAllCalledFuncFileNm( struct fileInfo * );
void updateOneCalledFuncFileNm( struct fileInfo *
                              , char *
                              , char * );
void updateCalledFuncBase( struct fileInfo * );
void updateCircularReferences( struct fileInfo * );
void doTheCircularReferenceUpdate( struct funcInfo *
                                 , struct funcInfo *
                                 , struct funcInfo * 
                                 , struct funcInfo * );
int funcIsInUpTreeList( struct funcInfo *
                      , struct funcInfo * );
void freeUpTreeList( struct funcInfo * );
