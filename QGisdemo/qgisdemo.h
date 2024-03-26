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
#include <gdal.h>

#include "qgsgraphmap.h"
#include "mygdaltool.h"
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
    void AllClear();
    void SaveAttribute();
    void curManvasStatusChange(int index);

public slots:
    void on_delete_button_clicked();
    void on_all_clear_button_clicked();
    void on_save_attribute_button_clicked();
    void on_min_path_clicked();






private:
    Ui::QGisDemo *ui;
    QgsGraphMap *Gis_map;
};

#endif // QGISDEMO_H
