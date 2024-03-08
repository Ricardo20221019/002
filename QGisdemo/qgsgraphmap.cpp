#include "qgsgraphmap.h"

QgsGraphMap::QgsGraphMap(QWidget *parent) : QWidget(parent)
{
    mMapCanvas = new QgsMapCanvas(this);
    mMapCanvas->enableAntiAliasing(true);//抗锯齿
    mMapCanvas->setCachingEnabled(true);//启动地图画布的缓存功能
    mMapCanvas->setCanvasColor(QColor(255,255,255));//设置画布背景色
    mMapCanvas->setVisible(true);
    mMapCanvas->zoomToFullExtent();
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(mMapCanvas);

    initalive();

    mapToolPan=new QgsMapToolPan(mMapCanvas);
    identifyTool= new QgsMapToolIdentify(mMapCanvas);
//    mMapCanvas->setMapTool(identifyTool);

    connect(mMapCanvas, SIGNAL(xyCoordinates(QgsPointXY)),
            this, SLOT(Show_Coordinate(QgsPointXY)) );
//    mToolEmitPoint=new QgsMapToolEmitPoint(mMapCanvas);
    add_feature_tool=new MyFeatureTool(mMapCanvas);
    connect(this,&QgsGraphMap::deletefeature,add_feature_tool,&MyFeatureTool::deleteFeature);

//    mMapCanvas->setMapTool(feature_tool);

    canvas_status=DrawingMode::SelectionMode;
//    mMapCanvas->setMapTool(mToolEmitPoint);








}
void QgsGraphMap::Show_Coordinate(const QgsPointXY &p)
{
//    qDebug()<<p.x()<<" "<<p.y();

}
void QgsGraphMap::setCanvasStatus(int index)
{
    canvas_status=getCanvasStatus(index);
    mMapCanvas->unsetMapTool(mMapCanvas->mapTool());
    if(index==0)
    {
        add_feature_tool->stopCapturing();
        mMapCanvas->setMapTool(select_feature_Tool[index]);
        qDebug()<<"-----选择工具";
    }
    else if(index==1)
    {
        add_feature_tool->startCapturing();
        mMapCanvas->setMapTool(add_feature_tool);
        if(highlight)
        {
            delete highlight;
            highlight=nullptr;
            select_feature=nullptr;
        }
        qDebug()<<"-----绘制工具";
    }

}

