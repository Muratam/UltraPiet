#ifndef EXECUTINGPIETLIBRARIES_H
#define EXECUTINGPIETLIBRARIES_H
#include "piettree.h"
#include <QHash>
#include <functional>

class ExecutingPietLibraries{
public:
//----------------------Utility--------------------------------------
    //(int max) => (int)
    static void random(std::vector<PietTree> & pt);

//----------------------GameLibrary----------------------------------
    //(int w ,int h,QString title) => ()
    static void MakeGLView (std::vector<PietTree> & pt);

    //() => (bool)
    static void GLProcessAll (std::vector<PietTree> & pt);

    //(int x ,int y,int w,int h,int r,int g, int b) => ()
    static void GLDrawRect(std::vector<PietTree> & pt);

    //(QString path) => (int)
    static void GLLoadImage(std::vector<PietTree> & pt);

    //(int handle,int x,int y) => ()
    static void GLDrawImage(std::vector<PietTree> & pt);

    //(int keyCode) => (bool)
    static void GLGetKey(std::vector<PietTree> & pt);

    //(QString path,int Volume) => ()
    static void GLPlayMusic(std::vector<PietTree> & pt);


//---------------------------------------------------------------------
    static QHash<QString,std::function<void(std::vector<PietTree>&)>> functionHash ;
    static bool Hash_HadSet ;
    static void Hash_FuncSet();
private :
    ExecutingPietLibraries(){}
};





#endif // EXECUTINGPIETLIBRARIES_H
