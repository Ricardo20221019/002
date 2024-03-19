#include "qgisdemo.h"
#include "ui_qgisdemo.h"

QGisDemo::QGisDemo(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QGisDemo)
{
    ui->setupUi(this);


    Gis_map=ui->qgsMap;
    connect(this,&QGisDemo::curManvasStatusChange,Gis_map,&QgsGraphMap::setToolStatus);
    connect(ui->select_combox,QOverload<int>::of(&QComboBox::currentIndexChanged),[=](int value)
    {
        if(value!=0)
        {
            ui->delete_button->setDisabled(true);
        }
        else
        {
            ui->delete_button->setDisabled(false);
        }
        emit curManvasStatusChange(value);

    });
    connect(this,&QGisDemo::DeleteFeature,Gis_map,&QgsGraphMap::deleteFeature);
    connect(this,&QGisDemo::AllClear,Gis_map,&QgsGraphMap::allClear);
    connect(this,&QGisDemo::SaveAttribute,Gis_map,&QgsGraphMap::saveAttributeData);
}

QGisDemo::~QGisDemo()
{
    delete ui;
}
void QGisDemo::on_delete_button_clicked()
{
    emit DeleteFeature();
}

void QGisDemo::on_all_clear_button_clicked()
{
    emit AllClear();
}
void QGisDemo::on_save_attribute_button_clicked()
{
    emit SaveAttribute();
}
void QGisDemo::on_min_path_clicked()
{
    qDebug()<<"最小路径";
}
