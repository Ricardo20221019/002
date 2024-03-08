#include "myaddfeaturetool.h"


#include "myaddfeaturetool.h".h"
//#include "qgis_dev.h"
#include <QStringList>

#include <qgsvectorlayer.h>
#include <qgslogger.h>
#include <qgsvectordataprovider.h>
//#include "qgscsexception.h"
#include "qgsproject.h"
//#include "qgsmaplayerregistry.h"

#include "qgsmaptopixel.h"
#include "qgsgeometry.h"
#include "qgsfeature.h"



MyAddFeatureTool::MyAddFeatureTool( QgsMapCanvas* mapCanvas )
    : QgsMapTool( mapCanvas )
{
    mToolName = tr( "Add Feature" );
    mRubberBand = 0;
    mTempRubberBand = 0;
    mValidator = 0;
    qDebug()<<"启动工具";
}

MyAddFeatureTool::~MyAddFeatureTool()
{

}

void MyAddFeatureTool::canvasPressEvent(QgsMapMouseEvent * e)
{
    qDebug()<<e->x()<<" "<<e->y();
}
bool MyAddFeatureTool::addFeature( QgsVectorLayer *layer, QgsFeature *feature, bool showModal /*= true */ )
{
    if ( !layer || !layer->isEditable() ) {return false;}

    QgsAttributeMap defaultAttributes;

    QgsVectorDataProvider *provider = layer->dataProvider();

    QSettings settings;
    bool reuseLastValues = settings.value( "/qgis/digitizing/reuseLastValues", false ).toBool();
    QgsDebugMsg( QString( "reuseLastValues: %1" ).arg( reuseLastValues ) );

    // add the fields to the QgsFeature

    const QgsFields& fields = layer->fields();

    feature->initAttributes( fields.count() );
    for ( int idx = 0; idx < fields.count(); ++idx )
    {
        QVariant v = provider->defaultValue( idx );
        feature->setAttribute( idx, v );
    }

    //show the dialog to enter attribute values
    //only show if enabled in settings and layer has fields
    bool isDisabledAttributeValuesDlg = ( fields.count() == 0 ) || settings.value( "/qgis/digitizing/disable_enter_attribute_values_dialog", false ).toBool();

    // override application-wide setting with any layer setting
//    layer->featureBlendMode()
//    switch ( layer->featureFormSuppress() )
//    {
//    case QgsVectorLayer::SuppressOn:
//        isDisabledAttributeValuesDlg = true;
//        break;
//    case QgsVectorLayer::SuppressOff:
//        isDisabledAttributeValuesDlg = false;
//        break;
//    case QgsVectorLayer::SuppressDefault:
//        break;
//    }
    if ( isDisabledAttributeValuesDlg )
    {
        layer->beginEditCommand( "" );
        bool mFeatureSaved = layer->addFeature( *feature );

        if ( mFeatureSaved )
        {
            layer->endEditCommand();
        }
        else
        {
            layer->destroyEditCommand();
        }
    }
    else
    {
        // 这里添加代码，做增加要素时填写属性

    }
}

