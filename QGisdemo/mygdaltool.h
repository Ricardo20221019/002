#ifndef MYGDALTOOL_H
#define MYGDALTOOL_H

#include <QObject>
#include <gdal.h>
#include <ogrsf_frmts.h>
#include <QDebug>
#include <cmath>
#include <QtMath>
#include "gnm.h"
//#include "gdalgrid_priv.h"
//#include "gdal_priv.h"
//#include "ogr_api.h"



#include "data.h"


class MyGDALTool : public QObject
{
    Q_OBJECT
public:
    explicit MyGDALTool(QObject *parent = nullptr);
    ~MyGDALTool();
    void initGDAL();
    void getMapData(std::string file_path);
//    void shortestPath();
//    std::vector<OGRPoint> calPermutations();//
    void vectorToGraph();

    void setFindPathPoint(double x,double y,int index);
    void setTargetPointId(int id,int index);// 0 起点  1 目标点

    std::vector<int> getShortestPath();
    int getSelectLineId(){return 1;}
    bool getFindPointStatus(){return find_point_status;}
    QgsPoint getPointOnLine(int index);//0 起点  1 终点
    QgsPoint findIntersection(QgsPoint p1,QgsPoint p2, double m1, QgsPoint p);
    bool checkToolStatus();
    void allClear();


signals:


public slots:



private:

    void disPlayGraph();
    std::vector<int> dijkstraShortestPath(int start_);
    int minDistance(std::vector<double> distance,std::vector<bool> visted);
//    void getStartPoint();
    void addPointLineToMap(QgsPoint temp,int index);


private:

//    QMap<int,OGRLayer *> all_layers;

    std::map<int,PointFeature> m_points;
    std::map<int,LineFeature> m_lines;



//    std::vector<OGRPoint> m_vecOGRPoint;
//    std::vector<std::vector<PointFeature>> m_indexandPointInfo;//std::vector<std::vector<OGRPoint>> m_indexandPointInfo;
//    std::vector<std::pair<int,double>> m_pathLang;

    std::vector<std::vector<double>> graph;//邻接矩阵
    std::vector<double> distance;

    OGRPoint *out_line_start_point=nullptr;
    OGRPoint *out_line_target_point=nullptr;
    double min_start_distance=INT_MAX;
    double min_target_distance=INT_MAX;
    int start_min_distance_line_id=-1;
    int target_min_distance_line_id=-1;

    int start_point_id=-1;
    int target_point_id=-1;
    bool find_point_status=false;
    bool insert_start_point=false;
    bool insert_target_point =false;
    std::string point_path;
    std::string line_path;
    std::string pologon_path;


};

#endif // MYGDALTOOL_H
