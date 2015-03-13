#ifndef ALIGNRESULTWIDGET_H
#define ALIGNRESULTWIDGET_H

#include <glm/glm.hpp>
#include "glwidget.h"

class AlignResultWidget: public GLWidget {
public:
    AlignResultWidget(PointsMatchRelation *m_relation, const QString &modelPath, int iwidth, int iheight, QWidget *parent = 0);
    ~AlignResultWidget();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
    void paintGL() Q_DECL_OVERRIDE;

protected:
    glm::mat4 m_customProj, m_customMV;
    int m_iwidth, m_iheight;
};

#endif // ALIGNRESULTWIDGET_H
