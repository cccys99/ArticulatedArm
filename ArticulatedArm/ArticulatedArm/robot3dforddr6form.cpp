#include "robot3dforddr6form.h"
#include "ui_robot3dforddr6form.h"

#include <QDebug>
#include <QDoubleSpinBox>

Robot3DForDDR6Form::Robot3DForDDR6Form(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::Robot3DForDDR6Form) {
    ui->setupUi(this);

    QList<int> sizes;
    //设置QSplitter比例2:1，在保持比例的情况下，绝对值要尽量大
    sizes << 20000 << 10000;
    ui->splitter->setSizes(sizes);

    connect(ui->robotControl, &RobotControlForm::sigJoinValueChanged, this,
            &Robot3DForDDR6Form::slotJVarsValueChange, Qt::UniqueConnection);

    connect(ui->robotControl, &RobotControlForm::sigDValueChanged, this,
            &Robot3DForDDR6Form::slotDValueChanged, Qt::UniqueConnection);

    connect(ui->robotControl, &RobotControlForm::sigAValueChanged, this,
            &Robot3DForDDR6Form::slotAValueChanged, Qt::UniqueConnection);

    connect(ui->robotControl, &RobotControlForm::sigAlphaValueChanged, this,
            &Robot3DForDDR6Form::slotAlphaValueChanged, Qt::UniqueConnection);

    connect(ui->robotControl, &RobotControlForm::sigCheckStateChanged, this,
            &Robot3DForDDR6Form::slotUpdateGlobalConfig, Qt::UniqueConnection);

    // 实时模型运行
    connect(ui->robotControl, &RobotControlForm::sigSetModelRealTimeShow, this, [=](bool show) {
        isModelShow = show;
    });

    connect(ui->robotControl, &RobotControlForm::sigMoveToVirtual, this, &Robot3DForDDR6Form::signal_moveToVirtual);

    // 切换实物模型时将控制操作界面隐藏
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [=](int index) {
        QList<int> sizes;
        if (index == 0) {
            //ui->robotControl->setControlBoxVisible(true);
            sizes << 20000 << 10000;
            ui->splitter->setSizes(sizes);
        } else {
            //ui->robotControl->setControlBoxVisible(false);
            sizes << 100000 << 10000;
            ui->splitter->setSizes(sizes);
        }
    });
}

Robot3DForDDR6Form::~Robot3DForDDR6Form() {
    delete ui;
}

void Robot3DForDDR6Form::setControlModleJoints(QVector<double> joints) {
    if (joints.size() != 6) return;
    for (int index = 0; index != 6; index++) {
        ui->robot3D_virtual->mRobotConfig.JVars[index + 1] = joints[index];
        // 二关节取反
        if (index == 1) {
            ui->robot3D_virtual->mRobotConfig.JVars[index + 1] = -joints[index];
        }
    }
    //ui->robotControl->setSliderValue(joints);
    ui->robot3D_virtual->update();
}

void Robot3DForDDR6Form::slotJVarsValueChange(int index, int value) {
    ui->robot3D_virtual->mRobotConfig.JVars[index] = value;
    if (index == 2) {
        ui->robot3D_virtual->mRobotConfig.JVars[index] = -value;
    }
    ui->robot3D_virtual->update();
}

void Robot3DForDDR6Form::slotDValueChanged(int index, double value) {
    ui->robot3D_virtual->mRobotConfig.d[index] = value;
    ui->robot3D_virtual->updateGL();
}

void Robot3DForDDR6Form::slotAValueChanged(int index, double value) {
    ui->robot3D_virtual->mRobotConfig.a[index] = value;
    ui->robot3D_virtual->updateGL();
}

void Robot3DForDDR6Form::slotAlphaValueChanged(int index, double value) {
    ui->robot3D_virtual->mRobotConfig.alpha[index] = value;
    ui->robot3D_virtual->updateGL();
}

void Robot3DForDDR6Form::slotUpdateGlobalConfig() {
    ui->robot3D_virtual->mGlobalConfig.isDrawGrid = ui->robotControl->getIsRealGridChecked();
    ui->robot3D_virtual->mGlobalConfig.isDrawWorldCoord = ui->robotControl->getIsRealWorldCoord();
    ui->robot3D_virtual->mGlobalConfig.isDrawDesk = ui->robotControl->getIsRealShowDesk();
    // ui->robot3D_virtual->mGlobalConfig.isDrawJoint1Coord = ui->robotControl->getIsJointChecked(1);
    // ui->robot3D_virtual->mGlobalConfig.isDrawJoint2Coord = ui->robotControl->getIsJointChecked(2);
    // ui->robot3D_virtual->mGlobalConfig.isDrawJoint3Coord = ui->robotControl->getIsJointChecked(3);
    // ui->robot3D_virtual->mGlobalConfig.isDrawJoint4Coord = ui->robotControl->getIsJointChecked(4);
    // ui->robot3D_virtual->mGlobalConfig.isDrawJoint5Coord = ui->robotControl->getIsJointChecked(5);
    // ui->robot3D_virtual->mGlobalConfig.isDrawJoint6Coord = ui->robotControl->getIsJointChecked(6);

    ui->robot3D_virtual->updateGL();
}

void Robot3DForDDR6Form::hideRobot3D(bool hide) {
    if (hide == true) {
        ui->robot3D_virtual->hide();
    } else {
        ui->robot3D_virtual->show();
    }
}
