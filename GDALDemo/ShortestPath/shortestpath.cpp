#include "shortestpath.h"

MyGDALTool::MyGDALTool()
{
    out_line_start_point = new OGRPoint(-1, -1);
    out_line_target_point = new OGRPoint(-1, -1);
    point_path = "/home/chenlang/soft/QGisdemo/Map/net/points.shp";
    line_path = "/home/chenlang/soft/QGisdemo/Map/net/lines.shp";
    pologon_path = "/home/chenlang/soft/QGisdemo/Map/net/virtuals.shp";
};
void MyGDALTool::initData()
{
    //    char **papszDSCO=NULL;

    // 确定起点和终点
    std::cout << "请输入起点和终点坐标：" << std::endl;
    double x1, y1, x2, y2;
    std::cin >> x1 >> y1 >> x2 >> y2;
    out_line_start_point->setX(x1);
    out_line_start_point->setY(y1);
    out_line_target_point->setX(x2);
    out_line_target_point->setY(y2);
        // std::cout << "成功设置起点：" << out_line_start_point->getX() << " " << out_line_start_point->getY() << std::endl;
    // std::cout << "成功设置目标点：" << out_line_target_point->getX() << " " << out_line_target_point->getY() << std::endl;

    // 判断这两个点是否是路网上的点
    // 是  设置id
    // 否  记录点坐标
    // getPointInLayerId(*out_line_start_point, *out_line_target_point);

    // 空间探索
    // 1.读取.shp文件，根据id记录要素或者坐标求出垂足，增加要素
    getMapData(point_path);
    getMapData(line_path);

    if (start_point_id == -1)
    {
        OGRPoint start_Point = getPointOnLine(0); // 获取垂足坐标
        addPointLineToMap(start_Point, 0);
        start_point_id = m_points.size() - 1;
        insert_start_point = true;
    }
    if (target_point_id == -1)
    {
        OGRPoint target_Point = getPointOnLine(1); // 获取垂足坐标
        addPointLineToMap(target_Point, 1);
        target_point_id = m_points.size() - 1;
        insert_target_point = true;

        // 创建一个点两条线段
    }

    vectorToGraph();

    //    gdal_tool->getPointOnLine();
    if (!checkToolStatus())
    {
        std::cout << "数据不完整，探索失败！" << std::endl;
        return;
    }
    system("clear");
    std::vector<int> path = getShortestPath();
    std::cout << "最短路径：" << path.size() << std::endl;
    for (auto it : path)
    {
        std::cout << it << " ";
    }
    std::cout << std::endl;
    
    allClear();
    if (path.empty())
    {
        std::cout << "不存在路径！" << std::endl;
        return;
    }


    // getMapData(pologon_path);
    // std::cout << "点的数量：" << m_points.size() << std::endl;
    // std::cout << "线段的数量：" << m_lines.size() << std::endl;
    // 2.最短路径探索
    // 3.打印数组
    // 4.循环



    // vectorToGraph();

    // std::cout<<"寻找最短路径：";
    // std::vector<std::vector<int>> temp_vec=dijkstraShortestPath(0);
    // std::cout<<"temp_vec："<<temp_vec.size();
    // for(auto i=0;i<temp_vec.size();++i)
    // {
    //     std::cout<<0<<"到"<<i<<" ";
    //     for(auto j=0;j<temp_vec[i].size();++j)
    //     {
    //         std::cout<<temp_vec[i][j]<< " ";
    //     }
    //     std::cout<<std::endl;
    // }
    //    disPlayGraph();
}

