#ifndef QGSGRAPHMAP_H
#define QGSGRAPHMAP_H

#include <QWidget>
#include <qgis.h>
#include <qgsapplication.h>
#include <qgsmapcanvas.h>
#include <QGridLayout>
#include <qgsrasterlayer.h>
#include <qgsvectorfilewriter.h>
#include <qgsproviderregistry.h>
#include <qgssinglesymbolrenderer.h>
#include <qgssimplelinematerialsettings.h>
#include <qgsgeometry.h>
#include <qgsmaptoolidentify.h>
#include <qgsvertexmarker.h>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <qgssymbol.h>
#include <qgsfillsymbollayer.h>
#include <qgslinesymbollayer.h>
//QgsMapToolIdentifyFeature
#include <qgsmaptoolidentifyfeature.h>
#include <qgshighlight.h>
//QgsMapToolEmitPoint
#include <qgsmaptoolemitpoint.h>
//QgsMapToolPan
#include <qgsmaptoolpan.h>
#include <qgsmaptoolcapture.h>
#include <qgsmaptooledit.h>
#include <qgsmaptooladvanceddigitizing.h>
#include <QMouseEvent>
#include <QGraphicsView>
#include <memory>
#include <map>
#include "qgssymbollayer.h"
#include "data.h"

#include "myfeaturetool.h"

class QgsGraphMap : public QWidget
{
    Q_OBJECT
public:
    explicit QgsGraphMap(QWidget *parent);

    void initalive();
    QgsMapLayer* SetPointsData();
    QgsMapLayer* SetLinesData();
    void GetNetData();
    void RenderingLayers(QColor color_,double size_,QgsVectorLayer* points);
//    void Rendering(QColor lines_color,QColor points_color,double line_size,double point_size,QgsVectorLayer* layers);
    void SaveShp(QString dir_path,QString filename,QgsVectorLayer* layers);



signals:
    void deletefeature();
    void allClearFeatures();
protected:
    void mousePressEvent(QMouseEvent *event) override
    {

    }
public slots:
//    void MousePressEvent(QMouseEvent *event);
     void Show_Coordinate(const QgsPointXY &p);
     void setToolStatus(int index);
     void deleteFeature();
     void allClear();
     void updateAtrribute();

     void saveAttributeData();
//     void on_edit_button_clicked();
//     void selectFeature(const QgsFeature& feature);



//     void clear(bool value);



private:
     QgsMapLayer* addRasterSlot();
     QgsMapLayer* requestImage();
     QgsMarkerSymbol* GetPointsSymbol(QColor color_,double size_,QgsVectorLayer* points);
     QgsLineSymbol* GetLinesSymbol(QColor color_,double size_,QgsVectorLayer* lines);
     QgsFillSymbol* GetVirtualsSymbol(QColor color_,double size_,QgsVectorLayer* virtuals);
     void resizeEvent(QResizeEvent*);
     void clearShpLayer();
     DrawingMode getCanvasStatus(int index);





private:
    QWidget *centralWidget;
    MyFeatureTool* add_feature_tool;

    QgsMapCanvas *mMapCanvas;
    QList<QgsMapLayer*> layers;
    QList<QgsMapLayer*> select_layers;
    QgsFeatureIds deleteFeatureIds;
//    QgsFeature *select_feature=nullptr;
//    int select_feature_id;
//    QList<QgsVectorLayer*> readLayers;

    std::map<int,QgsVectorLayer *> my_layers;
    QgsVectorLayer *point_layer;
    QgsVectorLayer *line_layer;
    QgsVectorLayer *virtual_layer;
    QgsGeometry select_geometry;
    QgsHighlight *highlight=nullptr;

    QList<QgsFeature> lineFeatureList;
    QVector<QgsPoint> line_vertex;
    QgsFeature creating_lineFeature;


    DrawingMode canvas_status;
    int currentLayer=0;
    QgsMapToolCapture *my_toolCapture;




//    QList<QgsMapLayer*> readLayers;
    QgsCoordinateReferenceSystem crs;

    QString dirname;

};

#endif // QGSGRAPHMAP_H
