#include "qgisdemo.h"
#include "ui_qgisdemo.h"

QGisDemo::QGisDemo(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QGisDemo)
{
    ui->setupUi(this);


    Gis_map=ui->qgsMap;
//    Gis_map->initalive();
//    connect(ui->point_button,&QPushButton::clicked,Gis_map,&QgsGraphMap::clear);
//    connect(this,&QGisDemo::selectLayer,Gis_map,&QgsGraphMap::selectShpLayer);
    connect(this,&QGisDemo::curLayerChange,Gis_map,&QgsGraphMap::setCurrentLayer);
    connect(this,&QGisDemo::curManvasStatusChange,Gis_map,&QgsGraphMap::setCanvasStatus);
    connect(ui->layer_combox,QOverload<int>::of(&QComboBox::currentIndexChanged),[=](int value)
    {
        emit curLayerChange(value);
        ui->layer_combox->setDisabled(true);
        ui->select_combox->setDisabled(true);
    });
    connect(ui->select_combox,QOverload<int>::of(&QComboBox::currentIndexChanged),[=](int value)
    {
        emit curManvasStatusChange(value);
        ui->select_combox->setDisabled(true);
        ui->layer_combox->setDisabled(true);
    });
//    connect(ui->layer_combox,QOverload<int>::of(&QComboBox::currentIndexChanged),Gis_map,&QgsGraphMap::setCurrentLayer);
//    connect(ui->select_combox,QOverload<int>::of(&QComboBox::currentIndexChanged),Gis_map,&QgsGraphMap::setCanvasStatus);
    connect(this,&QGisDemo::DeleteFeature,Gis_map,&QgsGraphMap::deleteFeature);
    connect(this,&QGisDemo::addFeature,Gis_map,&QgsGraphMap::on_edit_button_clicked);

//    connect(ui->select_combox,&QComboBox::currentIndexChanged,Gis_map,&QgsGraphMap::GetFunction);
//    connect(ui->layer_combox,&QComboBox::currentIndexChanged,Gis_map,&QgsGraphMap::SelectLayer);
//    connect(ui->select_button,&QPushButton::clicked,Gis_map,&QgsGraphMap::selectShpLayer);

}

QGisDemo::~QGisDemo()
{
    delete ui;
}
void QGisDemo::on_delete_button_clicked()
{
    emit DeleteFeature();
//    qDebug()<<123;
}
void QGisDemo::on_edit_button_clicked()
{
    ui->layer_combox->setDisabled(false);
    ui->select_combox->setDisabled(false);
    emit addFeature();
}


