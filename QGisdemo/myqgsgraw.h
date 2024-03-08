#ifndef MYQGSGRAW_H
#define MYQGSGRAW_H

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
#include <QMouseEvent>
#include <QGraphicsView>
#include <memory>
#include <map>
#include "qgssymbollayer.h"

class MyQgsGraw
{
public:
    MyQgsGraw();



private:
    QgsFields fields;//属性
    QgsVectorLayer *pointLayer;
    QgsVectorLayer *lineLayer;
    QgsVectorLayer *virtualLayer;


};

#endif // MYQGSGRAW_H
