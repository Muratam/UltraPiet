#ifndef LOADPIETDLL_H
#define LOADPIETDLL_H
#include <QString>
#include <vector>

class PietTree;

class LoadPietDLL{
public:
    static PietTree LoadDLL(bool& Miss ,QString dllname,QString funcname,QString TypeNames,std::vector<PietTree>& pts);
private:
    LoadPietDLL(){}
};

#endif // LOADPIETDLL_H
