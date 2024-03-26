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


    connect(mMapCanvas, SIGNAL(xyCoordinates(QgsPointXY)),
            this, SLOT(Show_Coordinate(QgsPointXY)) );
//    mToolEmitPoint=new QgsMapToolEmitPoint(mMapCanvas);
    add_feature_tool=new MyFeatureTool(mMapCanvas);
    add_feature_tool->addLayers(my_layers[0],0);
    add_feature_tool->addLayers(my_layers[1],1);
    add_feature_tool->addLayers(my_layers[2],2);
    connect(this,&QgsGraphMap::deletefeature,add_feature_tool,&MyFeatureTool::deleteFeature);
//    connect(add_feature_tool,&MyFeatureTool::curLayerChange,this,&QgsGraphMap::setCurrentLayer);
    connect(this,&QgsGraphMap::allClearFeatures,add_feature_tool,&MyFeatureTool::allClear);


    add_feature_tool->setMode(DrawingMode::SelectionMode);
    mMapCanvas->setMapTool(add_feature_tool);
    updateAtrribute();

//    canvas_status=DrawingMode::SelectionMode;
//    mMapCanvas->setMapTool(mToolEmitPoint);








}
void QgsGraphMap::Show_Coordinate(const QgsPointXY &p)
{
//    qDebug()<<p.x()<<" "<<p.y();

}
void QgsGraphMap::setToolStatus(int index)
{
//    canvas_status=getCanvasStatus(index);
    if(index!=0)//绘制
    {
        if(add_feature_tool->Mode()==DrawingMode::SelectionMode)
        {
            add_feature_tool->stopCapturing();
        }
        add_feature_tool->clearHighLight();
        add_feature_tool->startCapturing();
        if(index==4)
        {
            add_feature_tool->setFindStatus(true);
        }
    }
    else//选中
    {
        add_feature_tool->stopCapturing();
    }

    add_feature_tool->setMode(getCanvasStatus(index));
}

//void QgsGraphMap::setCurrentLayer(int index)//curLayerChange(QgsWkbTypes::GeometryType curType);
//{
//    currentLayer=index;
//    mMapCanvas->setCurrentLayer(my_layers[index]);
//    my_layers[index]->startEditing();
////    QgsVectorLayer *curLayer=qobject_cast<QgsVectorLayer *>(mMapCanvas->currentLayer());
////    qDebug()<<"当前图层："<<curLayer->geometryType();

//}

void QgsGraphMap::deleteFeature()
{
    if(add_feature_tool->getFeatureId()!=-1)
    {
        emit deletefeature();
        mMapCanvas->refresh();
    }
}
void QgsGraphMap::allClear()
{
    emit allClearFeatures();
}
void QgsGraphMap::SaveShp(QString file_path,QgsVectorLayer* layers)
{
    QgsVectorFileWriter::SaveVectorOptions saveOptions;
    saveOptions.driverName = "ESRI Shapefile"; // 设置输出驱动为Shapefile
    saveOptions.fileEncoding = "UTF-8";
    QgsCoordinateTransformContext transformContext;
    int res=QgsVectorFileWriter::writeAsVectorFormatV2(layers,file_path,transformContext,saveOptions);
    if(res!=0)
    {
        qDebug()<<"保存矢量图层为.shp文件失败"<<res;
    }
}

