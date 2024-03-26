#include "myfeaturetool.h"

MyFeatureTool::MyFeatureTool(QgsMapCanvas* canvas)
    :QgsMapTool(canvas)
{
    mCapturing=false;
    mRubberBand = 0;
    mTempRubberBand = 0;
    select_Tool=new QgsMapToolIdentify(canvas);
    gdal_tool=new MyGDALTool();

}
MyFeatureTool::~MyFeatureTool()
{

}

void MyFeatureTool::canvasPressEvent( QgsMapMouseEvent* e )
{
    clearHighLight();

    if(tool_status==DrawingMode::SelectionMode)
    {

        QList<QgsMapToolIdentify::IdentifyResult> features=select_Tool->identify(e->x(),e->y(),my_layers,QgsMapToolIdentify::TopDownStopAtFirst);
        if(!features.empty())
        {
            moving=true;
            all_layers[0]->startEditing();
            all_layers[1]->startEditing();
            setHightLight(features.first().mFeature);
//            qDebug()<<"当前选中要素 type："<<features.first().mFeature.geometry().type()<<"  wkbType："<<features.first().mFeature.geometry().wkbType();
            if(features.first().mFeature.geometry().type()!=QgsWkbTypes::PolygonGeometry)
            {
                moving_feature_id=features.first().mFeature.id();
            }
//            qDebug()<<features.first().mFeature.attributes();
            if(features.first().mFeature.geometry().type()==QgsWkbTypes::PointGeometry)
            {
                qDebug()<<"选中点id:"<<features.first().mFeature.id();
                qDebug()<<"选中点的坐标："<<all_points[features.first().mFeature.id()].x<<" "<<all_points[features.first().mFeature.id()].y;


//                qDebug()<<"选中点id:"<<features.first().mFeature.id()<<" feature_id"<<all_points[features.first().mFeature.id()].feature_id
//                       <<" lines:"<<all_points[features.first().mFeature.id()].lines;
            }
            else if(features.first().mFeature.geometry().type()==QgsWkbTypes::LineGeometry)
            {
                qDebug()<<"选中直线id:"<<features.first().mFeature.id()<<" length:"<<all_lines[features.first().mFeature.id()].length;
                qDebug()<<"src:"<<all_lines[features.first().mFeature.id()].src_feature_id<< " dst:"<<all_lines[features.first().mFeature.id()].dst_feature_id;
//                if(all_lines[features.first().mFeature.id()].control_point_id!=-1)
//                {
//                    qDebug()<<"选中曲线id:"<<features.first().mFeature.id()<<" feature_id"<<all_lines[features.first().mFeature.id()].feature_id
//                           <<" src_id:"<<all_lines[features.first().mFeature.id()].src_feature_id<<" dst_id"<<all_lines[features.first().mFeature.id()].dst_feature_id
//                          <<" control_point"<<all_control_points[features.first().mFeature.id()].x() << all_control_points[features.first().mFeature.id()].y();
//                }
//                else
//                {
//                    qDebug()<<"选中直线id:"<<features.first().mFeature.id()<<" feature_id"<<all_lines[features.first().mFeature.id()].feature_id
//                           <<" src_id:"<<all_lines[features.first().mFeature.id()].src_feature_id<<" dst_id"<<all_lines[features.first().mFeature.id()].dst_feature_id;
//                }

            }
//            QgsWkbTypes::MultiLineString




        }

    }
    else if(tool_status==DrawingMode::FindPath)
    {
        if(finding_status)
        {
            QgsPointXY temp_point=toLayerCoordinates( all_layers[0], mCanvas->mapSettings().mapToPixel().toMapCoordinates( e->pos() ) );
            QgsPoint savePoint;
            savePoint.setX(temp_point.x());
            savePoint.setY(temp_point.y());
            qDebug()<<"当前坐标："<<temp_point.x()<<temp_point.y();
            QList<QgsMapToolIdentify::IdentifyResult> features=select_Tool->identify(e->x(),e->y(),my_layers,QgsMapToolIdentify::TopDownStopAtFirst);
            if(!find_status)//false 设置起点
            {

                if(features.size()>0 && features.first().mFeature.geometry().type()==QgsWkbTypes::PointGeometry)
                {
                    gdal_tool->setTargetPointId(features.first().mFeature.id(),0);
    //                features.size()>0 && features.first().mFeature.geometry()
                }
                else//插点
                {
                    qDebug()<<"先外点："<<savePoint.x()<<" "<<savePoint.y();
                    gdal_tool->setFindPathPoint(savePoint.x(),savePoint.y(),0);//设置线段外起点
                    setStartTestHighLight(savePoint);
//                    insertPoint(0);





    //                features.first().mFeature.geometry().closestSegmentWithContext(savePoint);

                }
    //            emit findPathStartPoint(temp_point.x(),temp_point.y());
    //            gdal_tool->setFindPathStartPoint(temp_point.x(),temp_point.y());
    //            all_layers[1]->getFeature(0).geometry().closestSegmentWithContext()
                find_status=true;
    //            clearTestHighLight();

            }
            else//true 设置终点
            {

                if(features.size()>0 && features.first().mFeature.geometry().type()==QgsWkbTypes::PointGeometry)
                {
                    gdal_tool->setTargetPointId(features.first().mFeature.id(),1);
                }
                else//插点
                {
                    gdal_tool->setFindPathPoint(savePoint.x(),savePoint.y(),1);//设置线段外目标点
                    setTargetTestHighLight(savePoint);

//                    insertPoint(1);
                }

                find_status=false;
                finding_status=false;


            }

        }




    }
    else
    {

//        QDateTime timeCurrent4 = QDateTime::currentDateTime(); // 获取当前时间
//        QString timeStr4 = timeCurrent4.toString("[yy-MM-dd hh:mm:ss.zzz]"); // 转换自定义格式精确到毫秒
//        qDebug()<<"4:"<<timeStr4;

        QgsPointXY temp_point=toLayerCoordinates( all_layers[0], mCanvas->mapSettings().mapToPixel().toMapCoordinates( e->pos() ) );

        QgsPoint savePoint;
        savePoint.setX(temp_point.x());
        savePoint.setY(temp_point.y());

        QList<QgsMapToolIdentify::IdentifyResult> features=select_Tool->identify(e->x(),e->y(),my_layers);

//        QDateTime timeCurrent1 = QDateTime::currentDateTime(); // 获取当前时间
//        QString timeStr1 = timeCurrent1.toString("[yy-MM-dd hh:mm:ss.zzz]"); // 转换自定义格式精确到毫秒
//        qDebug()<<"1:"<<timeStr1;
        if(features.size()>0)
        {
            qDebug()<<"出现覆盖"<<features.size();
            if(features.first().mFeature.geometry().type()==QgsWkbTypes::PointGeometry
                    && tool_status!=DrawingMode::AreaMode)
            {
                if(e->button()==Qt::LeftButton)
                {
                    if(capture_point_List.size()<2)
                    {
                        capture_point_List.append(savePoint);
                        capture_point_ids.push_back(features.first().mFeature.id());
                        if(capture_point_List.size()==2)
                        {
                            qDebug()<<"连接";
                            drawLineOrCurve();
                            stopCapturing();
                            mCanvas->refresh();
                            all_layers[0]->commitChanges(true);
                            all_layers[1]->commitChanges(true);

                        }
                    }

                }
            }
            return;
        }

//        QDateTime timeCurrent2 = QDateTime::currentDateTime(); // 获取当前时间
//        QString timeStr2 = timeCurrent2.toString("[yy-MM-dd hh:mm:ss.zzz]"); // 转换自定义格式精确到毫秒
//        qDebug()<<"2:"<<timeStr2;
        if(tool_status==DrawingMode::LineMode || tool_status==DrawingMode::CurveMode)
        {
            all_layers[0]->startEditing();
            all_layers[1]->startEditing();
            if(e->button()==Qt::LeftButton)
            {
                startCapturing();
                capture_point_List.append( savePoint );
                addPoint(savePoint);
                if(capture_point_List.size()>=2)//创建线
                {
                    drawLineOrCurve();
                }

            }
            else if(e->button()==Qt::RightButton)
            {
                stopCapturing();
            }
//            QDateTime timeCurrent3 = QDateTime::currentDateTime(); // 获取当前时间
//            QString timeStr3 = timeCurrent3.toString("[yy-MM-dd hh:mm:ss.zzz]"); // 转换自定义格式精确到毫秒
//            qDebug()<<"3:"<<timeStr3;
            all_layers[0]->commitChanges(true);
            all_layers[1]->commitChanges(true);
//            QDateTime timeCurrent4 = QDateTime::currentDateTime(); // 获取当前时间
//            QString timeStr4 = timeCurrent4.toString("[yy-MM-dd hh:mm:ss.zzz]"); // 转换自定义格式精确到毫秒
//            qDebug()<<"4:"<<timeStr4;
            mCanvas->refresh();

        }
        else if(tool_status==DrawingMode::AreaMode)
        {
            all_layers[2]->startEditing();
            if(e->button()==Qt::LeftButton)
            {
                //qDebug()<<"添加顶点："<<savePoint.x();
                addVertex(savePoint);
                startCapturing();

            }
            else if(e->button()==Qt::RightButton)
            {
                if(capture_point_List.size()<3) return;
                QgsFeature* feature = new QgsFeature( all_layers[2]->fields(), 0 );
                QgsGeometry* g = 0; // 新建一个geometry
                QPolygonF polygon;
                foreach (const QgsPoint& point, capture_point_List.toVector()) {
                    polygon << QPointF(point.x(), point.y());
                }
                polygon<<QPointF(capture_point_List.first().x(), capture_point_List.first().y());
                QgsGeometry *temp_g= new QgsGeometry(QgsGeometry::fromQPolygonF(polygon));//fromPointXY
                QString res=temp_g->asWkt();
                res.replace("LineString","Polygon");

                g=new QgsGeometry(QgsGeometry::fromWkt(res));
                feature->setGeometry( *g );


//                int area_feature_id=all_areas.empty()?0:all_areas[all_areas.lastKey()].attribute_id+1;

//                feature->setAttribute("id",area_feature_id);
                QString points_str=feature->attribute("points").toString();
                for(auto it:capture_point_ids)
                {
                    if(points_str.size()==0)
                    {
                        points_str=QString::number(it);
                    }
                    else
                    {
                        points_str+=" "+QString::number(it);
                    }
                }
                feature->setAttribute("points",points_str);
                addFeature(all_layers[2],feature);
//                all_layers[2]->addFeature(*feature);
                stopCapturing();

                PolgonFeature temp_area;
//                temp_area.attribute_id=area_feature_id;
                temp_area.feature_id=feature->id();
                temp_area.points=points_str;

                all_areas[feature->id()]=temp_area;

            }
        }
    }


    mCanvas->refresh();
}
//! 重写鼠标指针释放事件
void MyFeatureTool::canvasReleaseEvent( QgsMapMouseEvent* e )
{
    if(tool_status==DrawingMode::SelectionMode)
    {
        moving=false;
        moving_feature_id=-1;
//        all_layers[0]->commitChanges(true);
//        all_layers[1]->commitChanges(true);
    }
}


