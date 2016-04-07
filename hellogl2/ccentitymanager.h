#ifndef CCENTITYMANAGER_H
#define CCENTITYMANAGER_H

#include <QString>
#include <QFileInfo>

class CCEntityManager
{
public:
    CCEntityManager(const QString path);
    ~CCEntityManager();
    bool load();

    QString modelPath();
    QString imagePath();
    QString relationPath();
    QString imagesFilePath();
    QString imgsDir();
    QString outputFile();

private:
    QFileInfo *cc_configFile;
    QFileInfo *cc_modelFile;
    QFileInfo *cc_imageFile;
    QFileInfo *cc_relationFile;
    QFileInfo *cc_imagesFile;
    QFileInfo *cc_outputFile;
    QString cc_imgsDir;

};

#endif // CCENTITYMANAGER_H
