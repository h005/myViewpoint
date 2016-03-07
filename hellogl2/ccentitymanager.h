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

private:
    QFileInfo *cc_configFile;
    QFileInfo *cc_modelFile;
    QFileInfo *cc_imageFile;
    QFileInfo *cc_relationFile;

};

#endif // CCENTITYMANAGER_H