void QgsGraphMap::setCurrentLayer(int index)
{
    currentLayer=index;
    mMapCanvas->setCurrentLayer(my_layers[index]);
    QgsVectorLayer *curLayer=qobject_cast<QgsVectorLayer *>(mMapCanvas->currentLayer());
    qDebug()<<"当前图层："<<curLayer->geometryType();
    if(canvas_status==DrawingMode::SelectionMode)
    {
        mMapCanvas->unsetMapTool(mMapCanvas->mapTool());
        mMapCanvas->setMapTool(select_feature_Tool[index]);
    }
    else if(canvas_status==DrawingMode::LineMode)
    {
        qDebug()<<"切换到直线图层";
    }
    else if(canvas_status==DrawingMode::AreaMode)
    {
        qDebug()<<"切换到多边形图层";
    }



//    qDebug()<<"当前图层："<<index;

//    mMapCanvas->currentLayer()->
}
void QgsGraphMap::deleteFeature()
{

    if(highlight)
    {
        emit deletefeature(select_feature_id);
        delete highlight;
        highlight=nullptr;
        select_feature=nullptr;
    }
    mMapCanvas->refresh();

}
//void QgsGraphMap::clear(bool value)
//{
//    clearShpLayer();
//}
void QgsGraphMap::on_edit_button_clicked()
{

    my_layers[0]->rollBack(true);
    my_layers[1]->rollBack(true);
    my_layers[2]->rollBack(true);
    qDebug()<<"绘制 回滚";


    mMapCanvas->setCurrentLayer(my_layers[currentLayer]);
    my_layers[currentLayer]->startEditing();
    qDebug()<<"图层："<<currentLayer<<"开始编辑！";

//    qDebug()<<123;
}
void QgsGraphMap::selectFeature(const QgsFeature& feature)
{
    if(!highlight)//选中要素没有高亮
    {
        if(select_feature)//之前有选中要素
        {
//                    delete highlight;
            highlight=nullptr;
//                    select_feature=nullptr;
            setHightLight(feature);
        }
        else
        {
            setHightLight(feature);
        }

    }
    else//选中要素有高亮
    {
        delete highlight;
        highlight=nullptr;
        select_feature=nullptr;
        setHightLight(feature);
    }
}
void QgsGraphMap::SaveShp(QString dir_path,QString filename,QgsVectorLayer* layers)
{
    QgsVectorFileWriter::SaveVectorOptions saveOptions;
    saveOptions.driverName = "ESRI Shapefile"; // 设置输出驱动为Shapefile
    saveOptions.fileEncoding = "UTF-8";
    QgsCoordinateTransformContext transformContext;
    int res=QgsVectorFileWriter::writeAsVectorFormatV2(layers,dir_path+filename,transformContext,saveOptions);
    if(res!=0)
    {
        qDebug()<<"保存矢量图层为.shp文件失败";
    }
}
void QgsGraphMap::InitConnectMapTool()
{

//
    select_feature_Tool[0]=new QgsMapToolIdentifyFeature(mMapCanvas,my_layers[0]);
    select_feature_Tool[1]=new QgsMapToolIdentifyFeature(mMapCanvas,my_layers[1]);
    select_feature_Tool[2]=new QgsMapToolIdentifyFeature(mMapCanvas,my_layers[2]);

    feature_editors[0]=my_layers[0]->dataProvider();
    feature_editors[1]=my_layers[1]->dataProvider();
    feature_editors[2]=my_layers[2]->dataProvider();

    connect(select_feature_Tool[0], static_cast<void (QgsMapToolIdentifyFeature::*)(const QgsFeature &)>
            (&QgsMapToolIdentifyFeature::featureIdentified),this,&QgsGraphMap::selectFeature);
    connect(select_feature_Tool[1], static_cast<void (QgsMapToolIdentifyFeature::*)(const QgsFeature &)>
            (&QgsMapToolIdentifyFeature::featureIdentified),this,&QgsGraphMap::selectFeature);
    connect(select_feature_Tool[2], static_cast<void (QgsMapToolIdentifyFeature::*)(const QgsFeature &)>
            (&QgsMapToolIdentifyFeature::featureIdentified),this,&QgsGraphMap::selectFeature);
}
void QgsGraphMap::initalive()
{

    QgsMapLayer* map_layer=requestImage();//
    GetNetData();
    InitConnectMapTool();


    select_layers.append(my_layers[0]);
    select_layers.append(my_layers[1]);
    select_layers.append(my_layers[2]);
    layers.append(my_layers[0]);
    layers.append(my_layers[1]);
    layers.append(my_layers[2]);
    layers.append(map_layer);
//    my_layers[0]->startEditing();
//    my_layers[1]->startEditing();
//    my_layers[2]->startEditing();

    mMapCanvas->setExtent(map_layer->extent());
    mMapCanvas->setLayers(layers);
    mMapCanvas->setGeometry(QWidget::geometry());
    mMapCanvas->refresh();

    mMapCanvas->setCurrentLayer(my_layers[0]);
    mMapCanvas->setMapTool(select_feature_Tool[0]);
//        mMapCanvas->setMapTool(identifyTool);
}
// 在 MainWindow.cpp 文件中实现槽函数
QgsMapLayer* QgsGraphMap::requestImage()
{
//    // 获取栅格图层    auto uri = QString("file:////home/chenlang/QGisDemo/TIF/grid_map_.tif");
    QgsRasterLayer *my_rasterlayer = new QgsRasterLayer("/home/chenlang/QGisDemo/TIF/grid_map_.tif", "Raster Layer", "gdal");
    //比例为0.15
//    qDebug()<<my_rasterlayer->width()<<" "<<my_rasterlayer->height();
    return my_rasterlayer;
}
void QgsGraphMap::resizeEvent(QResizeEvent*)
{
    mMapCanvas->setGeometry(this->geometry());
}
void QgsGraphMap::clearShpLayer()
{
    point_layer->startEditing();//切换编辑
    point_layer->selectAll();//选中所有要素
    const auto selectIds=point_layer->selectedFeatureIds();
    for(const auto &selectId:selectIds )
    {
        point_layer->deleteFeature(selectId);
    }

//    point_layer-
//    point_layer->commitChanges(false);
//    mMapCanvas->refresh();

}
void QgsGraphMap::setHightLight(const QgsFeature & feature)
{
//    qDebug()<<feature.id();
    select_feature=const_cast<QgsFeature*>(&feature);
//    qDebug()<<select_feature->id();
    select_feature_id=select_feature->id();
    highlight=new QgsHighlight(mMapCanvas,feature,my_layers[currentLayer]);
    highlight->setFillColor(Qt::green);	// 填充颜色
    highlight->setColor(Qt::green);	// 设置线条/描边颜色
    highlight->update();
//    qDebug()<<"填充";
//    highlight->setBuffer(0.5);
//    highlight->setBoundingRegionGranularity(0.1);
    highlight->setBuffer(Qgis::DEFAULT_HIGHLIGHT_BUFFER_MM);	// 设置行/行程缓冲以毫米为单位
    // DEFAULT_HIGHLIGHT_MIN_WIDTH_MM = 1.0
    highlight->setMinWidth(Qgis::DEFAULT_HIGHLIGHT_MIN_WIDTH_MM);	// 设置最小线/笔画宽度，以毫米为单位



}
DrawingMode QgsGraphMap::getCanvasStatus(int index)
{
    switch (index)
    {
    case 0:
        return DrawingMode::SelectionMode;
    case 1:
        return DrawingMode::LineMode;
    case 2:
        return DrawingMode::AreaMode;//区域模式（虚拟墙编辑）
    default:
        return DrawingMode::ForbidEditMode;
    }
}
QgsMapLayer* QgsGraphMap::SetPointsData()
{
    // 创建点图层
    QgsFields fields;

    fields.append(QgsField("Name", QVariant::String));
    QgsVectorLayer *pointLayer = new QgsVectorLayer("Point?crs=EPSG:4326", "PointLayer", "memory");
//    pointLayer->addAttribute()

    QgsVectorDataProvider* pointProvider = pointLayer->dataProvider();



    // 创建点要素
    QgsFeature pointFeature;
    QgsPointXY point(10.0, 20.0);  // 使用 QgsPointXY 创建点
    pointFeature.setGeometry(QgsGeometry::fromPointXY(point));
    pointFeature.setAttribute("Name", "Point1");


    // 添加点要素到图层
    QList<QgsFeature> pointFeatureList;
    pointFeatureList.append(pointFeature);
    pointProvider->addFeatures(pointFeatureList);
//    RenderingPoints(Qt::red,1.5,pointLayer);

    // 显示图层
    QgsProject::instance()->addMapLayer(pointLayer, true);
    return pointLayer;

}
QgsMapLayer* QgsGraphMap::SetLinesData()
{

    //设置线图层的样式，包括color, width (in millimeters) and penStyle
//    QgsSimpleLineSymbolLayer* Line_Renderer_Layer = new QgsSimpleLineSymbolLayer();
//    Line_Renderer_Layer->setColor(Qt::red);
//    Line_Renderer_Layer->setPenStyle(Qt::PenStyle::DotLine);

    // 创建线图层
    QgsVectorLayer* lineLayer = new QgsVectorLayer("LineString?crs=EPSG:4326", "LineLayer", "memory");
    lineLayer->setCrs(crs);
    QgsVectorDataProvider* lineProvider = lineLayer->dataProvider();
    // 创建线要素
    QgsFeature lineFeature;
    QgsGeometry line = QgsGeometry::fromPolylineXY({QgsPoint(0.0, 0.0), QgsPoint(100.0, 100.0)});
    lineFeature.setGeometry(line);


    // 添加线要素到图层
    QList<QgsFeature> lineFeatureList;
    lineFeatureList.append(lineFeature);
    lineProvider->addFeatures({lineFeatureList});
    // 显示图层
    QgsProject::instance()->addMapLayer(lineLayer, true);
    return lineLayer;
}

