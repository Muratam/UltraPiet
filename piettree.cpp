#include "piettree.h"

void PietTree::MakeStackByMod0(int n,std::vector<PietTree> & pts){
    //nは既にPopされているとして
    if(n <= 0) return ;
    int size = pts.size();
    int min = n < size ? n : size ;
    std::vector<PietTree> NewNode;
    REP(i,min){
        NewNode.push_back(pts[size - 1 - i ]);
        pts.pop_back();
    }
    pts.push_back(PietTree(NewNode));
}

//@show :OK
//isleaf:OK
//isEmptyTree:OK
QString PietTree::showStack() const {
    //return QString("%1 ( ").arg(val) + QString(val) + QString(" )");
    if( isleaf )return QString::number(val);
    QString res("[ ");
    for(auto& n : nodes ){
        res += n.showStack() + ( QString(" , "));
    }
    res += QString(" ]");
    return res;
}

//@Not  : OK
//isleaf: @NOT
//isEmptyTree : OK
int PietTree::Not() const{
   if(isleaf) return val == 0 ? 1 : 0;
   else return isEmptyTree();
}


//@Switch:×
//isleaf:OK
//isEmptyTree:OK
void PietTree::flatten(){
    if(isleaf) return;
    if(isEmptyTree())return;
    std::vector<PietTree> newNodes ;
    for(auto n : nodes){
        if(n.isLeaf()) newNodes.push_back(n);
        else {
            n.flatten();
            for(auto& n2 : n.Nodes()){
                newNodes.push_back(n2);
            }
        }
    }
    nodes = newNodes;
}

//@Point:×
//isleaf:×
//isEmptyTree:OK

PietTree PietTree::popHead(){
    //if(isleaf) //未定義
    if(isEmptyTree()) return nodes;
    PietTree top = nodes[nodes.size()-1];
    nodes.pop_back();
    return top;
}


//@Out(C): OK
//isLeaf : OK
//isEmptyTree:OK
//副作用 : flatten
QString PietTree::toString(){
    if(isleaf)return QString(val);
    flatten();
    QString res("");
    for(auto& n : nodes){
        res += QString(n.Val());
    }
    return res;
}


//@Add          : OK
//isLeaf*isleaf : @ADD
//isLeaf*______ : OK  A + [...]  => [A,...]
//______*isleaf : OK  [...] + A => [...,A]
//______*______ : OK
void PietTree::append(const PietTree& pt){ //@Add
    if(isleaf) {
        if(pt.isLeaf()) {
            val += pt.Val(); return;
        } else {
            isleaf = false;
            nodes.clear();
            nodes.push_back(PietTree(val));
        }
    } else if(pt.isLeaf()) {
        nodes.push_back(PietTree(pt.Val())); return;
    }
    auto an = pt.copyNodes();
    for(auto n : an )  nodes.push_back(n);
}

//未実装
void PietTree::split(const PietTree & pt){ //@Sub
    if(isleaf){
        if(pt.isLeaf()){
            val -= pt.Val(); return;
        }
    }
}

//@Mul          : OK
//isLeaf*isleaf : @MUL
//isLeaf*______ : OK
//______*isleaf : OK
//______*______ : OK
void PietTree::product(const PietTree & pt){
    if(isleaf){
        if(pt.isLeaf()) {
            val *= pt.Val(); return;
        } else {
            isleaf = false;
            nodes.clear();
            nodes.push_back(PietTree(val));
        }
    }
    std::vector<PietTree> HerNodes =
        pt.isLeaf()?
        std::vector<PietTree>{PietTree(pt.Val())}:
        pt.copyNodes();
    std::vector<PietTree> newNodes ;
    for(auto her : HerNodes){
        for(auto my : nodes){
            std::vector<PietTree> New;
            New.push_back(my);
            New.push_back(her);
            newNodes.push_back( PietTree(New));
        }
    }
    nodes = newNodes;
}

//未実装
void PietTree::match(const PietTree &pt){
    if(isleaf){
        if(pt.isLeaf()){
            val /= pt.Val(); return;
        }
    }
}

//@Mod          : OK
//isLeaf*isleaf : @Mod
//isLeaf*______ : OK
//______*isleaf : OK
//______*______ : OK
void PietTree::zip(const PietTree &pt){
    if(isleaf){
        if(pt.isLeaf()) {
            val = val % pt.Val(); return;
        } else {
            isleaf = false;
            nodes.clear();
            nodes.push_back(PietTree(val));
        }
    }
    std::vector<PietTree> HerNodes =
        pt.isLeaf()?
        std::vector<PietTree>{PietTree(pt.Val())}:
        pt.copyNodes();
    int min = nodes.size() > HerNodes.size() ? HerNodes.size() : nodes.size();
    std::vector<PietTree> newNodes ;
    REP(i,min){
        std::vector<PietTree> New;
        New.push_back(nodes[i]);
        New.push_back(pt.Nodes()[i]);
        newNodes.push_back( PietTree(New));
    }
    nodes = newNodes;
}

//未実装 @Greater
void PietTree::loadFile(const PietTree &pt){
    if(isleaf){
        if(pt.isLeaf()){
            val = val < pt.Val() ? 0 : 1 ; return;
        }
    }
     return;
}