void MyGDALTool::getMapData(std::string file_path)
{

    GDALDataset *poDS = (GDALDataset *)GDALOpenEx(file_path.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
    if (poDS == NULL)
    {
        std::cout << "打开失败！";
        return;
    }
    // 获取图层
    OGRLayer *poLayer = poDS->GetLayer(0);
    // 获取图层的几何图形类型
    OGRwkbGeometryType layerGeomType = poLayer->GetGeomType();

    //    qDebug()<<QString::fromStdString(file_path)<<"当前图层类型："<<layerGeomType;

    OGRFeature *poFeature;
    poLayer->ResetReading();
    while ((poFeature = poLayer->GetNextFeature()) != NULL)
    { // 获取图层数据
        OGRGeometry *poGeometry = poFeature->GetGeometryRef();
        if (layerGeomType == 1) // 点
        {
            OGRPoint *point = (OGRPoint *)poGeometry;
            if (point->Distance(out_line_start_point) <= 3) // 起点在路网中 abs(temp_point.x - point1.getX()) <= 2 && abs(temp_point.y - point1.getY()) <= 2
            {
                start_point_id = poFeature->GetFID();
                std::cout << "起点在路网上，id为：" << start_point_id << std::endl;
            }
            if (point->Distance(out_line_target_point) <= 3) // 终点在路网中
            {
                target_point_id = poFeature->GetFID();
                std::cout << "目标点在路网上，id为：" << target_point_id << std::endl;
            }

            PointFeature temp_point;
            temp_point.feature_id = poFeature->GetFID();
            temp_point.x = poFeature->GetFieldAsDouble(poFeature->GetFieldIndex("x"));
            temp_point.y = poFeature->GetFieldAsDouble(poFeature->GetFieldIndex("x"));
            m_points[temp_point.feature_id] = temp_point;
        }
        else if (layerGeomType == 2) // 线
        {
            LineFeature temp_line;
            temp_line.feature_id = poFeature->GetFID();
            temp_line.src_feature_id = poFeature->GetFieldAsDouble(poFeature->GetFieldIndex("src_id"));
            temp_line.dst_feature_id = poFeature->GetFieldAsDouble(poFeature->GetFieldIndex("dst_id"));
            temp_line.control_point_id = poFeature->GetFieldAsDouble(poFeature->GetFieldIndex("control_id"));
            temp_line.orinted = poFeature->GetFieldAsDouble(poFeature->GetFieldIndex("orinted"));
            temp_line.length = poFeature->GetFieldAsDouble(poFeature->GetFieldIndex("length"));
            m_lines[temp_line.feature_id] = temp_line;

            // OGRGeometry *poGeometry = poFeature->GetGeometryRef();
            OGRLineString *line = (OGRLineString *)poGeometry;
            //            qDebug()<<"目标点："<<start_point->getX()<<start_point->getY();
            if (start_point_id == -1) // 起点在线外
            {
                double distance_start = line->Distance(out_line_start_point);
                if (distance_start < min_start_distance)
                {
                    min_start_distance = distance_start;
                    start_min_distance_line_id = temp_line.feature_id;
                }
            }

            std::cout << "目标点：" << out_line_target_point->getX() << out_line_target_point->getY() << std::endl;
            if (target_point_id == -1) // 终点在线外
            {
                double distance_target = line->Distance(out_line_target_point);
                std::cout << "距离直线：" << distance_target << std::endl;
                if (distance_target < min_target_distance)
                {
                    min_target_distance = distance_target;
                    target_min_distance_line_id = temp_line.feature_id;
                    std::cout << "距离目标点最近的线段：" << target_min_distance_line_id << std::endl;
                }
            }
        }
    }
}

void MyGDALTool::vectorToGraph()
{
    int count_ = m_points.size();
    graph.resize(count_, std::vector<double>(count_, INT_MAX));

    for (auto it = m_lines.begin(); it != m_lines.end(); ++it)
    {
        int m = it->second.src_feature_id;
        int n = it->second.dst_feature_id;
        graph[m][n] = it->second.length;
        if (it->second.orinted == 0) // 双向
        {
            graph[n][m] = it->second.length;
        }
        // graph[n][m]=it->second.length;
        std::cout << "m:" << m << " n:" << n << " value:" << graph[m][n] << std::endl;
    }

    std::cout << "成功将图层数据转换为graph！";
}
int MyGDALTool::minDistance(std::vector<double> distance, std::vector<bool> visted)
{
    int min_value = INT_MAX, min_index;

    for (int i = 0; i < distance.size(); ++i)
    {
        if (!visted[i] && distance[i] < min_value)
        {
            min_value = distance[i];
            min_index = i;
        }
    }
    return min_index;
}
std::vector<int> MyGDALTool::dijkstraShortestPath(int start_)
{

    //    disPlayGraph();
    int count_ = graph.size();
    distance.resize(count_, INT_MAX);
    std::vector<bool> visted(count_, false);
    std::vector<std::vector<int>> paths(count_);
    for (auto its : paths)
    {
        its.push_back(start_);
    }

    //    for (int i = 0; i < count_; i++) {
    //        distance[i] = INT_MAX;
    //        visted[i] = false;
    //        point_ids[i] = -1; // 初始化为-1，表示暂时没有前一个顶点
    //    }

    distance[start_] = 0; // 开始索引

    for (int count = 0; count < count_ - 1; count++)
    {
        int u = minDistance(distance, visted);
        if (u < 0 || u > count_)
            continue;
        visted[u] = true;
        // std::cout<<count;
        for (int v = 0; v < count_; v++)
        {
            if (!visted[v] && distance[u] != INT_MAX && distance[u] + graph[u][v] < distance[v])
            {
                distance[v] = distance[u] + graph[u][v]; // 经过u点到达v点距离更短，更新当前最短距离
                paths[v] = paths[u];
                paths[v].push_back(v);
                // std::cout<<"修改："<<v<<" "<<"更新当前最短距离："<<distance[v]<<std::endl;
            }
        }
    }
    // qDebug().noquote()<<"路径：";
    for (auto it = 0; it < paths[target_point_id].size(); ++it)
    {
        //        qDebug()<<"0点当前"<<it<<"点最短距离"<<distance[it];
        std::cout << paths[target_point_id][it] << std::endl;
    }

    return paths[target_point_id];
}

void MyGDALTool::getPointInLayerId(OGRPoint point1, OGRPoint point2)
{
    GDALDataset *poDS = (GDALDataset *)GDALOpenEx(point_path.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
    if (poDS == NULL)
    {
        std::cout << "打开失败！";
        return;
    }
    // 获取图层
    OGRLayer *poLayer = poDS->GetLayer(0);
    // 获取图层的几何图形类型
    OGRwkbGeometryType layerGeomType = poLayer->GetGeomType();

    //    qDebug()<<QString::fromStdString(file_path)<<"当前图层类型："<<layerGeomType;

    OGRFeature *poFeature;
    poLayer->ResetReading();
    while ((poFeature = poLayer->GetNextFeature()) != NULL)
    { // 获取图层数据
        OGRGeometry *poGeometry = poFeature->GetGeometryRef();
        OGRPoint *point = (OGRPoint *)poGeometry;

        // PointFeature temp_point;
        // temp_point.feature_id = poFeature->GetFID();
        // temp_point.x = poFeature->GetFieldAsDouble(poFeature->GetFieldIndex("x"));
        // temp_point.y = poFeature->GetFieldAsDouble(poFeature->GetFieldIndex("x"));
        // m_points[temp_point.feature_id] = temp_point;
        // std::cout << "点的坐标：" << temp_point.x << " " << temp_point.y;
        if (point->Distance(&point1) <= 3) // 起点在路网中 abs(temp_point.x - point1.getX()) <= 2 && abs(temp_point.y - point1.getY()) <= 2
        {
            start_point_id = poFeature->GetFID();
            std::cout << "起点在路网上，id为：" << start_point_id << std::endl;
        }

        if (point->Distance(&point2) <= 3) // 终点在路网中
        {
            target_point_id = poFeature->GetFID();
            std::cout << "目标点在路网上，id为：" << target_point_id << std::endl;
        }
        if (start_point_id != -1 && target_point_id != -1)
        {
            return;
        }
    }
}

OGRPoint MyGDALTool::getPointOnLine(int index)
{
    //    OGRGeometry *poGeometry=all_layers[1]->GetFeature(min_distance_line_id)->GetGeometryRef();
    //    OGRLineString *line=(OGRLineString*)poGeometry;

    //    line->getNumPoints();
    //    line->
    int line_id = -1;
    OGRPoint out_line_point;
    if (index == 0) // 起点
    {
        line_id = start_min_distance_line_id;
        out_line_point.setX(out_line_start_point->getX());
        out_line_point.setY(out_line_start_point->getY());
    }
    else if (index == 1)
    {
        line_id = target_min_distance_line_id;
        out_line_point.setX(out_line_target_point->getX());
        out_line_point.setY(out_line_target_point->getY());
        std::cout << "line_id" << line_id << std::endl;
    }

    double p1_x = m_points[m_lines[line_id].src_feature_id].x;
    double p1_y = m_points[m_lines[line_id].src_feature_id].y;
    double p2_x = m_points[m_lines[line_id].dst_feature_id].x;
    double p2_y = m_points[m_lines[line_id].dst_feature_id].y;

    double angle = (p2_y - p1_y) / (p2_x - p1_x);
    OGRPoint p1(p1_x, p1_y), p2(p2_x, p2_y);
    //    QgsPoint out_line(out_line_point->getX(),out_line_point->getY());
    return findIntersection(p1, p2, angle, out_line_point);
    //    qDebug()<<"线段 "<<min_distance_line_id<<" 的方向角："<<angle;
}
OGRPoint MyGDALTool::findIntersection(OGRPoint p1, OGRPoint p2, double m1, OGRPoint p)
{
    OGRPoint intersection;
    // 求交点的 x 坐标   m1 * p.x - p.y 线外一点在x轴上的截距
    double x = ((p.getX() - m1 * p1.getY() + m1 * m1 * p1.getX() + m1 * p.getY()) / (1 + m1 * m1));
    intersection.setX(x);
    // 求交点的 y 坐标
    intersection.setY(p1.getY() - m1 * (p1.getX() - x));
    // OGRGeometry *poGeometry;
    // poGeometry
    if (intersection.Distance(&p1) + intersection.Distance(&p2) > p1.Distance(&p2)) // 插入的点不在两端点之间
    {
        find_point_status = false;
        if (p.Distance(&p1) < p.Distance(&p2)) // 离p1更近
        {
            std::cout << "获取起点 1 "
                      << "为目标点" << std::endl;
            return p1;
        }
        std::cout << "获取终点 2"
                  << "为目标点" << std::endl;
        return p2;
    }
    find_point_status = true;
    std::cout << "垂足：" << intersection.getX() << " " << intersection.getY();
    return intersection;
}

void MyGDALTool::addPointLineToMap(OGRPoint temp, int index)
{
    // 创建一个点两条线段
    int line_id = -1;
    if (index == 0)
    {
        line_id = start_min_distance_line_id;
    }
    else
    {
        line_id = target_min_distance_line_id;
    }
    int src_id = m_lines[line_id].src_feature_id, dst_id = m_lines[line_id].dst_feature_id;

    PointFeature point_feature;
    point_feature.x = temp.getX();
    point_feature.y = temp.getY();
    point_feature.feature_id = m_points.size();

    OGRPoint src_point(m_points[src_id].x, m_points[src_id].y);
    OGRPoint dst_point(m_points[dst_id].x, m_points[dst_id].y);

    LineFeature line1;
    line1.feature_id = m_lines.size();
    line1.orinted = 1;
    line1.src_feature_id = src_id;
    line1.dst_feature_id = point_feature.feature_id;
    line1.control_point_id = -1;
    line1.length = src_point.Distance(&temp);

    LineFeature line2;
    line2.feature_id = m_lines.size() + 1;
    line2.orinted = 1;
    line2.src_feature_id = point_feature.feature_id;
    line2.dst_feature_id = dst_id;
    line2.control_point_id = -1;
    line2.length = dst_point.Distance(&temp);

    m_points[src_id].lines.push_back(line1.feature_id);
    point_feature.lines.push_back(line1.feature_id);
    point_feature.lines.push_back(line2.feature_id);
    m_points[dst_id].lines.push_back(line2.feature_id);

    m_points[point_feature.feature_id] = point_feature;
    m_lines[line1.feature_id] = line1;
    m_lines[line2.feature_id] = line2;
}
bool MyGDALTool::checkToolStatus()
{
    if (start_point_id == -1)
    {
        std::cout << "最短路径起点未确定！" << std::endl;
        return false;
    }
    else if (target_point_id == -1)
    {
        std::cout << "最短路径终点未确定！" << std::endl;
    }
    else if (start_min_distance_line_id == -1)
    {
        std::cout << "最近直线未确定！" << std::endl;
    }
    return true;
}
std::vector<int> MyGDALTool::getShortestPath()
{
    std::cout << "寻找最短路径：" << std::endl;
    std::vector<int> temp_vec = dijkstraShortestPath(start_point_id);
    if (temp_vec.size() < 1)
        return temp_vec;
    if (!insert_start_point)
    {
        temp_vec.insert(temp_vec.begin(), start_point_id);
    }
    if (insert_target_point)
    {
        temp_vec.erase(--temp_vec.end());
    }
    //    temp_vec.insert(temp_vec.begin(),start_point_id);
    return temp_vec;
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
    min_start_distance = INT_MAX;
    min_target_distance = INT_MAX;
    start_min_distance_line_id = -1;
    target_min_distance_line_id = -1;

    start_point_id = -1;
    target_point_id = -1;
    find_point_status = false;
    insert_start_point = false;
    insert_target_point = false;
    // system("clear");
}