void QgsGraphMap::RenderingLayers(QColor color_,double size_,QgsVectorLayer* layers)
{
    QgsWkbTypes::GeometryType geometyrtype_= layers->geometryType();
    QgsSingleSymbolRenderer* renderer;
    if(geometyrtype_==QgsWkbTypes::PointGeometry)
    {
//        qDebug()<<1;
        renderer= new QgsSingleSymbolRenderer(GetPointsSymbol(color_,size_,layers));
        // 设置点符号为圆形

    }
    else if(geometyrtype_==QgsWkbTypes::LineGeometry)
    {
//        qDebug()<<2;
        renderer= new QgsSingleSymbolRenderer(GetLinesSymbol(color_,size_,layers));
    }
    else if(geometyrtype_==QgsWkbTypes::PolygonGeometry)
    {
//        qDebug()<<3;
        renderer= new QgsSingleSymbolRenderer(GetVirtualsSymbol(color_,size_,layers));
    }
    else
    {
        qDebug()<<"渲染失败，无法判断传入图层的几何图形类型！";
        return;
    }
    layers->setRenderer(renderer);
}

void QgsGraphMap::GetNetData()//QList<QString> paths
{
//    dirname="/home/chenlang/soft/QGisdemo/Map/net";
//    paths.append(dirname+"/points.shp");
//    paths.append(dirname+"/lines.shp");
//    paths.append(dirname+"/virtuals.shp");

    QString point_path="/home/chenlang/soft/QGisdemo/Map/net/points.shp";
    QString line_path="/home/chenlang/soft/QGisdemo/Map/net/lines.shp";
    QString virtual_path="/home/chenlang/soft/QGisdemo/Map/net/virtuals.shp";

//    point_layer=new QgsVectorLayer(point_path, "point", "ogr");
//    line_layer=new QgsVectorLayer(line_path, "line", "ogr");
//    virtual_layer=new QgsVectorLayer(virtual_path, "virtual", "ogr");
    my_layers[0]=new QgsVectorLayer(point_path, "point", "ogr");
    my_layers[1]=new QgsVectorLayer(line_path, "line", "ogr");
    my_layers[2]=new QgsVectorLayer(virtual_path, "virtual", "ogr");

//    RenderingLayers();
    RenderingLayers(Qt::red,5,my_layers[0]);
    RenderingLayers(QColor(0,0,255,180),0.5,my_layers[1]);
    RenderingLayers(Qt::red,1,my_layers[2]);
//    QgsProject::instance()->addMapLayer(point_layer);
//    QgsProject::instance()->addMapLayer(line_layer);
//    QgsProject::instance()->addMapLayer(virtual_layer);

//    for(auto path:paths)
//    {
//        QgsVectorLayer *shpLayer=new QgsVectorLayer(path, "layer", "ogr");

//        if (!shpLayer->isValid()) {
//            qDebug() <<path <<"Error: Invalid layer!";
//            return;
//        }
//        if(shpLayer->geometryType()==QgsWkbTypes::PointGeometry)
//        {
////            SaveShp("/home/chenlang/QGisDemo/Map/savenet","/points.shp",shpLayer);
//            RenderingLayers(Qt::red,2,shpLayer);
//            point_layer=shpLayer;

//        }
//        else if(shpLayer->geometryType()==QgsWkbTypes::LineGeometry)
//        {
//            RenderingLayers(QColor(0,0,255,180),0.5,shpLayer);
//        }
//        else if(shpLayer->geometryType()==QgsWkbTypes::PolygonGeometry)
//        {
//            RenderingLayers(Qt::red,1,shpLayer);
//        }
//        readLayers.append(shpLayer);
//        QgsProject::instance()->addMapLayer(shpLayer);
//    }

}
//void QgsGraphMap::MousePressEvent(QMouseEvent *event)
//{
////     QPointF point = mapToScene(event->pos());
//}
QgsMapLayer* QgsGraphMap::addRasterSlot()
{
    QString urlWithParams;
    QString type = QStringLiteral("xyz");
    int zMin = 0;
    int zMax = 0;
    //  auto uri = QString("http://127.0.0.1:9090/download/{z}/{x}/{y}.png");
//    auto uri = QString("file:////home/chenlang/QGisDemo/PGM/grid_map.pgm");//grid_map_.tif
    auto uri = QString("file:////home/chenlang/QGisDemo/TIF/grid_map_.tif");
    //    auto uri = QString("file:////home/chenlang/map/Mapnik");
    //          QString uri = QStringLiteral("https://wprd01.is.autonavi.com/appmaptile?style=8&x={x}&y={y}&z={z}");
    //    QString uri = QStringLiteral("https://server.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile/{z}/{y}/{x}");
#if 1
    QgsDataSourceUri urik;
    urik.setParam( QStringLiteral( "url" ), uri );
    urik.setParam( QStringLiteral( "type" ), type );
    urik.setParam( QStringLiteral( "zmin" ), QString::number( zMin ) );
    urik.setParam( QStringLiteral( "zmax" ), QString::number( zMax ) );
    urlWithParams = urik.encodedUri();
#else
    QString urlEncode = QUrl::toPercentEncoding(url);
    urlWithParams = QString("type=xyz&url=%1&zmax=18&zmin=0").arg(urlEncode);
#endif
    QString type2 = QStringLiteral("wms");
    QgsRasterLayer *rlayer = new QgsRasterLayer(urlWithParams,"baseName",type2);

    //    QgsProject::instance()->addMapLayer(rlayer);
    qDebug()<<urlWithParams<<rlayer->isValid();
    return rlayer;
}
QgsMarkerSymbol* QgsGraphMap::GetPointsSymbol(QColor color_,double size_,QgsVectorLayer* points)
{
    QgsMarkerSymbol* symbol = new QgsMarkerSymbol();//std::shared_ptr<QgsMarkerSymbol> symbol = std::make_shared<QgsMarkerSymbol>();
    symbol->setSize(size_);  // 设置点的大小
    symbol->setColor(color_);  // 设置点的颜色
    return symbol;
}
QgsLineSymbol* QgsGraphMap::GetLinesSymbol(QColor color_,double size_,QgsVectorLayer* lines)
{
    QgsLineSymbol* Line_Renderer_Layer = new QgsLineSymbol();
    Line_Renderer_Layer->setColor(QColor(255,255,255,0));
    Line_Renderer_Layer->setWidth(size_);



    // 创建自定义虚线样式
    QVector<qreal> customDashes;
    customDashes << 5 << 5; // 自定义虚线的样式，交替设置不同长度的实线和空白部分
    QgsSimpleLineSymbolLayer* simpleLineLayer = new QgsSimpleLineSymbolLayer();
    simpleLineLayer->setUseCustomDashPattern(true);//开启自定义虚线
    simpleLineLayer->setCustomDashVector(customDashes);
    simpleLineLayer->setColor(color_);
//    simpleLineLayer->setStrokeColor(Qt::white);
    simpleLineLayer->setWidth(size_);
    Line_Renderer_Layer->symbolLayers().clear(); // 清除默认的 symbolLayers
    Line_Renderer_Layer->appendSymbolLayer(simpleLineLayer);

    return Line_Renderer_Layer;
}
QgsFillSymbol* QgsGraphMap::GetVirtualsSymbol(QColor color_,double size_,QgsVectorLayer* virtuals)
{
    //     创建填充符号并设置符号图层列表
    QgsFillSymbol* fillSymbol = new QgsFillSymbol();
    fillSymbol->setColor(Qt::white);

    QgsSimpleFillSymbolLayer* simpleFillLayer=new QgsSimpleFillSymbolLayer();
    simpleFillLayer->setBrushStyle(Qt::BDiagPattern);//BDiagPattern

    simpleFillLayer->setStrokeColor(color_);
    simpleFillLayer->setStrokeWidth(size_);
    simpleFillLayer->setColor(color_);

    //    // 设置斜线填充的宽度
    //    QgsLineSymbol* lineSymbol = new QgsLineSymbol();
    //    lineSymbol->setWidth(1.0); // 设置斜线的宽度为 1.0
    //    simpleFillLayer->setBrushStyle();
    //    simpleFillLayer->setLineSymbol(lineSymbol);


    fillSymbol->symbolLayers().clear();
    fillSymbol->appendSymbolLayer(simpleFillLayer);
    return fillSymbol;
}
