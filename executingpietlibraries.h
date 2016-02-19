#ifndef EXECUTINGPIETLIBRARIES_H
#define EXECUTINGPIETLIBRARIES_H
#include "piettree.h"

class ExecutingPietLibraries{
public:
    static PietTree LoadDLL(bool& Miss ,QString dllname,QString funcname,QString TypeNames,std::vector<PietTree>& pts);
    ExecutingPietLibraries();
};

#endif // EXECUTINGPIETLIBRARIES_H