void MyFeatureTool::canvasMoveEvent( QgsMapMouseEvent *e )
{
//    qDebug()<<"移动要素："<<moving_feature_id;
    if(moving && select_feature_id!=-1)
    {
        QgsPointXY temp_point=toLayerCoordinates( all_layers[0], mCanvas->mapSettings().mapToPixel().toMapCoordinates( e->pos() ) );
        QgsPoint savePoint(temp_point.x(),temp_point.y());
        if(select_feature_type==QgsWkbTypes::PointGeometry)//选中点
        {
            QgsGeometry new_point(QgsGeometry::fromPointXY(temp_point));
            all_layers[0]->getFeature(select_feature_id).clearGeometry();

            all_layers[0]->changeGeometry(select_feature_id,new_point,true);
            mCanvas->refresh();
            all_points[select_feature_id].x=savePoint.x();
            all_points[select_feature_id].y=savePoint.y();//更新缓存坐标

//            qDebug()<<"更新点";
            clearHighLight();
            setHightLight(all_layers[0]->getFeature(moving_feature_id));//更新高亮



            QgsGeometry new_line_geometry;
//            qDebug()<<"select:"<<select_feature_id<<" "<<"moving:"<<moving_feature_id;
            //获取所有与点相连接的线
            for(auto it:all_points[select_feature_id].lines)
            {
    //            int change_index=-1;
                if(all_lines[it].src_feature_id==select_feature_id)//选中起点
                {

                    QgsPoint dst_point(all_points[all_lines[it].dst_feature_id].x,all_points[all_lines[it].dst_feature_id].y);//终点不变
                    capture_point_List<<savePoint<<dst_point;
    //                qDebug()<<savePoint.x()<<savePoint.y()<<" "<<no_change_point.x()<<no_change_point.y();
                    if(all_lines[it].control_point_id==-1)
                    {
                        new_line_geometry=QgsGeometry::fromPolyline( capture_point_List.toVector() );

                    }
                    else
                    {
                        QgsPoint control_point((savePoint.x()+dst_point.x())*0.5,(savePoint.y()+dst_point.y())*0.5+10);
                        QgsCircularString *curve1=new QgsCircularString(savePoint,control_point,dst_point);

                        new_line_geometry.set(curve1);
                    }


                }
                else if(all_lines[it].dst_feature_id==select_feature_id)//选中终点
                {
                    QgsPoint src_point(all_points[all_lines[it].src_feature_id].x,all_points[all_lines[it].src_feature_id].y);//起点不变
                    capture_point_List<<savePoint<<src_point;
    //                qDebug()<<savePoint.x()<<savePoint.y()<<" "<<no_change_point.x()<<no_change_point.y();
                    if(all_lines[it].control_point_id==-1)
                    {
                        new_line_geometry=QgsGeometry::fromPolyline( capture_point_List.toVector() );
                    }
                    else
                    {
                        QgsPoint control_point((src_point.x()+savePoint.x())*0.5,(src_point.y()+savePoint.y())*0.5+10);
                        QgsCircularString *curve1=new QgsCircularString(src_point,control_point,savePoint);
                        new_line_geometry.set(curve1);

                    }
                }

                all_layers[1]->changeGeometry(it,new_line_geometry,true);
                all_lines[it].length=new_line_geometry.length();//更新长度
                mCanvas->refresh();
                capture_point_List.clear();
            }
        }
        else if(select_feature_type==QgsWkbTypes::LineGeometry)
        {
            if(all_lines[select_feature_id].control_point_id!=-1)
            {
                clearHighLight();

                QgsPoint src_point(all_points[all_lines[moving_feature_id].src_feature_id].x,all_points[all_lines[moving_feature_id].src_feature_id].y);
                QgsPoint dst_point(all_points[all_lines[moving_feature_id].dst_feature_id].x,all_points[all_lines[moving_feature_id].dst_feature_id].y);
                QgsCircularString *curve1=new QgsCircularString(src_point,savePoint,dst_point);
                QgsGeometry newgeometry(curve1);
//                all_lines[moving_feature_id].feature.setGeometry(newgeometry);
                all_layers[1]->changeGeometry(moving_feature_id,newgeometry,true);
                all_lines[moving_feature_id].length=newgeometry.length();
                setHightLight(all_layers[1]->getFeature(moving_feature_id));
            }
        }

    }

    mCanvas->refresh();



}
int MyFeatureTool::addFeature(QgsVectorLayer*layer,QgsFeature * feature)
{
//    QDateTime timeCurrent4 = QDateTime::currentDateTime(); // 获取当前时间
//    QString timeStr4 = timeCurrent4.toString("[yy-MM-dd hh:mm:ss.zzz]"); // 转换自定义格式精确到毫秒
//    qDebug()<<"4:"<<timeStr4;

    QgsVectorDataProvider *provider = layer->dataProvider();
    provider->addFeature(*feature);
    layer->commitChanges(true);
//    layer->addFeature(*feature);
//    QDateTime timeCurrent5 = QDateTime::currentDateTime(); // 获取当前时间
//    QString timeStr5 = timeCurrent5.toString("[yy-MM-dd hh:mm:ss.zzz]"); // 转换自定义格式精确到毫秒
//    qDebug()<<"5:"<<timeStr5;

    mCanvas->refresh();

}


