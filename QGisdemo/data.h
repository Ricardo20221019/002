#ifndef DATA_H
#define DATA_H
#include <QString>
struct netPath
{
    QString pointPath;
    QString linePath;
    QString virtualPath;
};
enum DrawingMode
{
    SelectionMode = 0,//选中模式
    LineMode = 1,//线段模式（路线编辑 路网编辑）
    CurveMode = 2,//曲线模式（路网编辑 路线编辑）
    AreaMode = 3,//区域模式（虚拟墙编辑）
    NavigatorMode = 4,//导航模式
    MappingMode=5, //建图模式
    ForbidEditMode=6
    //任务编辑
    //录制模式
};

enum CaptureMode // 矢量化类型
{
    CaptureNone,       // 无
    CapturePoint,      // 点
    CaptureLine,       // 线
    CapturePolygon     // 面
};

#endif // DATA_H
