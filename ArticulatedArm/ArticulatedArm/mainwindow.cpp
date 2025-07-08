#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    robotDialog = new RobotDialog(ui->widgetContainer);
    robotDialog->show();

    this->setMinimumSize(1150, 850);  // 设置合适的默认最小窗口大小
    showMaximized();
}

MainWindow::~MainWindow() {
    delete robotDialog;
    delete ui;
}

//窗口自适应
void MainWindow::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event);
    robotDialog->resize(ui->widgetContainer->size());
}
