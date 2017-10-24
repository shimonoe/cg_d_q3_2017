#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButtonCalculate, SIGNAL(clicked()), this, SLOT(calculate()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::calculate()
{
    QString inputA = ui->lineEditMatrixA->text();
    QString inputB = ui->lineEditMatrixB->text();

    QStringList matrixA = convert_to_matrix(inputA);
    QStringList matrixB = convert_to_matrix(inputB);

    QString sum = sum_matrices(matrixA, matrixB);

}

QStringList MainWindow::convert_to_matrix(QString matrix)
{
    QStringList parsed;
    QStringList row;
    parsed = matrix.replace('[[', '[').replace(']]', ']').split(';');
    int i;
    for ( i = 0; i < 4; i++ ) {
        row = parsed[i].remove('(').remove(')').split(',');
        parsed.append(row);
    }
    return parsed;
}

QString MainWindow::sum_matrices( QStringList A, QStringList B )
{
    QString result;
    result.append('[');
    float row[4];
    int i, j;
    for ( i = 0; i < 4; i++ ) {
        for ( j = 0; j < 4; j++) {
            row[j] = (float)A[i][j] + (float)B[i][j];
        }
        result.append(row.to_text());
    }
}