void MyFeatureTool::addLayers(QgsMapLayer* layer,int index)
{
    my_layers.append(layer);
    all_layers[index]=qobject_cast<QgsVectorLayer*>(layer);
}
void MyFeatureTool::findCurShortestPath()
{
    gdal_tool->initGDAL();
    gdal_tool->vectorToGraph();

    //    gdal_tool->getPointOnLine();
    if(!gdal_tool->checkToolStatus())
    {
        qDebug()<<"数据不完整，探索失败！";
        return;
    }
    std::vector<int> path= gdal_tool->getShortestPath();

    qDebug()<<"最短路径："<<path.size();
    for(auto it:path)
    {
        qDebug().noquote()<<it<<" ";
    }
    gdal_tool->allClear();
    if(path.empty())
    {
        qDebug()<<"不存在路径！";
        return;
    }
    setPathHighLight(path);




}

void MyFeatureTool::insertPoint(int index)
{

//    QgsPoint insert_Point= gdal_tool->getPointOnLine();//获取垂足坐标
//    int delete_line_id=gdal_tool->getSelectLineId();//获取最近线段id
//    QgsPoint src_point(all_points[all_lines[delete_line_id].src_feature_id].x,all_points[all_lines[delete_line_id].src_feature_id].y);
//    QgsPoint dst_point(all_points[all_lines[delete_line_id].dst_feature_id].x,all_points[all_lines[delete_line_id].dst_feature_id].y);
//    int set_point_id=-1;
//    if(gdal_tool->getFindPointStatus())//需要插入点
//    {
//        capture_point_ids.clear();
//        capture_point_List.clear();
//        qDebug()<<"找到垂足："<<insert_Point.x()<<" "<<insert_Point.y();
//        //删除直线



//        startCapturing();
//        tool_status=DrawingMode::LineMode;
//        //插入点
//        all_layers[0]->startEditing();
//        startCapturing();
//        capture_point_ids.append(all_lines[delete_line_id].src_feature_id);
//        capture_point_List.append( src_point );
//        capture_point_List.append(insert_Point);
//        addPoint(insert_Point);
////            连接
//        all_layers[0]->commitChanges(true);
//        drawLineOrCurve();
//        capture_point_List.append(dst_point);
//        capture_point_ids.append(all_lines[delete_line_id].dst_feature_id);
//        drawLineOrCurve();
//        tool_status=DrawingMode::FindPath;
//        qDebug()<<"插入点成功！";
//        stopCapturing();
//        all_layers[1]->commitChanges(true);

//        setHightLight(all_layers[1]->getFeature(delete_line_id));
//        deleteFeature();
//        clearHighLight();
//        set_point_id=all_points.lastKey();
//    }
//    else
//    {
//        if(insert_Point==src_point)
//        {
//            set_point_id=all_lines[delete_line_id].src_feature_id;
//        }
//        else
//        {
//            set_point_id=all_lines[delete_line_id].dst_feature_id;
//        }
//        qDebug()<<"不需要插入点";
//    }

//    gdal_tool->setTargetPointId(set_point_id,index);
}

