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
    cc_imagesFile = NULL;
    cc_outputFile = NULL;
}

CCEntityManager::~CCEntityManager()
{
    delete cc_configFile;
    delete cc_modelFile;
    delete cc_imageFile;
    delete cc_imagesFile;
    delete cc_outputFile;
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
//    if(!cc_imageFile->isFile())
//        return false;
    // read in relation info
    cc_relationFile = new QFileInfo(settings->value("relation/path").toString());

    cc_imagesFile = new QFileInfo(settings->value("images/path").toString());
    if(!cc_imagesFile->isFile())
        return false;

    cc_imgsDir = settings->value("images/dir").toString();

    cc_outputFile = new QFileInfo(settings->value("images/output").toString());
    // if without assign
    if(!cc_outputFile->isFile())
    {
        // set the same path as imagesFile, named as output.txt
        QString tmpPath = cc_imagesFile->absolutePath();
        std::cout << "tmpPath "<< tmpPath.toStdString() << std::endl;
        delete cc_outputFile;
        cc_outputFile = new QFileInfo(tmpPath+"/output.txt");
    }

    std::cout << cc_imagesFile->absoluteFilePath().toStdString() << std::endl;

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

QString CCEntityManager::imagesFilePath()
{
    if(cc_imagesFile)
        return cc_imagesFile->absoluteFilePath();
    else
    {
        qDebug() << "cc_images file empty" << endl;
        return "";
    }
}

QString CCEntityManager::imgsDir()
{
    return cc_imgsDir;
}

QString CCEntityManager::outputFile()
{
    return cc_outputFile->absoluteFilePath();
}


