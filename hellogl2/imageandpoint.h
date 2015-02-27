#ifndef IMAGEANDPOINT_H
#define IMAGEANDPOINT_H

#include <QObject>
#include <QLabel>
#include <QString>
#include <glm/glm.hpp>

class QLabel;
class PointsMatchRelation;
class ImageAndPoint : public QLabel
{
    Q_OBJECT
public:
    explicit ImageAndPoint(QWidget *parent, const QString &imagePath, PointsMatchRelation &relation);
    ~ImageAndPoint();
    int addPoint(const QPoint &p);
    bool removePoint();

signals:

public slots:
private:
    QImage *image;
    PointsMatchRelation &relation;
    void redisplay();
};

#endif // IMAGEANDPOINT_H
