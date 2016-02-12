#ifndef DEFINES_H
#define DEFINES_H

#define REP(i,n) for(int i = 0 ; i < (int)(n);++i)
#include <QMessageBox>
#define MSGBOX(str) QMessageBox msgBox(this); msgBox.setText(str); msgBox.exec();
#endif // DEFINES_H
