#include "ccsiftmatchwindow.h"
#include <QVBoxLayout>

CCSiftMatchWindow::CCSiftMatchWindow()
{

}

CCSiftMatchWindow::CCSiftMatchWindow(CCSiftMatch *ccsiftMatch,CCModelWidget *ccMW,ImgLabel *imgLabel)
{
    this->imgLabel = imgLabel;
    this->ccMW = ccMW;
    this->ccsiftMatch = ccsiftMatch;
    reMatchBtn = new QPushButton(tr("reMatch"),this);
    okBtn = new QPushButton(tr("OK"),this);

    connect(reMatchBtn,SIGNAL(clicked()),this,SLOT(reMatch()));
    connect(okBtn,SIGNAL(clicked()),this,SLOT(setRelation()));

    imgMatch = new ImgLabel(ccsiftMatch,ccMW);
    scrollArea = new QScrollArea();


    QSizePolicy cellPolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
    cellPolicy.setHorizontalStretch(1);
    scrollArea->setSizePolicy(cellPolicy);

    scrollArea->setWidget(imgMatch);

    // 左边一列放按钮
    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(okBtn);
    leftLayout->addWidget(reMatchBtn);


    QHBoxLayout *container = new QHBoxLayout;
    container->addLayout(leftLayout);
    container->addWidget(scrollArea);

    QWidget *w = new QWidget;
    w->setLayout(container);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(w);
    setLayout(mainLayout);

    setCentralWidget(w);
    setWindowTitle(tr("points match"));
}

CCSiftMatchWindow::~CCSiftMatchWindow()
{
    delete imgMatch;
    delete reMatchBtn;
    delete scrollArea;
}

QSize CCSiftMatchWindow::sizeHint() const
{
    return QSize(1024,1024);
}

void CCSiftMatchWindow::reMatch()
{
    ccsiftMatch->reMatch();
//    imgMatch->update();
    imgMatch->updateImg();
}

void CCSiftMatchWindow::setRelation()
{
    std::vector<int> index;
    ccMW->setPoints(ccsiftMatch->getModelPoints(),index);
    imgLabel->setPoints(ccsiftMatch->getImagePoints(),index);
    this->close();
}


