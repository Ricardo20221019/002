#include <iostream>



#include <list>




struct PointFeature
{
    int feature_id;
//    int attribute_id;
    int type;
    double x;
    double y;
//    QString lines;
    std::list<int> lines;

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
    double length;
    int orinted;
};