#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

public slots:
    void opVectors ( void );
    float dotProduct ( QStringList, QStringList );
    float smallerAngle( QStringList, QStringList, float );
    //QString MainWindow::orthogonalVector( QStringList, QStringList, float );
    float areaOfTriangle(QStringList, QStringList );
};

#endif // MAINWINDOW_H
