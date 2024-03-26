#ifndef MYGDALTOOL_H
#define MYGDALTOOL_H

#include <iostream>
#include <gdal.h>
#include <ogrsf_frmts.h>
#include <cmath>
// #include 
#include "gnm.h"
#include "data.h"

using namespace std;


class MyGDALTool 
{

public:
    explicit MyGDALTool();
    ~MyGDALTool(){};
    void initData(); 
    void getMapData(std::string file_path); //get layer datas
//    void shortestPath();
//    std::vector<OGRPoint> calPermutations();//
    void vectorToGraph(); // transform graph


private:
    void allClear();// clear status
    // void disPlayGraph();
    int minDistance(std::vector<double> distance,std::vector<bool> visted); //find min distance
    void getPointInLayerId(OGRPoint point1,OGRPoint point2);//whether point in the road
    OGRPoint getPointOnLine(int index);//  get  Hanging feet
    OGRPoint findIntersection(OGRPoint p1,OGRPoint p2, double m1, OGRPoint p);
    void addPointLineToMap(OGRPoint temp,int index);// add feature
    bool checkToolStatus(); //judge res status
    std::vector<int> getShortestPath(); //get res
    std::vector<int> dijkstraShortestPath(int start_);//get shortest path



private:
    std::map<int,PointFeature> m_points;
    std::map<int,LineFeature> m_lines;

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



