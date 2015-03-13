#ifndef ALIGNRESULTWIDGET_H
#define ALIGNRESULTWIDGET_H

#include <glm/glm.hpp>
#include "glwidget.h"

class AlignResultWidget: public GLWidget {
public:
    AlignResultWidget(const QString &modelPath, float imgRatio, const glm::mat4 &mvMatrix, const glm::mat4 &projMatrix, QWidget *parent = 0);
    ~AlignResultWidget();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
    void paintGL() Q_DECL_OVERRIDE;

protected:
    glm::mat4 m_customProj, m_customMV;
    float m_imgRatio;
};

#endif // ALIGNRESULTWIDGET_H
