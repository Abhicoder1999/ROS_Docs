#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMainWindow>
#include <ros/ros.h>
#include <std_msgs/Int32.h>
#include <std_msgs/Int32MultiArray.h>
#include <iostream>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<QtGui>

using namespace cv;
using namespace Qt;
using namespace std;

MainWindow::MainWindow(ros::NodeHandle _nh, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), nh(_nh), it(_nh)
{
    ui->setupUi(this);
    sub = it.subscribe("object_track", 1, &MainWindow::callback,this);
    pub = nh.advertise<std_msgs::Int32>("control",1000);
    set =nh.advertise<sample_qt_app::Combined>("HSV",1000);//changed the publishing type to custom
     ui->label->setScaledContents(true);
     ui->minhue->setMaximum(255);ui->minhue->setMinimum(0);ui->minhue->setValue(21);
     ui->maxhue->setMaximum(255);ui->maxhue->setMinimum(0);ui->maxhue->setValue(85);
     ui->minsat->setMaximum(255);ui->minsat->setMinimum(0);ui->minsat->setValue(90);
     ui->maxsat->setMaximum(255);ui->maxsat->setMinimum(0);ui->maxsat->setValue(165);
     ui->minval->setMaximum(255);ui->minval->setMinimum(0);ui->minval->setValue(129);
     ui->maxval->setMaximum(255);ui->maxval->setMinimum(0);ui->maxval->setValue(255);
     connect(ui->button_test, SIGNAL(clicked()),this,SLOT(slotButtonClicked()));
     connect(ui->minhue,SIGNAL(sliderMoved(int)),this,SLOT(slotset()));
     connect(ui->maxhue,SIGNAL(sliderMoved(int)),this,SLOT(slotset()));
     connect(ui->minsat,SIGNAL(sliderMoved(int)),this,SLOT(slotset()));
     connect(ui->maxsat,SIGNAL(sliderMoved(int)),this,SLOT(slotset()));
     connect(ui->minval,SIGNAL(sliderMoved(int)),this,SLOT(slotset()));
     connect(ui->maxval,SIGNAL(sliderMoved(int)),this,SLOT(slotset()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::callback(const sensor_msgs::ImageConstPtr& msg)
{
    try
  {
    if(!cv_bridge::toCvShare(msg, "bgr8")->image.empty())
    {
		Mat img=cv_bridge::toCvShare(msg, "bgr8")->image;
        cvtColor(img, img, CV_BGR2RGB);
		ui->label->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888)));
    }
    else
    {
	ROS_INFO("ERROR: image is empty");
    }
  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
  }
}

void MainWindow::slotButtonClicked()
{
    std_msgs::Int32 sig;
    if (((QPushButton*)sender())->text() =="Stop")
    {
     ui->button_test->setText("Resume");
     sig.data=0;

    }
    else
    {
     ui->button_test->setText("Stop");
     sig.data=1;
     }
    std::cout<<sig.data<<'\n';
    pub.publish(sig);
}

void MainWindow::slotset()
{/*
    std_msgs::Int32MultiArray array;
    array.data.clear();
    array.data.push_back(ui->maxval->value());
    array.data.push_back(ui->minval->value());
    array.data.push_back(ui->maxsat->value());
    array.data.push_back(ui->minsat->value());
    array.data.push_back(ui->maxhue->value());
    array.data.push_back(ui->minhue->value());
    set.publish(array);
*/
    sample_qt_app::Combined array;
    array.Vmax = ui->maxval->value();
    array.Vmin = ui->minval->value();
    array.Smax = ui->maxsat->value();
    array.Smin = ui->minsat->value();
    array.Hmin = ui->maxhue->value();
    array.Hmax = ui->minhue->value();
    set.publish(array);
}
