#ifndef MYFEATURETOOL_H
#define MYFEATURETOOL_H


#include <qgis.h>
#include <qgsgeometry.h>
#include <qgsmaptool.h>
#include <qgsmapcanvas.h>
#include <QMouseEvent>
#include <qgsmapmouseevent.h>
#include <qgsvertexmarker.h>
#include <qgsgeometryvalidator.h>
#include <qgsmessagebar.h>
//QgsRubberBand
#include <qgsrubberband.h>
//QgsMapLayerRegistry
#include <qgsmaplayer.h>
#include <qgspolygon.h>
#include <data.h>
#include <qgsmaptoolidentifyfeature.h>
class MyFeatureTool : public QgsMapTool
{
public:
    MyFeatureTool(QgsMapCanvas* canvas);
    ~MyFeatureTool();

//    void cnavasPressEvent()
    void canvasPressEvent( QgsMapMouseEvent* e ) override;
    //! 重写鼠标指针释放事件
    void canvasReleaseEvent( QgsMapMouseEvent* e ) override;

    //添加要素
    int addFeature(QgsVectorLayer* layer,QgsFeature * feature);

    CaptureMode Mode(){return m_captureMode;}

    void startCapturing();
    void stopCapturing();


public slots:
    void deleteFeature(QgsFeatureId id);

private:
    void addVertex(const QgsPoint &point);
    int nextPoint(const QgsPoint &point);

    QgsRubberBand* createRubberBand(QgsWkbTypes::GeometryType geometryType,bool alternativeBand );
    void deleteTempRubberBand();


private:
    QgsMapCanvas* my_canvas;

    std::map<int,QgsMapToolIdentifyFeature*>my_identifyTool;
    QList<QgsPoint> capture_point_List;
    QList< QgsVertexMarker * > mGeomErrorMarkers;


    CaptureMode m_captureMode;
    bool mCapturing;

    QgsRubberBand* mRubberBand;


    QgsRubberBand* mTempRubberBand;
};

#endif // MYFEATURETOOL_H
