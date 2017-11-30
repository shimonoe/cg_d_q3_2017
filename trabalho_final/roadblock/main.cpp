#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
    QSurfaceFormat format;

#ifdef Q_OS_LINUX
    format.setVersion(4, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);
#endif

// defining anti-aliasing -> can be used for win/linux

    format.setDepthBufferSize(24);
    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
