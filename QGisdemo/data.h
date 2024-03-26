#ifndef DATA_H
#define DATA_H
#include <QString>
#include <qgspoint.h>
#include <qgsfeature.h>

struct netPath
{
    QString pointPath;
    QString linePath;
    QString virtualPath;
};
struct PointFeature
{
    int feature_id;
//    int attribute_id;
    int type;
    double x;
    double y;
    QgsFeature feature;
//    QString lines;
    QList<int> lines;

};
struct LineFeature
{
    int feature_id;
//    int attribute_id;
    int src_feature_id;
//    int src_attribute_id;
//    int dst_attribute_id;
    int dst_feature_id;
    int control_point_id;
    QgsFeature feature;
    double length;
    int orinted;
};
struct CurveFeature
{
    int feature_id;
    int src_id;
    int dst_id;
    int control_point_id;
    QgsFeature feature;
};
struct PolgonFeature
{
    int feature_id;
//    int attribute_id;
    QString points;

};
enum DrawingMode
{
    SelectionMode = 0,//选中模式
    LineMode = 1,//线段模式（路线编辑 路网编辑）
    CurveMode = 2,//曲线模式（路网编辑 路线编辑）
    AreaMode = 3,//区域模式（虚拟墙编辑）
    FindPath=4,
//    NavigatorMode = 3,//导航模式
//    MappingMode=5, //建图模式
    ForbidEditMode=5
    //任务编辑
    //录制模式
};
enum FindPoint
{
    Insert=0,
    NoInsert=1
};

enum CaptureMode // 矢量化类型
{
    CaptureNone,       // 无
    CapturePoint,      // 点
    CaptureLine,       // 线
    CapturePolygon     // 面
};

#endif // DATA_H
