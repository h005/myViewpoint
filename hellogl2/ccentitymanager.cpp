#include "ccentitymanager.h"
#include <iostream>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QStringList>
#include <QSettings>
#include <QDebug>

CCEntityManager::CCEntityManager(const QString path)
{
    cc_configFile = new QFileInfo(path);
    cc_modelFile = NULL;
    cc_imageFile = NULL;
}

CCEntityManager::~CCEntityManager()
{
    delete cc_configFile;
    delete cc_modelFile;
    delete cc_imageFile;
}

bool CCEntityManager::load()
{
    QSettings *settings = new QSettings(cc_configFile->absoluteFilePath(),QSettings::IniFormat);
    // read in model info
    cc_modelFile = new QFileInfo(settings->value("model/path").toString());
    std::cout << cc_modelFile->absoluteFilePath().toStdString() << std::endl;
    if(!cc_modelFile->isFile())
        return false;
    // read in image info
    cc_imageFile = new QFileInfo(settings->value("image/path").toString());
    std::cout << cc_imageFile->absoluteFilePath().toStdString() << std::endl;
    if(!cc_imageFile->isFile())
        return false;
    // read in relation info
    cc_relationFile = new QFileInfo(settings->value("relation/path").toString());
    std::cout << "ccEntityManager load done" << std::endl;
    return true;
}

QString CCEntityManager::modelPath()
{
    if(cc_modelFile)
        return cc_modelFile->absoluteFilePath();
    else
    {
        qDebug() << "cc_model file empty" << endl;
        return "";
    }
}

QString CCEntityManager::imagePath()
{
    if(cc_imageFile)
        return cc_imageFile->absoluteFilePath();
    else
    {
        qDebug() << "cc_imageFile file empty" << endl;
        return "";
    }
}

QString CCEntityManager::relationPath()
{
    if(cc_relationFile)
        return cc_relationFile->absoluteFilePath();
    else
    {
        qDebug() << "cc_relation file empty" << endl;
        return "";
    }
}


