#ifndef CCSIFTMATCHWINDOW_H
#define CCSIFTMATCHWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QScrollArea>
#include "imglabel.h"
#include "ccsiftmatch.h"
#include "ccmodelwidget.h"

class CCSiftMatchWindow : public QMainWindow
{
    Q_OBJECT
public:
    CCSiftMatchWindow();
    CCSiftMatchWindow(CCSiftMatch *ccsiftMatch,CCModelWidget *ccMW,ImgLabel *imgLabel);
    ~CCSiftMatchWindow();

    QSize sizeHint() const Q_DECL_OVERRIDE;

private slots:
    void reMatch();
    // ok button
    void setRelation();

signals:
    void relationDone();

private:
    ImgLabel *imgMatch;
    QPushButton *reMatchBtn;
    QPushButton *okBtn;
    CCModelWidget *ccMW;
    ImgLabel *imgLabel;

    CCSiftMatch *ccsiftMatch;
    QScrollArea *scrollArea;

};

#endif // CCSIFTMATCHWINDOW_H
