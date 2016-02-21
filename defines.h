#ifndef DEFINES_H
#define DEFINES_H

#define REP(i,n) for(int i = 0 ; i < (int)(n);++i)

#include <QMessageBox>
#define MSGBOX(STR) {    QMessageBox msgBox(this); msgBox.setText(STR); msgBox.exec();}
//auto MSGBOX = [] (QString str, QWidget* parent) {    QMessageBox msgBox(parent); msgBox.setText(str); msgBox.exec();};

#endif // DEFINES_H