int MyFeatureTool::getIndex(QgsWkbTypes::GeometryType value)
{
    switch (value) {
    case QgsWkbTypes::PointGeometry:
        return 0;
    case QgsWkbTypes::LineGeometry:
        return 1;
    case QgsWkbTypes::PolygonGeometry:
        return 2;
    default:
        break;
    }
    return -1;
}
void MyFeatureTool::clearHighLight()
{
    if(highlight)
    {
        delete highlight;
        highlight=nullptr;
        select_feature=nullptr;
        select_feature_id=-1;
        select_attributte_id=-1;
        select_feature_type=QgsWkbTypes::NullGeometry;
//        qDebug()<<"ok";
    }
}
void MyFeatureTool::clearPathHighLight()
{
    if(!path_highlight.empty())
    {
        for(int i=0;i<path_highlight.size();++i)
        {
            delete path_highlight[i];
        }
        path_highlight.clear();
    }
}
void MyFeatureTool::clearTestHighLight()
{
    if(start_highlight)
    {
        delete start_highlight;
        start_highlight=nullptr;
    }
    if(target_highlight)
    {
        delete target_highlight;
        target_highlight=nullptr;
    }
}
void MyFeatureTool::setPointFeature(int id,PointFeature point_feature)
{
    all_points.insert(id,point_feature);
}
void MyFeatureTool::setLineFeature(int id,LineFeature line_feature)
{
    all_lines.insert(id,line_feature);
}
void MyFeatureTool::setPolognFeature(int id,PolgonFeature area_feature)
{
    all_areas.insert(id,area_feature);
}

void MyFeatureTool::displayAllFeature()
{
    for(auto it=all_points.begin();it!=all_points.end();++it)
    {
        qDebug().noquote()<<"当前点："<<it.key()<<" "<<it.value().feature_id;
    }
    qDebug();
    for(auto it=all_lines.begin();it!=all_lines.end();++it)
    {
        qDebug().noquote()<<"当前直线："<<it.key()<<" "<<it.value().feature_id;
    }
    qDebug();
    for(auto it=all_areas.begin();it!=all_areas.end();++it)
    {
        qDebug().noquote()<<"当前虚拟墙："<<it.key()<<" "<<it.value().feature_id;
    }
    qDebug();
}
void MyFeatureTool::saveAttributeDatas()
{
    all_layers[0]->startEditing();
    all_layers[1]->startEditing();
    all_layers[2]->startEditing();

    for(auto it:all_points)
    {
        all_layers[0]->changeAttributeValue(it.feature_id,all_layers[0]->getFeature(it.feature_id).fieldNameIndex("id"),QString::number(it.feature_id));
        all_layers[0]->changeAttributeValue(it.feature_id,all_layers[0]->getFeature(it.feature_id).fieldNameIndex("type"),QString::number(it.type));
        all_layers[0]->changeAttributeValue(it.feature_id,all_layers[0]->getFeature(it.feature_id).fieldNameIndex("lines"),listToQString( it.lines));
//        QgsPointXY point(it.x,it.y);
//        QgsPointXY tem_point=toMapCoordinates(all_layers[0],point);
//        qDebug()<<"保存的点："<< it.feature_id<<" "<<tem_point.x()<<" "<<tem_point.y();

        all_layers[0]->changeAttributeValue(it.feature_id,all_layers[0]->getFeature(it.feature_id).fieldNameIndex("x"),it.x);
        all_layers[0]->changeAttributeValue(it.feature_id,all_layers[0]->getFeature(it.feature_id).fieldNameIndex("y"),it.y);
//        all_layers[0]->changeAttributeValue(it.feature_id,"id",it.feature_id);
    }
    for(auto it:all_lines)
    {
        all_layers[1]->changeAttributeValue(it.feature_id,all_layers[1]->getFeature(it.feature_id).fieldNameIndex("id"),QString::number(it.feature_id));
        all_layers[1]->changeAttributeValue(it.feature_id,all_layers[1]->getFeature(it.feature_id).fieldNameIndex("src_id"),QString::number(it.src_feature_id));
        all_layers[1]->changeAttributeValue(it.feature_id,all_layers[1]->getFeature(it.feature_id).fieldNameIndex("dst_id"),QString::number(it.dst_feature_id));
        all_layers[1]->changeAttributeValue(it.feature_id,all_layers[1]->getFeature(it.feature_id).fieldNameIndex("control_id"),QString::number(it.control_point_id));
        all_layers[1]->changeAttributeValue(it.feature_id,all_layers[1]->getFeature(it.feature_id).fieldNameIndex("orinted"),QString::number(it.orinted));
        all_layers[1]->changeAttributeValue(it.feature_id,all_layers[1]->getFeature(it.feature_id).fieldNameIndex("length"),QString::number(it.length));


    }
    for(auto it:all_areas)
    {
        all_layers[2]->changeAttributeValue(it.feature_id,all_layers[2]->getFeature(it.feature_id).fieldNameIndex("id"),QString::number(it.feature_id));
        all_layers[2]->changeAttributeValue(it.feature_id,all_layers[2]->getFeature(it.feature_id).fieldNameIndex("points"),it.points);

    }

    qDebug()<<"保存成功！";
    all_layers[0]->commitChanges(true);
    all_layers[1]->commitChanges(true);
    all_layers[2]->commitChanges(true);


    clearPathHighLight();
    find_status=false;
    finding_status=true;



}