void QgsGraphMap::initalive()
{

    QgsMapLayer* map_layer=requestImage();//
    GetNetData();


    select_layers.append(my_layers[0]);
    select_layers.append(my_layers[1]);
    select_layers.append(my_layers[2]);



    layers.append(my_layers[0]);
    layers.append(my_layers[1]);
    layers.append(my_layers[2]);
    layers.append(map_layer);

    mMapCanvas->setExtent(map_layer->extent());
    mMapCanvas->setLayers(layers);
    mMapCanvas->setGeometry(QWidget::geometry());
    mMapCanvas->refresh();

    mMapCanvas->setCurrentLayer(my_layers[0]);

}
// 在 MainWindow.cpp 文件中实现槽函数
QgsMapLayer* QgsGraphMap::requestImage()
{
//    // 获取栅格图层    auto uri = QString("file:////home/chenlang/soft/QGisdemo/TIF/grid_map_.tif");
    QgsRasterLayer *my_rasterlayer = new QgsRasterLayer("/home/chenlang/soft/QGisdemo/TIF/grid_map_.tif", "Raster Layer", "gdal");
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

DrawingMode QgsGraphMap::getCanvasStatus(int index)
{
    switch (index)
    {
    case 0:
        return DrawingMode::SelectionMode;
    case 1:
        return DrawingMode::LineMode;
    case 2:
        return DrawingMode::CurveMode;
    case 3:
        return DrawingMode::AreaMode;//区域模式（虚拟墙编辑）
    case 4:
        return DrawingMode::FindPath;
    default:
        return DrawingMode::ForbidEditMode;
    }
}
void QgsGraphMap::updateAtrribute()
{

    qDebug()<<"更新字段";
    QgsFeatureIterator point_itertor=my_layers[0]->getFeatures();
    QgsFeature feature;
    while(point_itertor.nextFeature(feature))
    {
        PointFeature point_feature;
        point_feature.feature_id=feature.id();
//        point_feature.attribute_id=feature.id();
//        point_feature.feature=new QgsFeature(&feature);
        QList<QString> list=feature.attribute("lines").toString().split(' ');
        for(auto it:list)
        {
            point_feature.lines.append(it.toInt());
        }
        point_feature.feature=feature;
        point_feature.x=feature.attribute("x").toDouble();
        point_feature.y=feature.attribute("y").toDouble();
        point_feature.type=feature.attribute("type").toInt();
        add_feature_tool->setPointFeature(feature.id(),point_feature);
    }

    QgsFeatureIterator line_itertor=my_layers[1]->getFeatures();
    QgsFeature line_feature;
    while(line_itertor.nextFeature(line_feature))
    {
        LineFeature temp_feature;
        temp_feature.feature_id=line_feature.id();
//        temp_feature.attribute_id=line_feature.id();
        temp_feature.feature=line_feature;
        temp_feature.src_feature_id=line_feature.attribute("src_id").toInt();
        temp_feature.dst_feature_id=line_feature.attribute("dst_id").toInt();
        temp_feature.control_point_id=line_feature.attribute("control_id").toInt();
        temp_feature.orinted=line_feature.attribute("orinted").toInt();
        temp_feature.length=line_feature.attribute("length").toDouble();
        add_feature_tool->setLineFeature(line_feature.id(),temp_feature);
    }

    QgsFeatureIterator area_itertor=my_layers[2]->getFeatures();
    QgsFeature area_feature;
    while(area_itertor.nextFeature(area_feature))
    {
        PolgonFeature temp_feature;
        temp_feature.feature_id=area_feature.id();
//        temp_feature.attribute_id=area_feature.id();
        temp_feature.points=area_feature.attribute("points").toString();
        add_feature_tool->setPolognFeature(area_feature.id(),temp_feature);
    }



}
void QgsGraphMap::saveAttributeData()
{
    add_feature_tool->saveAttributeDatas();
//    SaveShp(point_path,my_layers[0]);
//    SaveShp(line_path,my_layers[1]);
//    SaveShp(virtual_path,my_layers[2]);
//    qDebug()<<"保存字段";
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


    point_path="/home/chenlang/soft/QGisdemo/Map/net/points.shp";
    line_path="/home/chenlang/soft/QGisdemo/Map/net/lines.shp";
    virtual_path="/home/chenlang/soft/QGisdemo/Map/net/virtuals.shp";

    my_layers[0]=new QgsVectorLayer(point_path, "point", "ogr");
    my_layers[1]=new QgsVectorLayer(line_path, "line", "ogr");
    my_layers[2]=new QgsVectorLayer(virtual_path, "virtual", "ogr");

//    RenderingLayers();
    RenderingLayers(Qt::red,5,my_layers[0]);
    RenderingLayers(QColor(0,0,255,180),0.8,my_layers[1]);
    RenderingLayers(Qt::red,1,my_layers[2]);

//    my_layers[0]->setCustomProperty();






}

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
