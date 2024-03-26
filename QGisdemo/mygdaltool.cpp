#include "mygdaltool.h"

MyGDALTool::MyGDALTool(QObject *parent) : QObject(parent)
{
    GDALAllRegister();
    OGRRegisterAll();
    out_line_start_point=new OGRPoint(-1,-1);
    out_line_target_point=new OGRPoint(-1,-1);
    point_path= "/home/chenlang/soft/QGisdemo/Map/net/points.shp";
    line_path= "/home/chenlang/soft/QGisdemo/Map/net/lines.shp";
    pologon_path= "/home/chenlang/soft/QGisdemo/Map/net/virtuals.shp";


}
MyGDALTool::~MyGDALTool()
{

}

void MyGDALTool::initGDAL()
{
//    char **papszDSCO=NULL;


    m_points.clear();
    m_lines.clear();
    getMapData(point_path);
    getMapData(line_path);//找到最近的直线
//    getMapData(pologon_path);
    if(start_point_id==-1)
    {
        QgsPoint start_Point= getPointOnLine(0);//获取垂足坐标
        addPointLineToMap(start_Point,0);
        start_point_id=m_points.size()-1;
        insert_start_point=true;

    }
    if(target_point_id==-1)
    {
        QgsPoint target_Point= getPointOnLine(1);//获取垂足坐标
        addPointLineToMap(target_Point,1);
        target_point_id=m_points.size()-1;
        insert_target_point=true;

        //创建一个点两条线段
    }




//    getStartPoint();//获取到最近直线上的两个点的id
//    vectorToGraph();

//    disPlayGraph();

}
void MyGDALTool::getMapData(std::string file_path)
{


    GDALDataset *poDS = (GDALDataset*)GDALOpenEx(file_path.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
    if (poDS == NULL) {
        qDebug()<<"打开失败！";
        return ;
    }
    // 获取图层
    OGRLayer *poLayer = poDS->GetLayer(0);


    // 获取图层的几何图形类型
    OGRwkbGeometryType layerGeomType = poLayer->GetGeomType();

    OGRFeature *poFeature;
    poLayer->ResetReading();
    while ((poFeature = poLayer->GetNextFeature()) != NULL) {//获取图层数据
//        OGRGeometry *poGeometry = poFeature->GetGeometryRef();
        if(layerGeomType==1)//点
        {
            PointFeature temp_point;
            temp_point.feature_id=poFeature->GetFID();
            temp_point.x=poFeature->GetFieldAsDouble(poFeature->GetFieldIndex("x"));
            temp_point.y=poFeature->GetFieldAsDouble(poFeature->GetFieldIndex("y"));
            m_points[temp_point.feature_id]=temp_point;
//            target_point.setX(temp_point.x);
//            target_point.setY(temp_point.y);
//            qDebug()<<"目标点1："<<target_point.getX()<<target_point.getY();
        }
        else if(layerGeomType==2)//线
        {
            LineFeature temp_line;
            temp_line.feature_id=poFeature->GetFID();
            temp_line.src_feature_id=poFeature->GetFieldAsDouble(poFeature->GetFieldIndex("src_id"));
            temp_line.dst_feature_id=poFeature->GetFieldAsDouble(poFeature->GetFieldIndex("dst_id"));
            temp_line.control_point_id=poFeature->GetFieldAsDouble(poFeature->GetFieldIndex("control_id"));
            temp_line.orinted=poFeature->GetFieldAsDouble(poFeature->GetFieldIndex("orinted"));
            temp_line.length=poFeature->GetFieldAsDouble(poFeature->GetFieldIndex("length"));
            m_lines[temp_line.feature_id]=temp_line;

            OGRGeometry *poGeometry=poFeature->GetGeometryRef();
            OGRLineString *line=(OGRLineString*)poGeometry;
//            qDebug()<<"目标点："<<start_point->getX()<<start_point->getY();
            if(out_line_start_point->getX()!=-1 && out_line_start_point->getY()!=-1)
            {
                double distance_start=line->Distance(out_line_start_point);
                if(distance_start<min_start_distance)
                {
                    min_start_distance=distance_start;
                    start_min_distance_line_id=temp_line.feature_id;
                }
            }

            qDebug()<<"目标点："<<out_line_target_point->getX()<<out_line_target_point->getY();
            if(out_line_target_point->getX()!=-1 && out_line_target_point->getY()!=-1)
            {
                double distance_target=line->Distance(out_line_target_point);
                qDebug()<<"距离直线："<<distance_target;
                if(distance_target<min_target_distance)
                {
                    min_target_distance=distance_target;
                    target_min_distance_line_id=temp_line.feature_id;
                    qDebug()<<"距离目标点最近的线段："<<target_min_distance_line_id;
                }
            }





//            qDebug()<<"目标点距离该直线距离："<<distance;

            //找到距离固定点最近的线

        }
    }

    GDALClose(poDS);

}



void MyGDALTool::vectorToGraph()
{
    int count_=m_points.size();
    graph.resize(count_,std::vector<double>(count_,INT_MAX));

    for(auto it=m_lines.begin();it!=m_lines.end();++it)
    {
        int m=it->second.src_feature_id;
        int n=it->second.dst_feature_id;
        graph[m][n]=it->second.length;
        if(it->second.orinted==0)
        {
            graph[n][m]=it->second.length;
        }

//        qDebug()<<"m:"<<m<<" n:"<<n<<" value:"<<graph[m][n];
    }




    qDebug()<<"成功将图层数据转换为graph！"<<"size："<<graph.size();




//    qDebug()<<"temp_vec："<<temp_vec.size();
//    for(auto i=0;i<temp_vec.size();++i)
//    {
//        std::cout<<0<<"到"<<i<<" ";
//        for(auto j=0;j<temp_vec[i].size();++j)
//        {
//            std::cout<<temp_vec[i][j]<< " ";
//        }
//        std::cout<<std::endl;
//    }






}
void MyGDALTool::setFindPathPoint(double x,double y,int index)//0 起点   1 终点
{
    if(index==0)
    {
        out_line_start_point->setX(x);
        out_line_start_point->setY(y);
        qDebug()<<"设置起点："<<x<<" "<<y;
    }
    else
    {
        out_line_target_point->setX(x);
        out_line_target_point->setY(y);
        qDebug()<<"设置终点："<<x<<" "<<y;
    }
//    initGDAL();

}
void MyGDALTool::setTargetPointId(int id,int index)
{
    if(index==0)
    {
        start_point_id=id;
        qDebug()<<"设置起点id："<<start_point_id;
    }
    else
    {
        target_point_id=id;
        qDebug()<<"设置目标点id："<<target_point_id;
    }


}
std::vector<int> MyGDALTool::getShortestPath()
{
    qDebug()<<"寻找最短路径：";
    std::vector<int> temp_vec=dijkstraShortestPath(start_point_id);
    if(temp_vec.size()<1) return temp_vec;
    if(!insert_start_point)
    {
        temp_vec.insert(temp_vec.begin(),start_point_id);
    }
    if(insert_target_point)
    {
        temp_vec.erase(--temp_vec.end());
    }
//    temp_vec.insert(temp_vec.begin(),start_point_id);
    return temp_vec;
}
QgsPoint MyGDALTool::getPointOnLine(int index)
{
//    OGRGeometry *poGeometry=all_layers[1]->GetFeature(min_distance_line_id)->GetGeometryRef();
//    OGRLineString *line=(OGRLineString*)poGeometry;

//    line->getNumPoints();
//    line->
    int line_id=-1;
    QgsPoint out_line_point;
    if(index==0)//起点
    {
        line_id=start_min_distance_line_id;
        out_line_point.setX(out_line_start_point->getX());
        out_line_point.setY(out_line_start_point->getY());

    }
    else if(index==1)
    {
        line_id=target_min_distance_line_id;
        out_line_point.setX(out_line_target_point->getX());
        out_line_point.setY(out_line_target_point->getY());
        qDebug()<<"line_id"<<line_id;
    }


    double p1_x=m_points[m_lines[line_id].src_feature_id].x;
    double p1_y=m_points[m_lines[line_id].src_feature_id].y;
    double p2_x=m_points[m_lines[line_id].dst_feature_id].x;
    double p2_y=m_points[m_lines[line_id].dst_feature_id].y;

    double angle= (p2_y-p1_y)/(p2_x-p1_x);
    QgsPoint p1(p1_x,p1_y),p2(p2_x,p2_y);
//    QgsPoint out_line(out_line_point->getX(),out_line_point->getY());
    return findIntersection(p1,p2,angle,out_line_point);
//    qDebug()<<"线段 "<<min_distance_line_id<<" 的方向角："<<angle;
}
QgsPoint MyGDALTool::findIntersection(QgsPoint p1,QgsPoint p2, double m1, QgsPoint p)
{
    QgsPoint intersection;
    // 求交点的 x 坐标   m1 * p.x - p.y 线外一点在x轴上的截距
    double x=((p.x()-m1*p1.y()+m1*m1*p1.x()+m1*p.y())/(1+m1*m1));
    intersection.setX(x);
    // 求交点的 y 坐标
    intersection.setY(p1.y()-m1*(p1.x()-x));
    if(intersection.distance(p1)+intersection.distance(p2)>p1.distance(p2))//插入的点不在两端点之间
    {
        find_point_status=false;
        if(p.distance(p1)<p.distance(p2))//离p1更近
        {
            qDebug()<<"获取起点 1 "<<"为目标点";
            return p1;
        }
        qDebug()<<"获取终点 2"<<"为目标点";
        return p2;
    }
    find_point_status=true;
    qDebug()<<"垂足："<<intersection.x()<<" "<<intersection.y();
    return intersection;
}
bool MyGDALTool::checkToolStatus()
{
    if(start_point_id==-1)
    {
        qDebug()<<"最短路径起点未确定！";
        return false;
    }
    else if(target_point_id==-1)
    {
        qDebug()<<"最短路径终点未确定！";
    }
    else if(start_min_distance_line_id==-1)
    {
        qDebug()<<"最近直线未确定！";
    }
    return true;
}
void MyGDALTool::allClear()
{
    m_points.clear();
    m_lines.clear();
    graph.clear();
    distance.clear();

    out_line_start_point->setX(-1);
    out_line_start_point->setY(-1);
    out_line_target_point->setX(-1);
    out_line_target_point->setY(-1);
    min_start_distance=INT_MAX;
    min_target_distance=INT_MAX;
    start_min_distance_line_id=-1;
    target_min_distance_line_id=-1;

    start_point_id=-1;
    target_point_id=-1;
    find_point_status=false;
    insert_start_point=false;
    insert_target_point =false;
}
void MyGDALTool::disPlayGraph()
{
    if(!graph.empty())
    {
        for(auto its:graph)
        {
            for(auto it:its)
            {
                std::cout<<it<<" ";

            }
            std::cout<<std::endl;
        }
    }
}
int MyGDALTool::minDistance(std::vector<double> distance,std::vector<bool> visted)
{
    int min_value=INT_MAX,min_index;

    for(int i=0;i<distance.size();++i)
    {
        if(!visted[i] && distance[i]<min_value)
        {
            min_value=distance[i];
            min_index=i;
            qDebug()<<i;
        }
    }
    qDebug()<<"最小index："<<min_index;
    return min_index;
}
void MyGDALTool::addPointLineToMap(QgsPoint temp,int index)
{
    //创建一个点两条线段
    int line_id=-1;
    if(index==0)
    {
        line_id=start_min_distance_line_id;
    }
    else
    {
        line_id=target_min_distance_line_id;
    }
    int src_id=m_lines[line_id].src_feature_id,dst_id=m_lines[line_id].dst_feature_id;

    PointFeature point_feature;
    point_feature.x=temp.x();
    point_feature.y=temp.y();
    point_feature.feature_id=m_points.size();

    QgsPoint src_point(m_points[src_id].x,m_points[src_id].y);
    QgsPoint dst_point(m_points[dst_id].x,m_points[dst_id].y);

    LineFeature line1;
    line1.feature_id=m_lines.size();
    line1.orinted=1;
    line1.src_feature_id=src_id;
    line1.dst_feature_id=point_feature.feature_id;
    line1.control_point_id=-1;
    line1.length=src_point.distance(temp);



    LineFeature line2;
    line2.feature_id=m_lines.size()+1;
    line2.orinted=1;
    line2.src_feature_id=point_feature.feature_id;
    line2.dst_feature_id=dst_id;
    line2.control_point_id=-1;
    line2.length=dst_point.distance(temp);



    m_points[src_id].lines.append(line1.feature_id);
    point_feature.lines.append(line1.feature_id);
    point_feature.lines.append(line2.feature_id);
    m_points[dst_id].lines.append(line2.feature_id);

    m_points[point_feature.feature_id]=point_feature;
    m_lines[line1.feature_id]=line1;
    m_lines[line2.feature_id]=line2;
}
//void MyGDALTool::getStartPoint()
//{
//    start_A = m_lines[min_distance_line_id].src_feature_id;
//    start_B = m_lines[min_distance_line_id].dst_feature_id;
//    qDebug()<<"得到两个点id："<<start_A<<" "<<start_B;
//}
std::vector<int> MyGDALTool::dijkstraShortestPath(int start_)
{

//    disPlayGraph();
    int count_=graph.size();
    distance.resize(count_,INT_MAX);
    std::vector<bool> visted(count_,false);
    std::vector<std::vector<int>> paths(count_);
    for(auto its:paths)
    {
        its.push_back(start_);
    }

//    for (int i = 0; i < count_; i++) {
//        distance[i] = INT_MAX;
//        visted[i] = false;
//        point_ids[i] = -1; // 初始化为-1，表示暂时没有前一个顶点
//    }


    distance[start_] = 0;//开始索引
    qDebug()<<124<<"start:"<<start_;

    for (int count = 0; count < count_ - 1; count++) {
        int u = minDistance(distance, visted);
        if(u<0 || u>count_) continue;
        visted[u] = true;
        qDebug()<<count;
        for (int v = 0; v < count_; v++) {
            if (!visted[v]  && distance[u] != INT_MAX && distance[u] + graph[u][v] < distance[v]) {
                distance[v] = distance[u] + graph[u][v];//经过u点到达v点距离更短，更新当前最短距离
                paths[v]=paths[u];
                paths[v].push_back(v);
                qDebug()<<"修改："<<v<<" "<<"更新当前最短距离："<<distance[v];
            }
        }
    }
    qDebug().noquote()<<"路径：";
    for(auto it=0;it<paths[target_point_id].size();++it)
    {
//        qDebug()<<"0点当前"<<it<<"点最短距离"<<distance[it];
         qDebug()<<paths[target_point_id][it];
    }

    return paths[target_point_id];

}
//double MyGDALTool::Distance(const PointFeature &point1,const PointFeature &point2)
//{
//    return qSqrt(qPow(point2.x - point1.x, 2) + qPow(point2.y - point1.y, 2));
//}