QString MyFeatureTool::listToQString(const QList<int>& value)
{
    QString res;
    for(auto it:value)
    {
        res+=QString::number(it)+" ";
    }
    if(res.size()>0)
    {
        res.remove(res.size()-1);
    }
    return res;
}

void MyFeatureTool::deleteFeature()
{
//    getALLFeatures();
//    getALLFeaturesId();
    if(select_feature_id!=-1)//select_attributte_id=-1
    {
        if(select_feature_type==QgsWkbTypes::PointGeometry)//选中的是线
        {
            all_layers[0]->startEditing();

//            QList<int> connect_lines=getPointOnLine(select_feature_id);
            QList<int> connect_lines=all_points[select_feature_id].lines;
            if(!connect_lines.isEmpty())//是否有线段连接
            {
                all_layers[1]->startEditing();
//                all_layers[1]->deleteFeature()
                for(auto it:connect_lines)
                {
                    updateDeleteLineOnPoints(it);
                    all_layers[1]->deleteFeature(it);
                }
                all_layers[1]->commitChanges(true);

            }
            all_layers[0]->deleteFeature(select_feature_id);

            all_layers[0]->commitChanges(true);
            updateAllPoints(select_feature_id,connect_lines);//更新所有点的id，相关联的线上的点id也作更新
            updateAllLines(connect_lines);//更新所有线的id
            updateLinesOnPoints();//变化的线的id，更新到点的lines属性中

//            disAllFeature();
        }
        else if(select_feature_type==QgsWkbTypes::LineGeometry)
        {
            all_layers[1]->startEditing();
            all_layers[1]->deleteFeature(select_feature_id);
            QList<int> lines;
            lines.append(select_feature_id);
            updateDeleteLineOnPoints(select_feature_id);
            updateAllLines(lines);//更新线段的id
            updateLinesOnPoints();//变化的线的id，更新到点的lines属性中
//            updateDeleteLineOnPoints(select_feature_id);//更新点的lines记录
//            qDebug()<<"删除线段："<<select_feature_id;
            all_layers[1]->commitChanges(true);

        }
        else
        {
            all_layers[2]->startEditing();
            all_layers[2]->deleteFeature(select_feature_id);

            all_layers[2]->commitChanges(true);
        }





        clearHighLight();
    }
    qDebug()<<"删除成功";
    mCanvas->refresh();
}
void MyFeatureTool::allClear()
{
    qDebug()<<"全部删除";
    all_layers[0]->startEditing();
    all_layers[1]->startEditing();
    all_layers[2]->startEditing();

    all_layers[0]->selectAll();
    all_layers[0]->deleteFeatures(all_layers[0]->selectedFeatureIds());


    all_layers[1]->selectAll();
    all_layers[1]->deleteFeatures(all_layers[1]->selectedFeatureIds());


    all_layers[2]->selectAll();
    all_layers[2]->deleteFeatures(all_layers[2]->selectedFeatureIds());


    all_layers[0]->commitChanges(true);
    all_layers[1]->commitChanges(true);
    all_layers[2]->commitChanges(true);


    clearHighLight();
    clearPathHighLight();
    clearTestHighLight();




    all_points.clear();
    all_lines.clear();
    all_control_points.clear();
    all_areas.clear();
    capture_point_List.clear();
    capture_point_ids.clear();
    temp_change_line.clear();
    temp_all_points.clear();
    find_status=false;




}



void MyFeatureTool::startCapturing()
{
    mCapturing = true;
}
void MyFeatureTool::addPoint(const QgsPoint &point)
{

    QgsGeometry* new_geomery=new QgsGeometry(QgsGeometry::fromPointXY(QgsPointXY( point.x(),point.y())));
    QgsFeature feature(all_layers[0]->fields(),0);
    feature.setGeometry(*new_geomery);

    //添加字段
    int feature_id=all_points.size()==0?0:all_points[all_points.lastKey()].feature_id+1;

    feature.setAttribute("id",feature_id);
    feature.setAttribute("type",0);



//    qDebug()<<"添加点"<<feature.attributes();
    addFeature(all_layers[0],&feature);
//    all_layers[0]->addFeature(feature);
    capture_point_ids.push_back(feature.id());


    //存入点
    PointFeature temp_point;
//    temp_point.feature=&feature;
    temp_point.feature_id=feature.id();
//    temp_point.attribute_id=attribute_id;
    temp_point.x=point.x();
    temp_point.y=point.y();
    temp_point.feature=feature;

    all_points[feature.id()]=temp_point;



//    emit curLayerChange(getCanvasStatus(tool_status));

}

