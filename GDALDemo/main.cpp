#include <iostream>
using namespace std;
#include <limits.h>


#include "gdal.h"
#include "./ShortestPath/shortestpath.h"

int main() {

    GDALAllRegister();
    OGRRegisterAll();
    // std::cout<<123;
    MyGDALTool*tool=new MyGDALTool();
    while(true)
    {
        tool->initData();
    }
    
    return 0;
}