void MyAddFeatureTool::canvasReleaseEvent( QgsMapMouseEvent* e )
{
    qDebug()<<e->x()<<e->x();
    // 获取当前图层
    QgsVectorLayer* layer = qobject_cast<QgsVectorLayer*>( mCanvas->currentLayer() );

//    qobject_cast<QgsVectorLayer*>( mCanvas->layers() );

    // 判断当前图层是否为矢量图层
    if( !layer ) {emit messageEmitted( tr( "not a valid vector layer." ) ); return;}

    // 判断数据驱动状态
    QgsVectorDataProvider* provider = layer->dataProvider();
    if ( !( provider->capabilities() & QgsVectorDataProvider::AddFeatures ) )
    {
//        emit messageEmitted(
//            tr( "The data provider for this layer does not support the addition of features." ),
//            QgsMessageBar::WARNING );
        return;
    }

    // 判断当前图层是否可编辑
    if( !layer->isEditable() ) {emit messageEmitted( tr( "can't edit this layer." ) ); return;}

    // 得到点坐标，转换为地图坐标
    QgsPoint savePoint;
    qDebug()<<savePoint.x()<<savePoint.y();
    try
    {
        QgsPointXY temp_point=toLayerCoordinates( layer, mCanvas->mapSettings().mapToPixel().toMapCoordinates( e->pos() ) );
        savePoint.setX(temp_point.x());
        savePoint.setY(temp_point.y());
        QgsDebugMsg( "savePoint = " + savePoint.toString() );
    }
    catch ( QgsCsException &cse )
    {
        Q_UNUSED( cse );
//        emit messageEmitted( tr( "Cannot transform the point to the layers coordinate system" ), QgsMessageBar::WARNING );
        return;
    }


    switch( layer->geometryType() )
    {
    case QgsWkbTypes::PointGeometry:
        m_captureMode = CapturePoint;
        break;
    case QgsWkbTypes::LineGeometry:
        m_captureMode = CaptureLine;
        break;
    case QgsWkbTypes::PolygonGeometry:
        m_captureMode = CapturePolygon;
        break;
    default:
        break;
    }

    QgsGeometry* g = 0; // 新建一个geometry
    if ( m_captureMode == CapturePoint )
    {
        // 转换为geometry
//        if ( layer->wkbType() == QGis::WKBPoint || layer->wkbType() == QGis::WKBPoint25D )//layer->wkbType() == QGis::WKBPoint || layer->wkbType() == QGis::WKBPoint25D
//        {
//            g = QgsGeometry::fromPoint( savePoint );
//        }
//        else if( layer->wkbType() == QGis::WKBMultiPoint || layer->wkbType() == QGis::WKBMultiPoint25D )
//        {
//            g = QgsGeometry::fromMultiPoint( QgsMultiPoint() << savePoint );
//        }
        g = new QgsGeometry(QgsGeometry::fromPointXY(QgsPointXY(savePoint.x(),savePoint.y())));

        // 转换为feature
        QgsFeature feature( layer->fields(), 0 );
        feature.setGeometry( *g );
        addFeature( layer, &feature, false );
        //layer->addFeature( feature, true );
        mCanvas->setExtent( layer->extent() );
        mCanvas->refresh();
    }
    else if ( m_captureMode == CaptureLine || m_captureMode == CapturePolygon )
    {
        if ( e->button() == Qt::LeftButton ) // 鼠标左键
        {
            QgsPointXY xypoint=mCanvas->mapSettings().mapToPixel().toMapCoordinates( e->pos() );
            QgsPoint temp_point(xypoint.x(),xypoint.y());

            int error = addVertex( temp_point );
            if ( error == 1 ) {return;} // current layer is not a vector layer
            else if ( error == 2 ) // problem with coordinate transformation
            {
//                emit messageEmitted( tr( "Cannot transform the point to the layers coordinate system" ), QgsMessageBar::WARNING );
                return;
            }
            startCapturing();
        }
        else if ( e->button() == Qt::RightButton ) // 鼠标右键
        {
            deleteTempRubberBand();
            if ( m_captureMode == CaptureLine && captureList.size() < 2 ) { return; }
            if ( m_captureMode == CapturePolygon && captureList.size() < 3 ) { return; }

            QgsFeature* feature = new QgsFeature( layer->fields(), 0 );
            QgsGeometry* g = 0; // 新建一个geometry
            if ( m_captureMode == CaptureLine )
            {
                if (layer->wkbType() == QgsWkbTypes::LineGeometry )// layer->wkbType() == QGis::WKBLineString || layer->wkbType() == QGis::WKBLineString25D
                {
                    g = new QgsGeometry(QgsGeometry::fromPolyline( captureList.toVector() ));
                }
                else
                {
//                    emit messageEmitted( tr( "Cannot add feature. Unknown WKB type" ), QgsMessageBar::CRITICAL );
                    stopCapturing();
                    delete feature;
                    return;
                }
                feature->setGeometry( *g );
            }
            else if ( m_captureMode == CapturePolygon )
            {
                if ( layer->wkbType()==QgsWkbTypes::PolygonGeometry )//layer->wkbType() == QGis::WKBPolygon ||  layer->wkbType() == QGis::WKBPolygon25D
                {
                    QPolygonF polygon;
                    foreach (const QgsPoint& point, captureList.toVector()) {
                        polygon << QPointF(point.x(), point.y());
                    }
                    g = new QgsGeometry(QgsGeometry::fromQPolygonF(polygon));
//                    g = new QgsGeometry( QgsGeometry::fromQPolygonF( QgsPolygon() << captureList.toVector() ));
                }
//                else if ( layer->wkbType() == QGis::WKBMultiPolygon ||  layer->wkbType() == QGis::WKBMultiPolygon25D )
//                {
//                    g = QgsGeometry::fromMultiPolygon( QgsMultiPolygon() << ( QgsPolygon() << m_captureList.toVector() ) );
//                }
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
//                qobject_cast<QHash<QgsVectorLayer *, QSet<QgsFeatureId> >>(mCanvas);
//                const QHash<QgsVectorLayer *, QSet<QgsFeatureId> > &ignoreFeatures=new QHash<QgsVectorLayer *, QSet<QgsFeatureId> >();
//                feature->geometry().avoidIntersections(qobject_cast<QgsVectorLayer*>( mCanvas->layers() ));

//                int avoidIntersectionsReturn = feature->geometry()->avoidIntersections();
//                if ( avoidIntersectionsReturn == 1 )
//                {
//                    //not a polygon type. Impossible to get there
//                }
//#if 0
//                else if ( avoidIntersectionsReturn == 2 ) //MH120131: disable this error message until there is a better way to cope with the single type / multi type problem
//                {
//                    //bail out...
//                    emit messageEmitted( tr( "The feature could not be added because removing the polygon intersections would change the geometry type" ), QgsMessageBar::CRITICAL );
//                    delete feature;
//                    stopCapturing();
//                    return;
//                }
//#endif
//                else if ( avoidIntersectionsReturn == 3 )
//                {
////                    emit messageEmitted( tr( "An error was reported during intersection removal" ), QgsMessageBar::CRITICAL );
//                }

//                if ( !feature->geometry()->asWkb() ) //avoid intersection might have removed the whole geometry
//                {
//                    QString reason;
//                    if ( avoidIntersectionsReturn != 2 )
//                    {
//                        reason = tr( "The feature cannot be added because it's geometry is empty" );
//                    }
//                    else
//                    {
//                        reason = tr( "The feature cannot be added because it's geometry collapsed due to intersection avoidance" );
//                    }
////                    emit messageEmitted( reason, QgsMessageBar::CRITICAL );
//                    delete feature;
//                    stopCapturing();
//                    return;
//                }
            }

            if ( addFeature( layer, feature, false ) )
            {
                //add points to other features to keep topology up-to-date
                int topologicalEditing = QgsProject::instance()->readNumEntry( "Digitizing", "/TopologicalEditing", 0 );

                //use always topological editing for avoidIntersection.
                //Otherwise, no way to guarantee the geometries don't have a small gap in between.
                QStringList intersectionLayers = QgsProject::instance()->readListEntry( "Digitizing", "/AvoidIntersectionsList" );
                bool avoidIntersection = !intersectionLayers.isEmpty();
                if ( avoidIntersection ) //try to add topological points also to background layers
                {
                    QStringList::const_iterator lIt = intersectionLayers.constBegin();
                    for ( ; lIt != intersectionLayers.constEnd(); ++lIt )
                    {

                        QgsProject *project = QgsProject::instance();
                        QgsMapLayer *ml = project->mapLayer(*lIt);
//                        QgsMapLayer* ml = QgsMapLayerRegistry::instance()->mapLayer( *lIt );

                        QgsVectorLayer* vl = qobject_cast<QgsVectorLayer*>( ml );
                        //can only add topological points if background layer is editable...
                        if ( vl && vl->geometryType() == QgsWkbTypes::PolygonGeometry && vl->isEditable() )
                        {
                            vl->addTopologicalPoints( feature->geometry() );
                        }
                    }
                }
                else if ( topologicalEditing )
                {
                    layer->addTopologicalPoints( feature->geometry() );
                }
            }
            stopCapturing();
        }
    }
}

