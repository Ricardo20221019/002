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
#include <qgsrubberband.h>
#include <qgsmaplayer.h>
#include <qgspolygon.h>
#include <data.h>
#include <qgsmaptoolidentifyfeature.h>
#include <qgshighlight.h>
#include <qgsmaptooledit.h>
#include <qgssnapindicator.h>
#include <qgscompoundcurve.h>
#include <qgscircularstring.h>
#include <qgsvectorfilewriter.h>

#include "mygdaltool.h"




class MyFeatureTool : public QgsMapTool
{
    Q_OBJECT
public:
    MyFeatureTool(QgsMapCanvas* canvas);
    ~MyFeatureTool();

    void canvasPressEvent( QgsMapMouseEvent* e ) override;
    //! 重写鼠标指针释放事件
    void canvasReleaseEvent( QgsMapMouseEvent* e ) override;

    //鼠标拖动事件
    void canvasMoveEvent( QgsMapMouseEvent *e ) override;


    //添加要素
    int addFeature(QgsVectorLayer* layer,QgsFeature * feature);

    void addLayers(QgsMapLayer* layer,int i);

    void setFindStatus(bool status){finding_status=status;}

    void setMode(DrawingMode mode){tool_status=mode;}

    int getFeatureId(){return select_attributte_id;}//select_attributte_id}

    DrawingMode Mode(){return tool_status;}

    void findCurShortestPath();

    void insertPoint(int index);




    void startCapturing();
    void stopCapturing();
    int getIndex(QgsWkbTypes::GeometryType type);
    void clearHighLight();
    void clearPathHighLight();
    void clearTestHighLight();




    void setPointFeature(int id,PointFeature point_feature);
    void setLineFeature(int id,LineFeature line_feature);
    void setPolognFeature(int id,PolgonFeature area_feature);
    void displayAllFeature();
    void saveAttributeDatas();
    QString listToQString(const QList<int>& value);
    MyGDALTool* getGDALTool(){return gdal_tool;}




signals:
//    void curLayerChange(int index);
    void findPathStartPoint(double x,double y);


public slots:
    void deleteFeature();
    void allClear();


private:
    void addPoint(const QgsPoint &point);
    void addVertex(const QgsPoint &point);
    void drawLineOrCurve();


    QgsRubberBand* createRubberBand(QgsWkbTypes::GeometryType geometryType,bool alternativeBand );
    void deleteTempRubberBand();

    void setHightLight(const QgsFeature & feature);
    void setPathHighLight(std::vector<int> path);
    void setStartTestHighLight(QgsPoint point);
    void setTargetTestHighLight(QgsPoint point);

//    void clearTestHighLight();
    int getCanvasStatus(DrawingMode mode);

    void updateAllPoints(int delete_point,QList<int> delete_lines);
    void updateAllLines(QList<int>delete_lines);
    void updatePointOnLines(QList<int> list,int cur_id,int pre_id);//更新线上的点
    void updateLinesOnPoints();
    void updateDeleteLineOnPoints(int line_id);



private:
    QgsMapCanvas* my_canvas;//tool
    QgsMapToolIdentify *select_Tool;
    MyGDALTool * gdal_tool;
    QgsRubberBand* mTempRubberBand;
    QgsRubberBand* mRubberBand;
    QgsHighlight *highlight=nullptr;
    QgsFeature *select_feature=nullptr;
    std::vector<QgsHighlight*> path_highlight;
    QgsHighlight *start_highlight=nullptr;
    QgsHighlight *target_highlight=nullptr;


    QList<QgsMapLayer *> my_layers;//Cache layers
    QMap<int,QgsVectorLayer *> all_layers;

    QList<QgsPoint> capture_point_List;//Temporary storage
    QVector<int> capture_point_ids;


    QMap<int,PointFeature> all_points;//cache
    QMap<int,LineFeature> all_lines;
    QMap<int,PolgonFeature> all_areas;
    QMap<int,QgsPoint> all_control_points;

    QMap<int,int> temp_change_line;//Temporary buffers
    QMap<int,PointFeature> temp_all_points;
    QMap<int,LineFeature> temp_all_lines;
    QMap<int,PolgonFeature> temp_all_areas;




    CaptureMode m_captureMode;//status
    DrawingMode tool_status;
    bool mCapturing;
    bool moving=false;
    int select_feature_id=-1;
    int select_attributte_id=-1;
    int moving_feature_id=-1;
    QgsWkbTypes::GeometryType select_feature_type;
    bool find_status=false;
    bool finding_status=false;







};

#endif // MYFEATURETOOL_H
