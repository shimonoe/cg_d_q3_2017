#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "math.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButtonCalculate, SIGNAL(clicked()), this, SLOT(opVectors()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::opVectors( void )
{
    QString input = ui->lineEditVectors->text();
    QStringList list = input.remove("(").remove(")").split(" ");

    QStringList vector1, vector2;

    vector1 = list[0].split(",");
    vector2 = list[1].split(",");

    float dot_product = dotProduct(vector1, vector2);
    float smaller_angle = smallerAngle(vector1, vector2, dot_product);
    //QString orthogonal_angle = orthogonalVector(vector1, vector2, dot_product);
    float area_of_triangle = areaOfTriangle(vector1, vector2);


    ui->labelDotProduct->setText(QString("Dot product between v1 and v2:\t\t %1").arg(dot_product));
    ui->labelSmallerAngle->setText(QString("Smaller angle between v1 and v2:\t\t %1°").arg(smaller_angle));
    ui->labelOrthogonalVector->setText(QString("Orthogonal vector of v1 and v2:\t\t %1").arg(" - "));
    ui->labelAreaOfTriangle->setText(QString("Area of triangle defined by v1 and v2:\t %1").arg(area_of_triangle));
}


float MainWindow::dotProduct( QStringList vector1, QStringList vector2 )
{
    float result, i;

    result = 0;
    for ( i = 0; i < 3; i++ )
    {
        result += vector1[i].toFloat() * vector2[i].toFloat();
    }
    return result;
}

float MainWindow::smallerAngle( QStringList vector1, QStringList vector2, float dot_product)
{
    int i;
    float norm1, norm2, angle;

    norm1 = 0;
    norm2 = 0;
    for ( i = 0; i < 3; i++ )
    {
        norm1 += pow(vector1[i].toFloat(), 2);
        norm2 += pow(vector2[i].toFloat(), 2);
    }
    norm1 = pow(norm1, 0.5);
    norm2 = pow(norm2, 0.5);

    angle = acos(dot_product / (norm1 * norm2));

    return angle;
}

/*
QString MainWindow::orthogonalVector( QStringList vector1, QStringList vector2, float dot_product)
{
    QString orth_vector;

    if ( dot_product != 0 )
    {
        orth_result = "-";
        return orth_result;
    }


    QStringList refVector;
    if ( vector1 != new QStringList(0,0,0) )
    {
        refVector = vector1;
    }
    else
    {
        refVector = vector2;
    }

    // TODO: Search in vector an element multiple of another, then find de multiplier and
    //       resolve orthogonal vector elements

    orth_vector = "";

    return refVector;
}
*/

float MainWindow::areaOfTriangle( QStringList vector1, QStringList vector2)
{
    int i;
    float area, vect[3];

    vect[0] = (vector1[1].toFloat() * vector2[2].toFloat()) - (vector1[2].toFloat() * vector2[1].toFloat());
    vect[1] = (vector1[0].toFloat() * vector2[2].toFloat()) - (vector1[2].toFloat() * vector2[0].toFloat());
    vect[2] = (vector1[0].toFloat() * vector2[1].toFloat()) - (vector1[1].toFloat() * vector2[0].toFloat());

    area = 0;
    for (i = 0; i < 3; i++)
    {
        area += pow(vect[i], 2);
    }

    area = pow(area, 0.5) / 2;

    return area;
}