void MyAddFeatureTool::activate()
{
    QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( mCanvas->currentLayer() );
    if ( layer && layer->geometryType() == QgsWkbTypes::NoGeometry )
    {
        QgsFeature f;
        addFeature( layer, &f, false );
        return;
    }
    QgsMapTool::activate();
}
void MyAddFeatureTool::notifyNotVectorLayer()
{
    emit messageEmitted( tr( "No active vector layer" ) );
}
void MyAddFeatureTool::notifyNotEditableLayer()
{
    emit messageEmitted( tr( "Layer not editable" ) );
}
CaptureMode MyAddFeatureTool::getMode()
{
    return m_captureMode;
}
int MyAddFeatureTool::addVertex( const QgsPoint& point )
{
    if ( getMode() == CaptureNone ) { QgsDebugMsg( "invalid capture mode" ); return 2;}
    QgsPoint layerPoint;
    int res = nextPoint( point, layerPoint );
    if ( res != 0 ) {return res;} // 当前点必须是最后一个点
    if ( !mRubberBand ) // 没有rubber band，就创建一个
    {
        mRubberBand = createRubberBand( m_captureMode == CapturePolygon ? QgsWkbTypes::PolygonGeometry: QgsWkbTypes::LineGeometry,true );
    }
    mRubberBand->addPoint( point );
    captureList.append( layerPoint );
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
    validateGeometry(); // 验证几何有效性
    return 0;
}
void MyAddFeatureTool::startCapturing()
{
    mCapturing = true;
}
void MyAddFeatureTool::deleteTempRubberBand()
{
    if ( mTempRubberBand )
    {
        delete mTempRubberBand;
        mTempRubberBand = 0;
    }
}
void MyAddFeatureTool::stopCapturing()
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
    while ( !mGeomErrorMarkers.isEmpty() )
    {
        delete mGeomErrorMarkers.takeFirst();
    }
    mGeomErrors.clear();
    mCapturing = false;
    captureList.clear();
    mCanvas->refresh();
}
int MyAddFeatureTool::nextPoint( const QgsPoint& mapPoint, QgsPoint& layerPoint )
{
    QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( mCanvas->currentLayer() );
    if ( !vlayer ) { QgsDebugMsg( "no vector layer" ); return 1;}
    try
    {
        QgsPointXY temp_point=toLayerCoordinates( vlayer, QPoint(mapPoint.x(),mapPoint.y()) ); //transform snapped point back to layer crs
        layerPoint.setX(temp_point.x());
        layerPoint.setY(temp_point.y());


    }
    catch ( QgsCsException &cse )
    {
        Q_UNUSED( cse );
        QgsDebugMsg( "transformation to layer coordinate failed" );
        return 2;
    }
    return 0;
}
QgsRubberBand* MyAddFeatureTool::createRubberBand( QgsWkbTypes::GeometryType geometryType,bool alternativeBand )//QgsWkbTypes geometryType /*= QGis::Line*/, bool alternativeBand /*= false */
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
void MyAddFeatureTool::validateGeometry()
{
    QSettings settings;
    if ( settings.value( "/qgis/digitizing/validate_geometries", 1 ).toInt() == 0 ) {return;}
    if ( mValidator )
    {
        mValidator->deleteLater();
        mValidator = 0;
    }
    mTip = "";
    mGeomErrors.clear();
    while ( !mGeomErrorMarkers.isEmpty() )
    {
        delete mGeomErrorMarkers.takeFirst();
    }
    QgsGeometry *g = 0;
    switch ( m_captureMode )
    {
    case CaptureNone:
    case CapturePoint:
        return;
    case CaptureLine:
        if ( captureList.size() < 2 ) {return;}
        g =new QgsGeometry( QgsGeometry::fromPolyline( captureList.toVector() ));
        break;
    case CapturePolygon:
        if ( captureList.size() < 3 ) {return;}
        QPolygonF polygon;
        foreach (const QgsPoint& point, captureList.toVector()) {
            polygon << QPointF(point.x(), point.y());
        }
        g = new QgsGeometry(QgsGeometry::fromQPolygonF(polygon));
//        g =new QgsGeometry(  QgsGeometry::fromQPolygonF( QgsPolygon() << ( QgsPolyline() << captureList.toVector() << m_captureList[0] ) ));
        break;
    }
    if ( !g ) {return;}
    mValidator = new QgsGeometryValidator( *g );
    connect( mValidator, SIGNAL( errorFound( QgsGeometry::Error ) ), this, SLOT( addError( QgsGeometry::Error ) ) );
    connect( mValidator, SIGNAL( finished() ), this, SLOT( validationFinished() ) );
    mValidator->start();
//    QStatusBar *sb = qgis_dev::instance()->statusBar();
//    sb->showMessage( tr( "Validation started." ) );
    delete g;
}
