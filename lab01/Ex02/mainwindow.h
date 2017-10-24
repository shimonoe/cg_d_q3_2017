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
    QStringList convert_to_matrix( QString );

public slots:
    void MainWindow::calculate( void );
    QString MainWindow::sum_matrices( QStringList, QStringList );
    QString MainWindow::multipy_matrices( QStringList, QStringList );

};

#endif // MAINWINDOW_H