void MyFeatureTool::addVertex(const QgsPoint &point)
{
//    qDebug()<<point.x()<<point.y();
    if ( !mRubberBand ) // 没有rubber band，就创建一个
    {
        mRubberBand = createRubberBand(tool_status==DrawingMode::AreaMode ? QgsWkbTypes::PolygonGeometry: QgsWkbTypes::LineGeometry,true );
    }

    mRubberBand->addPoint(QgsPointXY(point.x(),point.y()) );

    if ( !mTempRubberBand )
    {
        mTempRubberBand = createRubberBand( tool_status==DrawingMode::AreaMode ? QgsWkbTypes::PolygonGeometry: QgsWkbTypes::LineGeometry,true );
    }
    else
    {
        if(tool_status==DrawingMode::AreaMode){mTempRubberBand->reset();};

//        mTempRubberBand->reset( m_captureMode == CapturePolygon ? true : false );
    }

    const QgsPoint firstPoint = QgsPoint(mRubberBand->getPoint( 0, 0 )->x(),mRubberBand->getPoint( 0, 0 )->y());
    mTempRubberBand->addPoint( firstPoint );
    mTempRubberBand->movePoint( point );
    mTempRubberBand->addPoint( point );
//    validateGeometry(); // 验证几何有效性



}

void MyFeatureTool::drawLineOrCurve()
{
    LineFeature temp_line;
    all_layers[1]->startEditing();
    QgsFeature* feature = new QgsFeature( all_layers[1]->fields(), 0 );
    QgsGeometry* g = 0;; // 新建一个geometry
    QgsPoint src_point(capture_point_List.at(capture_point_List.size()-1).x(),capture_point_List.at(capture_point_List.size()-1).y());
    QgsPoint dst_point(capture_point_List.at(capture_point_List.size()-2).x(),capture_point_List.at(capture_point_List.size()-2).y());
    QgsPoint control_point((src_point.x()+dst_point.x())*0.5,(src_point.y()+dst_point.y())*0.5+10);

    if(tool_status==DrawingMode::LineMode)
    {
        g = new QgsGeometry(QgsGeometry::fromPolyline( capture_point_List.toVector() ));
    }
    else//曲线
    {

        QgsCircularString *curve1=new QgsCircularString(src_point,control_point,dst_point);
        g=new QgsGeometry(curve1);
    }

    //设置线属性
    int index=all_lines.size()==0?0:all_lines[all_lines.lastKey()].feature_id+1;
    feature->setAttribute("id",index);


//    int dst_id=all_points[all_points.lastKey()].feature_id;
    int dst_id=capture_point_ids[capture_point_ids.size()-1];
    int src_id=capture_point_ids[capture_point_ids.size()-2];

    feature->setAttribute("src_id",src_id);
    feature->setAttribute("dst_id",dst_id);
    //添加线
    feature->setGeometry( *g );
//                    all_layers[1]->addFeature(*feature);
    addFeature(all_layers[1],feature);
    temp_line.feature=*feature;
    temp_line.feature_id=feature->id();
    temp_line.control_point_id=-1;
//    temp_line.attribute_id=index;
    temp_line.src_feature_id=src_id;//all_points[dst_id-1].feature_id
//    temp_line.src_attribute_id=all_points[src_id].attribute_id;
    temp_line.dst_feature_id=dst_id;//all_points[dst_id].feature_id
//    temp_line.dst_attribute_id=all_points[dst_id].attribute_id;
    temp_line.length=feature->geometry().length();
    temp_line.orinted=1;//单向

    if(tool_status==DrawingMode::CurveMode)
    {
        temp_line.control_point_id=feature->id();
        all_control_points[feature->id()]=control_point;
    }

    all_lines[feature->id()]=temp_line;

    //修改点属性
//                    int src_feature_id=all_points[dst_id-1].feature_id;
//                    int dst_feature_id=all_points[dst_id].feature_id;
    all_points[src_id].lines.append(feature->id());//把线的id和点关联上
    all_points[dst_id].lines.append(feature->id());
//                    QString src_lines=listToQString(all_points[dst_id-1].lines),dst_lines=listToQString(all_points[dst_id].lines);
//                    all_layers[0]->changeAttributeValue(src_feature_id,all_layers[0]->getFeature(src_feature_id).fieldNameIndex("lines"),src_lines);
//                    all_layers[0]->changeAttributeValue(dst_feature_id,all_layers[0]->getFeature(dst_feature_id).fieldNameIndex("lines"),dst_lines);
//                    all_layers[0]->commitChanges(true);

    capture_point_List.removeAt(0);
}

