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
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <qgssymbol.h>
#include <qgsfillsymbollayer.h>
#include <qgslinesymbollayer.h>
#include <QMouseEvent>
#include <QGraphicsView>
#include <memory>
#include "qgssymbollayer.h"
#include "data.h"

class QgsGraphMap : public QWidget
{
    Q_OBJECT
public:
    explicit QgsGraphMap(QWidget *parent);
    void initalive();
    QgsMapLayer* SetPointsData();
    QgsMapLayer* SetLinesData();
    void GetNetData(QList<QString> paths);
    void RenderingLayers(QColor color_,double size_,QgsVectorLayer* points);
//    void Rendering(QColor lines_color,QColor points_color,double line_size,double point_size,QgsVectorLayer* layers);
    void SaveShp(QString dir_path,QString filename,QgsVectorLayer* layers);

signals:

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
//        qDebug()<<event->x();
    }
public slots:
//    void MousePressEvent(QMouseEvent *event);
     void Show_Coordinate(const QgsPointXY &p);

private:
     QgsMapLayer* addRasterSlot();
     QgsMapLayer* requestImage();
     QgsMarkerSymbol* GetPointsSymbol(QColor color_,double size_,QgsVectorLayer* points);
     QgsLineSymbol* GetLinesSymbol(QColor color_,double size_,QgsVectorLayer* lines);
     QgsFillSymbol* GetVirtualsSymbol(QColor color_,double size_,QgsVectorLayer* virtuals);
     void resizeEvent(QResizeEvent*);



private:
    QWidget *centralWidget;
    QgsMapCanvas *mMapCanvas;
    QList<QgsMapLayer*> layers;
    QList<QgsVectorLayer*> readLayers;
//    QList<QgsMapLayer*> readLayers;
    QgsCoordinateReferenceSystem crs;

    QString dirname;

};

#endif // QGSGRAPHMAP_H
