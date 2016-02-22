#ifndef EXECUTINGPIETLIBRARIES_H
#define EXECUTINGPIETLIBRARIES_H
#include "piettree.h"
#include <QHash>
#include <functional>
#include <iostream>
#include <QtWidgets>
#include <glgamewidget.h>
class ExecutingPietLibraries{
public:
    static void A(std::vector<PietTree> & pt){ //空だったり、[[7],""]とかだったりする要素数は、0 または1となる
        int i = pt[0].Val();
        std::cout << i << std::endl;
    }
    static QHash<QString,std::function<void(std::vector<PietTree>&)>> functionHash ;
    static bool Hash_HadSet ;
    static void Hash_FuncSet();
private :
    ExecutingPietLibraries(){}
};





#endif // EXECUTINGPIETLIBRARIES_H