QgsRubberBand* MyFeatureTool::createRubberBand( QgsWkbTypes::GeometryType geometryType,bool alternativeBand )//QgsWkbTypes geometryType /*= QGis::Line*/, bool alternativeBand /*= false */
{
    QSettings settings;
    QgsRubberBand* rb = new QgsRubberBand( mCanvas );
//    rb->setWidth(1.5);
    rb->setWidth( settings.value( "/qgis/digitizing/line_width", 2 ).toInt() );
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

void MyFeatureTool::setHightLight(const QgsFeature & feature)
{
//        qDebug()<<feature.id();
    QgsFeature on_const_feature=feature;
//    select_feature=const_cast<QgsFeature*>(&feature);
    select_feature=&on_const_feature;
    //    qDebug()<<select_feature->id();
    select_feature_id=select_feature->id();
    select_attributte_id=select_feature->attribute("id").toInt();
    select_feature_type=feature.geometry().type();
    if(feature.geometry().type()==QgsWkbTypes::PointGeometry)
    {
        highlight=new QgsHighlight(mCanvas,feature,qobject_cast<QgsVectorLayer*>(all_layers[0]));
    }
    else if(feature.geometry().type()==QgsWkbTypes::LineGeometry)
    {
        highlight=new QgsHighlight(mCanvas,feature,qobject_cast<QgsVectorLayer*>(all_layers[1]));
    }
    else
    {
        highlight=new QgsHighlight(mCanvas,feature,qobject_cast<QgsVectorLayer*>(all_layers[2]));
    }

    highlight->setFillColor(Qt::green);	// 填充颜色
    highlight->setColor(Qt::green);	// 设置线条/描边颜色
    highlight->update();
    //    qDebug()<<"填充";
    //    highlight->setBuffer(0.5);
    //    highlight->setBoundingRegionGranularity(0.1);
    highlight->setBuffer(Qgis::DEFAULT_HIGHLIGHT_BUFFER_MM);	// 设置行/行程缓冲以毫米为单位
    // DEFAULT_HIGHLIGHT_MIN_WIDTH_MM = 1.0
    highlight->setMinWidth(Qgis::DEFAULT_HIGHLIGHT_MIN_WIDTH_MM);	// 设置最小线/笔画宽度，以毫米为单位
//    qDebug()<<123<<feature.geometry().type();
//    for(auto it:capture_point_ids)
//    {

//    }

}
void MyFeatureTool::setPathHighLight(std::vector<int> path)
{
//    path_highlight.clear();
    clearPathHighLight();
    for(int i=0;i<path.size();++i)
    {
        QgsHighlight *temp_highlight=new QgsHighlight(mCanvas,all_layers[0]->getFeature(path[i]),all_layers[0]);//点的高亮
        temp_highlight->setFillColor(Qt::green);	// 填充颜色
        temp_highlight->setColor(Qt::green);	// 设置线条/描边颜色
        temp_highlight->update();
        //    qDebug()<<"填充";
        //    highlight->setBuffer(0.5);
        //    highlight->setBoundingRegionGranularity(0.1);
        temp_highlight->setBuffer(Qgis::DEFAULT_HIGHLIGHT_BUFFER_MM);	// 设置行/行程缓冲以毫米为单位
        // DEFAULT_HIGHLIGHT_MIN_WIDTH_MM = 1.0
        temp_highlight->setMinWidth(Qgis::DEFAULT_HIGHLIGHT_MIN_WIDTH_MM);	// 设置最小线/笔画宽度，以毫米为单位
        path_highlight.push_back(temp_highlight);
        QgsPoint point(all_points[path[i]].x,all_points[path[i]].y);
        capture_point_List.append(point);
        if(i>0)
        {

            QgsGeometry geo=QgsGeometry::fromPolyline( capture_point_List.toVector() );
            QgsHighlight *temp_line_highlight=new QgsHighlight(mCanvas,geo,all_layers[1]);
            temp_line_highlight->setFillColor(Qt::green);	// 填充颜色
            temp_line_highlight->setColor(Qt::green);	// 设置线条/描边颜色
            temp_line_highlight->update();
            //    qDebug()<<"填充";
            //    highlight->setBuffer(0.5);
            //    highlight->setBoundingRegionGranularity(0.1);
            temp_line_highlight->setBuffer(Qgis::DEFAULT_HIGHLIGHT_BUFFER_MM);	// 设置行/行程缓冲以毫米为单位
            // DEFAULT_HIGHLIGHT_MIN_WIDTH_MM = 1.0
            temp_line_highlight->setMinWidth(Qgis::DEFAULT_HIGHLIGHT_MIN_WIDTH_MM);	// 设置最小线/笔画宽度，以毫米为单位
            path_highlight.push_back(temp_line_highlight);
            qDebug()<<"创建一条直线高亮！";
            capture_point_List.removeAt(0);

        }


    }
    capture_point_List.clear();
}
void MyFeatureTool::setStartTestHighLight(QgsPoint point)
{
    QgsGeometry geo(QgsGeometry::fromPointXY(QgsPointXY( point.x(),point.y())));
    start_highlight=new QgsHighlight(mCanvas,geo,all_layers[0]);
    start_highlight->setFillColor(Qt::blue);	// 填充颜色
    start_highlight->setColor(Qt::blue);	// 设置线条/描边颜色
    start_highlight->update();
    //    qDebug()<<"填充";
    //    highlight->setBuffer(0.5);
    //    highlight->setBoundingRegionGranularity(0.1);
    start_highlight->setBuffer(Qgis::DEFAULT_HIGHLIGHT_BUFFER_MM);	// 设置行/行程缓冲以毫米为单位
    // DEFAULT_HIGHLIGHT_MIN_WIDTH_MM = 1.0
    start_highlight->setMinWidth(Qgis::DEFAULT_HIGHLIGHT_MIN_WIDTH_MM);
}
void MyFeatureTool::setTargetTestHighLight(QgsPoint point)
{
    QgsGeometry geo(QgsGeometry::fromPointXY(QgsPointXY( point.x(),point.y())));
    target_highlight=new QgsHighlight(mCanvas,geo,all_layers[0]);
    target_highlight->setFillColor(Qt::black);	// 填充颜色
    target_highlight->setColor(Qt::black);	// 设置线条/描边颜色
    target_highlight->update();
    //    qDebug()<<"填充";
    //    highlight->setBuffer(0.5);
    //    highlight->setBoundingRegionGranularity(0.1);
    target_highlight->setBuffer(Qgis::DEFAULT_HIGHLIGHT_BUFFER_MM);	// 设置行/行程缓冲以毫米为单位
    // DEFAULT_HIGHLIGHT_MIN_WIDTH_MM = 1.0
    target_highlight->setMinWidth(Qgis::DEFAULT_HIGHLIGHT_MIN_WIDTH_MM);
}
int MyFeatureTool::getCanvasStatus(DrawingMode mode)
{
    switch (mode)
    {
    case DrawingMode::SelectionMode:
        return 0;
    case DrawingMode::LineMode:
        return 1;
    case DrawingMode::AreaMode:
        return 2;//区域模式（虚拟墙编辑）
    default:
        return 3;
    }
}



void MyFeatureTool::updateAllPoints(int delete_point,QList<int> delete_lines)
{
    temp_all_points.clear();
    for(auto id:delete_lines)//更新点的 lines 属性
    {
        for(auto strs:all_points)
        {
            if(strs.lines.contains(id))
            {
                bool flag=false;
                while(!flag)
                {

                    if(strs.lines.last()==id)
                    {
                        flag=true;
                    }
                    strs.lines.removeLast();
                }
            }

        }
    }
    all_points.remove(delete_point);

    int start_=0;
    for(auto it=all_points.begin();it!=all_points.end();++it)
    {
        temp_all_points[start_]=it.value();

        if(it.key()!=start_)//所有id变化的点，需要和修改线上点的id
        {
            temp_all_points[start_].feature_id=start_;//记录点的变化
            //更新线段上的点属性

//                qDebug()<<"正在点连接的线："<<start_<<" 所有需要变化的线段："<<list;
            updatePointOnLines(temp_all_points[start_].lines,start_,it.key());//更新相关线段的点id

        }
        start_++;
    }
    all_points.clear();
    all_points=temp_all_points;




//    auto after_itertor=all_points.lowerBound(delete_point);
//    for(auto it=after_itertor;it!=all_points.end();++it)//更新要素id
//    {
//        int new_feature_id=it.key()-1;
//        PointFeature pointFeature;
//        pointFeature.feature_id=new_feature_id;
//        pointFeature.feature=it.value().feature;
//        pointFeature.x=it.value().x;
//        pointFeature.y=it.value().y;
//        pointFeature.lines=it.value().lines;
//        if(!pointFeature.lines.isEmpty())
//        {
//            QList<QString> list=pointFeature.lines.split(' ');
//            updatePointOnLines(list,new_feature_id,it.key());
//        }

//        pointFeature.type=it.value().type;
//        pointFeature.attribute_id=it.value().attribute_id;

//        all_points.insert(new_feature_id,pointFeature);

//    }
//    all_points.remove(all_points.lastKey());


}
void MyFeatureTool::updateAllLines(QList<int>delete_lines_id)
{
    for(auto line_id:delete_lines_id)//删除线段记录
    {
//        qDebug()<<"正在删除："<<line_id<<all_lines[line_id].feature_id;
//        int src_index=all_points[]
        if(all_control_points.contains(line_id))//曲线
        {
            all_control_points.remove(line_id);
        }
        all_lines.remove(line_id);
    }


    if(all_lines.size()>=1)//更新id
    {
        temp_all_lines.clear();
        int start_=0;
        for(auto it=all_lines.begin();it!=all_lines.end();++it)
        {
            temp_all_lines[start_]=it.value();
            if(it.key()!=start_)
            {
                temp_all_lines[start_].feature_id=start_;
                temp_change_line[it.key()]=start_;//记录线的id变化
                qDebug()<<"线段变化记录："<<it.key()<<" "<<start_;
            }
            start_++;
        }
        all_lines.clear();
        all_lines=temp_all_lines;
    }

}
void MyFeatureTool::updatePointOnLines(QList<int> list,int cur_id,int pre_id)
{
    for(auto it:list)//更新点在线段上的id记录
    {
//        qDebug()<<"正在更新线段："<<it<<" pre:"<<pre_id<<" cur:"<<cur_id;
        if(all_lines[it].src_feature_id==pre_id)
        {
            all_lines[it].src_feature_id=cur_id;
//            qDebug()<<pre_id<<"变更为"<<cur_id;
        }
        if(all_lines[it].dst_feature_id==pre_id)
        {
            all_lines[it].dst_feature_id=cur_id;
//            qDebug()<<pre_id<<"变更为"<<cur_id;
        }
//        qDebug()<<"当前线段："<<it.toInt()<<" "<<all_lines[it.toInt()].src_feature_id<<" "<<all_lines[it.toInt()].dst_feature_id;
    }
}
void MyFeatureTool::updateLinesOnPoints()
{
    for(auto it=all_points.begin();it!=all_points.end();++it)
    {
        for(auto iter=temp_change_line.begin();iter!=temp_change_line.end();++iter)
        {
            if(it.value().lines.contains(iter.key()))
            {
                int index_=it.value().lines.indexOf(iter.key());
                it.value().lines.replace(index_,temp_change_line[iter.key()]);
//                qDebug()<<"替换："<<iter.key()<<"在第"<<index_<<" 替换为："<<temp_change_line[iter.key()];
            }
        }
    }
    temp_change_line.clear();
}

void MyFeatureTool::updateDeleteLineOnPoints(int line_id)
{
    for(auto it=all_points.begin();it!=all_points.end();++it)
    {
        if(it.value().lines.contains(line_id))
        {
            it.value().lines.removeAll(line_id);
        }
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
    capture_point_List.clear();
    capture_point_ids.clear();
    QPoint point(0,0);
    QgsMapMouseEvent* e=new QgsMapMouseEvent(mCanvas,QEvent::MouseButtonPress,point,Qt::RightButton);
    canvasReleaseEvent(e);
    mCanvas->refresh();
//    qDebug()<<"停止";
}
