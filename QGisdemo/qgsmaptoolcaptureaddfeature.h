#ifndef QGSMAPTOOLCAPTUREADDFEATURE_H
#define QGSMAPTOOLCAPTUREADDFEATURE_H

#include <qgsmaptoolcapture.h>
#include <qgsmapcanvas.h>
#include <QMouseEvent>
class QgsMapToolCaptureAddFeature : public QgsMapToolCapture
{
public:
    QgsMapToolCaptureAddFeature(QgsMapCanvas* canvas);
    virtual ~QgsMapToolCaptureAddFeature();
    void canvasMapReleaseEvent(QgsMapMouseEvent* e) override;
};

#endif // QGSMAPTOOLCAPTUREADDFEATURE_H
