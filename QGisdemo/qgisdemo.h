#ifndef QGISDEMO_H
#define QGISDEMO_H
#include <QMainWindow>
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

#include "qgsgraphmap.h"
namespace Ui {
class QGisDemo;
}

class QGisDemo : public QMainWindow
{
    Q_OBJECT

public:
    explicit QGisDemo(QWidget *parent = 0);
    ~QGisDemo();

signals:
    void DeleteFeature();
    void addFeature();
    void curLayerChange(int index);
    void curManvasStatusChange(int index);
public slots:
    void on_delete_button_clicked();
    void on_edit_button_clicked();





private:
    Ui::QGisDemo *ui;
    QgsGraphMap *Gis_map;
};

#endif // QGISDEMO_H
