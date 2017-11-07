#ifndef SCATTERPLOT_WIDGET_H
#define SCATTERPLOT_WIDGET_H

#include "../SelectionListener.h"

#include "../graphics/BufferObject.h"
#include "../graphics/Vector2f.h"
#include "../graphics/Vector3f.h"
#include "../graphics/Selection.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>

#include <QMouseEvent>
#include <memory>

enum PointScaling {
    Absolute, Relative
};

namespace hdps
{
namespace gui
{

class ScatterplotWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    void setData(const std::vector<Vector2f>* data);
    void setColors(const std::vector<Vector3f>& data);
    void setPointSize(const float size);
    void setAlpha(const float alpha);
    void setPointScaling(PointScaling scalingMode);
    void addSelectionListener(const plugin::SelectionListener* listener);
protected:
    void initializeGL()         Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL()              Q_DECL_OVERRIDE;

    void mousePressEvent(QMouseEvent *event)   Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event)    Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void onSelection(Selection selection);
    void cleanup();

private:
    Vector2f toClipCoordinates(Vector2f windowCoordinates) const;

    const float        DEFAULT_POINT_SIZE    = 15;
    const float        DEFAULT_ALPHA_VALUE   = 0.5f;
    const PointScaling DEFAULT_POINT_SCALING = PointScaling::Relative;

    GLuint _vao;

    BufferObject _positionBuffer;
    BufferObject _colorBuffer;
    std::unique_ptr<QOpenGLShaderProgram> _shader;

    unsigned int _numPoints = 0;
    const std::vector<Vector2f>* _positions;
    std::vector<Vector3f> _colors;

    QSize _windowSize;
    PointScaling _scalingMode = DEFAULT_POINT_SCALING;
    float _pointSize          = DEFAULT_POINT_SIZE;
    float _alpha              = DEFAULT_ALPHA_VALUE;

    std::unique_ptr<QOpenGLShaderProgram> _selectionShader;
    bool _selecting = false;
    Selection _selection;
    std::vector<const plugin::SelectionListener*> _selectionListeners;
};

} // namespace gui

} // namespace hdps

#endif // SCATTERPLOT_WIDGET_H
