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
    explicit ImageAndPoint(const QString &imagePath, PointsMatchRelation &relation, QWidget *parent = 0);
    ~ImageAndPoint();
    int addPoint(const QPoint &p);
    bool removePoint();
    void redisplay();

signals:

public slots:
private:
    QImage *image;
    PointsMatchRelation &relation;
};

#endif // IMAGEANDPOINT_H
