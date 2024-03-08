#include "myfeaturetool.h"

MyFeatureTool::MyFeatureTool(QgsMapCanvas* canvas)
    :QgsMapTool(canvas)
{
    mCapturing=false;
    mRubberBand = 0;
    mTempRubberBand = 0;
}
MyFeatureTool::~MyFeatureTool()
{

}
void MyFeatureTool::canvasPressEvent( QgsMapMouseEvent* e )
{

}
//! 重写鼠标指针释放事件
void MyFeatureTool::canvasReleaseEvent( QgsMapMouseEvent* e )
{
    if(!mCapturing) return;
    qDebug()<<"当前点："<<e->x()<<e->y();
    QgsVectorLayer *curLayer=qobject_cast<QgsVectorLayer *>(mCanvas->currentLayer());

    if(!curLayer) {return;}//是否是矢量图层
//    if(!curLayer->isEditable())
//    {
//        qDebug()<<"不可编辑！";
//        return;
//    }
    QgsPointXY temp_point=toLayerCoordinates( curLayer, mCanvas->mapSettings().mapToPixel().toMapCoordinates( e->pos() ) );
    QgsPoint savePoint;
    savePoint.setX(temp_point.x());
    savePoint.setY(temp_point.y());

    switch( curLayer->geometryType() )
    {
    case QgsWkbTypes::PointGeometry:
        m_captureMode = CapturePoint;
        qDebug()<<"准备添加：点";
        break;
    case QgsWkbTypes::LineGeometry:
        m_captureMode = CaptureLine;
        qDebug()<<"准备添加：线";
        break;
    case QgsWkbTypes::PolygonGeometry:
        m_captureMode = CapturePolygon;
        qDebug()<<"准备添加：多边形";
        break;
    default:
        break;
    }
//    qDebug()<<"准备添加：";
    QgsGeometry* new_geomery;
    if(m_captureMode==CapturePoint)
    {
        new_geomery=new QgsGeometry(QgsGeometry::fromPointXY(QgsPointXY( savePoint.x(),savePoint.y())));
        QgsFeature feature(curLayer->fields(),0);
        feature.setGeometry(*new_geomery);

        addFeature(curLayer,&feature);
    }
    else if(m_captureMode == CaptureLine || m_captureMode == CapturePolygon)
    {
        if(e->button()==Qt::LeftButton)
        {
//            qDebug()<<"添加顶点："<<savePoint.x();
            addVertex(savePoint);
            startCapturing();

        }
        else if(e->button()==Qt::RightButton)
        {
            qDebug()<<"直线右键";
            deleteTempRubberBand();
            if ( m_captureMode == CaptureLine && capture_point_List.size() < 2 )  {return;}
            if ( m_captureMode == CapturePolygon && capture_point_List.size() < 3 ) { return; }

            QgsFeature* feature = new QgsFeature( curLayer->fields(), 0 );
            QgsGeometry* g = 0; // 新建一个geometry

            if ( m_captureMode == CaptureLine )
            {

                if (curLayer->geometryType() == QgsWkbTypes::LineGeometry )// layer->wkbType() == QGis::WKBLineString || layer->wkbType() == QGis::WKBLineString25D
                {
//                    qDebug()<<"添加线";
                    g = new QgsGeometry(QgsGeometry::fromPolyline( capture_point_List.toVector() ));
                }
                else
                {
//                    qDebug()<<123;
//                    emit messageEmitted( tr( "Cannot add feature. Unknown WKB type" ), QgsMessageBar::CRITICAL );
                    stopCapturing();
//                    delete feature;
                    return;
                }
                feature->setGeometry( *g );

            }
            else if(m_captureMode == CapturePolygon)
            {
                if ( curLayer->geometryType()==QgsWkbTypes::PolygonGeometry )//layer->wkbType() == QGis::WKBPolygon ||  layer->wkbType() == QGis::WKBPolygon25D
                {
                    QPolygonF polygon;
                    foreach (const QgsPoint& point, capture_point_List.toVector()) {
                        polygon << QPointF(point.x(), point.y());
                    }
                    polygon<<QPointF(capture_point_List.first().x(), capture_point_List.first().y());


                    QgsGeometry *temp_g= new QgsGeometry(QgsGeometry::fromQPolygonF(polygon));//fromPointXY
                    QString res=temp_g->asWkt();
                    res.replace("LineString","Polygon");
                    qDebug()<<res;
                    g=new QgsGeometry(QgsGeometry::fromWkt(res));
//                    QgsWkbTypes
//                    qDebug()<<g->type();

//                    g = new QgsGeometry( QgsGeometry::fromQPolygonF( QgsPolygon() << captureList.toVector() ));
                }
                else
                {
//                    emit messageEmitted( tr( "Cannot add feature. Unknown WKB type" ), QgsMessageBar::CRITICAL );
                    stopCapturing();
                    delete feature;
                    return;
                }
                if ( !g )
                {
                    stopCapturing();
                    delete feature;
                    return; // invalid geometry; one possibility is from duplicate points
                }
                feature->setGeometry( *g );
            }
            addFeature(curLayer,feature);
            stopCapturing();
        }
    }

}

