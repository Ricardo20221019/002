#ifndef MYFEATURETOOL_H
#define MYFEATURETOOL_H

#include <QObject>
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
#include <qgshighlight.h>
#include <qgsmaptooledit.h>
#include <qgssnapindicator.h>
#include <qgscompoundcurve.h>
//QgsCircularString
//QgsMapToolVertexEditor
#include <qgscircularstring.h>





class MyFeatureTool : public QgsMapTool
{
    Q_OBJECT
public:
    MyFeatureTool(QgsMapCanvas* canvas);
    ~MyFeatureTool();

    void initAllVertexs();
//    void cnavasPressEvent()
    void canvasPressEvent( QgsMapMouseEvent* e ) override;
    //! 重写鼠标指针释放事件
    void canvasReleaseEvent( QgsMapMouseEvent* e ) override;

    //鼠标拖动事件
    void canvasMoveEvent( QgsMapMouseEvent *e ) override;


    //添加要素
    int addFeature(QgsVectorLayer* layer,QgsFeature * feature);

    void addCurve(QgsPoint point);



    void addLayers(QgsMapLayer* layer,int i);

    void setMode(DrawingMode mode){tool_status=mode;}
    void catFeatureAtrribute(const QgsFeature &feature,QString des);

    int getFeatureId(){return select_attributte_id;}//select_attributte_id}

    void getALLFeaturesId();

    int getMaxAttributeId(const QList<int>&);

    DrawingMode Mode(){return tool_status;}

    void startCapturing();
    void stopCapturing();
    int getIndex(QgsWkbTypes::GeometryType type);
    void clearHighLight();


    void setPointFeature(int id,PointFeature point_feature);
    void setLineFeature(int id,LineFeature line_feature);
    void setPolognFeature(int id,PolgonFeature area_feature);
    void disAllFeature();
    void saveAttributeDatas();
    QString listToQString(const QList<int>& value);



signals:
//    void curLayerChange(int index);

public slots:
    void deleteFeature();
    void allClear();


private:
    void addPoint(const QgsPoint &point);

    void addVertex(const QgsPoint &point);
    void drawLineOrCurve();
    int nextPoint(const QgsPoint &point);
    void deleteSingleFeature();
    void getFeatureContainsId(QgsWkbTypes::GeometryType geometry_type,int id_);
    int getPointFeatureId(int id_);
    void getFeatureContainsId2(QgsWkbTypes::GeometryType geometry_type,int id_);

    QgsRubberBand* createRubberBand(QgsWkbTypes::GeometryType geometryType,bool alternativeBand );
    void deleteTempRubberBand();

    void setHightLight(const QgsFeature & feature);
    int getCanvasStatus(DrawingMode mode);
    void updateAllFeatureIds(QgsWkbTypes::GeometryType,int delete_id);
    void updateAllLinesVertexIds(int change_id);
    void updateAllPointsIds(int change_id);




    void updateAllPoints(int delete_point,QList<int> delete_lines);
    void updateAllLines(QList<int>delete_lines);
    void updatePointOnLines(QList<int> list,int cur_id,int pre_id);//更新线上的点
    void updateLinesOnPoints();
    void updateDeleteLineOnPoints(int line_id);

//    void deleteLineOnPoint(int line_id);
//    int toAttributeGetFeatureId(int attribute);


private:
    QgsMapCanvas* my_canvas;

    std::map<int,QgsMapToolIdentifyFeature*>my_identifyTool;
    QgsMapToolIdentify *select_Tool;



    QList<QgsPoint> capture_point_List;
    QVector<int> capture_point_ids;


    QList< QgsVertexMarker * > mGeomErrorMarkers;
    QList<QgsMapLayer *> my_layers;
    QList<int> all_points_attribute_ids;
    QList<int> all_lints_attribute_ids;
    QMap<int,QgsVectorLayer *> all_layers;
    QMap<QgsWkbTypes::GeometryType,QMap<int,QgsFeature*>> all_features;
    QMap<int,QPair<int,int>> all_line_vertexs_ids;
    QMap<int,QgsPoint> all_points_id;//featureid xy
//    QMap<int,QgsFeature*> all_vertexs;
    QMap<int,QList<int>> all_line_vertexs;//int：line_id，int：vertex_id
    QMap<int,QList<int>> all_virtual_vertexs;//int：line_id，int：vertex_id
    QMap<int,QgsFeature*> current_all_features;

    QMap<QgsWkbTypes::GeometryType,QList<QgsFeatureId>> all_ids;


    QMap<int,PointFeature> all_points;
    QMap<int,LineFeature> all_lines;
    QMap<int,PolgonFeature> all_areas;
//    QMap<int,int> all_control_points;
    QMap<int,QgsPoint> all_control_points;

    QMap<int,int> temp_change_line;//pre cur

    QMap<int,PointFeature> temp_all_points;
    QMap<int,LineFeature> temp_all_lines;

    QMap<int,PolgonFeature> temp_all_areas;

    QList<QgsPoint>curve_points;



    CaptureMode m_captureMode;
    DrawingMode tool_status;
    bool mCapturing;
    bool moving=false;
    int select_feature_id=-1;
    int select_attributte_id=-1;
    int moving_feature_id=-1;
    QgsWkbTypes::GeometryType select_feature_type;

    QgsRubberBand* mRubberBand;
    QgsHighlight *highlight=nullptr;
    QgsFeature *select_feature=nullptr;


    QgsRubberBand* mTempRubberBand;
//    QgsMapToolVertexEditor *too;

//    QgsMapToolEdit * tool_edit;
//    QgsVertexTool
};

#endif // MYFEATURETOOL_H
