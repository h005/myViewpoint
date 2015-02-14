#ifndef IMAGEANDPOINT_H
#define IMAGEANDPOINT_H

#include <QObject>
#include <QLabel>
#include <QString>
#include <glm/glm.hpp>

class QLabel;
class ImageAndPoint : public QLabel
{
    Q_OBJECT
public:
    explicit ImageAndPoint(const QString &imagePath, QWidget *parent = 0);
    ~ImageAndPoint();
    int addPoint(const QPoint &p);
    bool removePoint();
    std::vector<glm::vec2> getPoints();

signals:

public slots:
private:
    QImage *image;
    std::vector<glm::vec2> points;

    void redisplay();
};

#endif // IMAGEANDPOINT_H
