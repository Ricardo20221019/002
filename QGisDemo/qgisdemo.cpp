#include "qgisdemo.h"
#include "ui_qgisdemo.h"

QGisDemo::QGisDemo(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QGisDemo)
{
    ui->setupUi(this);


    Gis_map=ui->qgis_widget;
    Gis_map->initalive();

}

QGisDemo::~QGisDemo()
{
    delete ui;
}

