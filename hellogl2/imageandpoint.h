#ifndef IMAGEANDPOINT_H
#define IMAGEANDPOINT_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <glm/glm.hpp>

class QLabel;
class ImageAndPoint : public QWidget
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
    QLabel *label;
    QImage *image;
    std::vector<glm::vec2> points;
};

#endif // IMAGEANDPOINT_H
