#ifndef MYADDFEATURETOOL_H
#define MYADDFEATURETOOL_H

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


#include "data.h"
//#include <

class MyAddFeatureTool : public QgsMapTool
{
public:
    MyAddFeatureTool(QgsMapCanvas* canvas);
    ~MyAddFeatureTool();

    void canvasPressEvent(QgsMapMouseEvent * e) override;
    //! 重写鼠标指针释放事件
    void canvasReleaseEvent( QgsMapMouseEvent* e ) override;

    //! 添加要素函数，传入 QgsVectorLayer 指针、当前要素，以及是否实时显示
    bool addFeature( QgsVectorLayer *vlayer, QgsFeature *f, bool showModal = true );

    //激活工具
    void activate() override;

    //获取捕获状态
    CaptureMode getMode();

    int Size(){return captureList.size();}
    const QList<QgsPoint>& Points(){return captureList;}

private:

    void notifyNotVectorLayer();
    void notifyNotEditableLayer();
    int addVertex( const QgsPoint& point );
    int nextPoint( const QgsPoint& mapPoint, QgsPoint& layerPoint );
    QgsRubberBand* createRubberBand( QgsWkbTypes::GeometryType type,bool alternativeBand);//QgsGeometry::type()=QgsWkbTypes::LineString, bool alternativeBand = false


    void startCapturing();
    void stopCapturing();
    void deleteTempRubberBand();


    CaptureMode m_captureMode;
    bool mCapturing;
    /** rubber band for polylines and polygons */
    QgsRubberBand* mRubberBand;

    /** temporary rubber band for polylines and polygons. this connects the last added point to the mouse cursor position */
    QgsRubberBand* mTempRubberBand;

    QString mTip;
    QgsGeometryValidator *mValidator;
    QList< QgsGeometry::Error > mGeomErrors;
    QList< QgsVertexMarker * > mGeomErrorMarkers;

    bool mCaptureModeFromLayer;

    void validateGeometry();

    QgsVertexMarker* mSnappingMarker;

    QList<QgsPoint> captureList;


};

#endif // MYADDFEATURETOOL_H