int MyFeatureTool::addFeature(QgsVectorLayer*layer,QgsFeature * feature)
{
//    qDebug()<<"添加点";
//    qDebug()<<layer->featureCount();
    if(!layer || !layer->isEditable()) return -1;

    QgsVectorDataProvider *provider = layer->dataProvider();
    qDebug()<<feature->geometry().type();
    //填充默认属性
//    const QgsFields& fields = layer->fields();
//    feature->initAttributes( fields.count() );
//    for ( int idx = 0; idx < fields.count(); ++idx )
//    {
//        QVariant v = provider->defaultValue( idx );
//        feature->setAttribute( idx, v );
//    }

//    qDebug()<<"添加成功！";
    provider->addFeature(*feature);
    mCanvas->refresh();
//    layer->commitChanges(true);



}
void MyFeatureTool::deleteFeature(QgsFeatureId id)
{
    QgsVectorLayer *curLayer=qobject_cast<QgsVectorLayer *>(mCanvas->currentLayer());
    if(!curLayer->isEditable())
    {
        curLayer->startEditing();
    }
    curLayer->deleteFeature(id);
    curLayer->commitChanges(true);
}
void MyFeatureTool::startCapturing()
{
    mCapturing = true;
}
void MyFeatureTool::addVertex(const QgsPoint &point)
{
    qDebug()<<point.x()<<point.y();
    if(Mode()==CaptureMode::CaptureNone) return;
//    qDebug()<<2;
    int res=nextPoint(point);
    if(res!=0) return;
//    qDebug()<<3;
    if ( !mRubberBand ) // 没有rubber band，就创建一个
    {
        mRubberBand = createRubberBand( m_captureMode == CapturePolygon ? QgsWkbTypes::PolygonGeometry: QgsWkbTypes::LineGeometry,true );
    }
    mRubberBand->addPoint(QgsPointXY(point.x(),point.y()) );
    capture_point_List.append( point );
    if ( !mTempRubberBand )
    {
        mTempRubberBand = createRubberBand( m_captureMode == CapturePolygon ? QgsWkbTypes::PolygonGeometry: QgsWkbTypes::LineGeometry,true );
    }
    else
    {
        if(m_captureMode == CapturePolygon){mTempRubberBand->reset();};

//        mTempRubberBand->reset( m_captureMode == CapturePolygon ? true : false );
    }
    if ( m_captureMode == CaptureLine )
    {
        mTempRubberBand->addPoint( point );
    }
    else if ( m_captureMode == CapturePolygon )
    {
        const QgsPoint firstPoint = QgsPoint(mRubberBand->getPoint( 0, 0 )->x(),mRubberBand->getPoint( 0, 0 )->y());
        mTempRubberBand->addPoint( firstPoint );
        mTempRubberBand->movePoint( point );
        mTempRubberBand->addPoint( point );
    }
//    validateGeometry(); // 验证几何有效性



}
int MyFeatureTool::nextPoint(const QgsPoint &point)
{
    QgsVectorLayer *curLayer=qobject_cast<QgsVectorLayer *>(mCanvas->currentLayer());
    if(!curLayer) return -1;

    return 0;
}
QgsRubberBand* MyFeatureTool::createRubberBand( QgsWkbTypes::GeometryType geometryType,bool alternativeBand )//QgsWkbTypes geometryType /*= QGis::Line*/, bool alternativeBand /*= false */
{
    QSettings settings;
    QgsRubberBand* rb = new QgsRubberBand( mCanvas );
    rb->setWidth( settings.value( "/qgis/digitizing/line_width", 1 ).toInt() );
    QColor color( settings.value( "/qgis/digitizing/line_color_red", 255 ).toInt(),
                  settings.value( "/qgis/digitizing/line_color_green", 0 ).toInt(),
                  settings.value( "/qgis/digitizing/line_color_blue", 0 ).toInt() );
    double myAlpha = settings.value( "/qgis/digitizing/line_color_alpha", 200 ).toInt() / 255.0;
    if ( alternativeBand )
    {
        myAlpha = myAlpha * settings.value( "/qgis/digitizing/line_color_alpha_scale", 0.75 ).toDouble();
        rb->setLineStyle( Qt::DotLine );
    }
    if (geometryType  ==  QgsWkbTypes::PolygonGeometry )
    {
        color.setAlphaF( myAlpha );
    }
    color.setAlphaF( myAlpha );
    rb->setColor( color );
    rb->show();
    return rb;
}
void MyFeatureTool::deleteTempRubberBand()
{
    if ( mTempRubberBand )
    {
        delete mTempRubberBand;
        mTempRubberBand = 0;
    }
}

void MyFeatureTool::stopCapturing()
{
    if ( mRubberBand )
    {
        delete mRubberBand;
        mRubberBand = 0;
    }
    if ( mTempRubberBand )
    {
        delete mTempRubberBand;
        mTempRubberBand = 0;
    }
//    while ( !mGeomErrorMarkers.isEmpty() )
//    {
//        delete mGeomErrorMarkers.takeFirst();
//    }
//    mGeomErrors.clear();
//    mCapturing = false;
    capture_point_List.clear();
    mCanvas->refresh();
}
