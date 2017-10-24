#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int position_matrix[3][3] = {{-1,-1,-1}, {-1,-1,-1}, {-1,-1,-1}};
    char player = 'X';
    bool end = false;


}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::update_state(int position_matrix[3][3]) {


}
