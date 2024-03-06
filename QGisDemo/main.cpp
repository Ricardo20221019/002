#include "qgisdemo.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    QgsApplication::setPrefixPath( "/usr/bin/qgis", false );
    QString strPluginDir = "/usr/lib/qgis/plugins";
    QgsApplication::setPluginPath(strPluginDir);
    QString strPkgDir = "/usr/share/qgis";
    QgsApplication::setPkgDataPath(strPkgDir);
    //loads providers
    QgsApplication::initQgis();
    QgsProviderRegistry::instance();

    QGisDemo w;
    w.show();

    return a.exec();
}
