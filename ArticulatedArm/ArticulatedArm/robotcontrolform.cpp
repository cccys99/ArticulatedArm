#include "robotcontrolform.h"
#include "ui_robotcontrolform.h"
#include <QDebug>
#include <QtOpenGL>

RobotControlForm::RobotControlForm(QWidget *parent)
        : QWidget(parent), ui(new Ui::RobotControlForm) {
    ui->setupUi(this);

    initializeWindow();
}

RobotControlForm::~RobotControlForm() {
    delete ui;
}

void RobotControlForm::initializeWindow() {
    // checkBox 环境变化
    connect(ui->checkBoxGrid_real, &QCheckBox::stateChanged, this, [=](int state) {
        if (state == 0) {
            slotCheckStateChanged(false);
        } else if (state == 2) {
            slotCheckStateChanged(true);
        }
    });
    connect(ui->checkBoxWorldCoordinate_real, &QCheckBox::stateChanged, this, [=](int state) {
        if (state == 0) {
            slotCheckStateChanged(false);
        } else if (state == 2) {
            slotCheckStateChanged(true);
        }
    });
    connect(ui->checkBoxDesk_real, &QCheckBox::stateChanged, this, [=](int state) {
        if (state == 0) {
            slotCheckStateChanged(false);
        } else if (state == 2) {
            slotCheckStateChanged(true);
        }
    });
}

void RobotControlForm::slotDebugRobotConfig(double value) {
    QDoubleSpinBox *dsb = (QDoubleSpinBox *) sender();
    QString objectName = dsb->objectName();
    QString index = objectName.at(objectName.size() - 1);

    if (objectName.contains("doubleSpinBox_d")) {
        emit sigDValueChanged(index.toInt(), value);
    } else if (objectName.contains("doubleSpinBox_JVars")) {
        emit sigJoinValueChanged(index.toInt(), value);
    } else if (objectName.contains("doubleSpinBox_alpha")) {
        emit sigAlphaValueChanged(index.toInt(), value);
    } else if (objectName.contains("doubleSpinBox_a")) {
        emit sigAValueChanged(index.toInt(), value);
    }
}


bool RobotControlForm::getIsRealGridChecked() {
    return ui->checkBoxGrid_real->isChecked();
}

bool RobotControlForm::getIsRealWorldCoord() {
    return ui->checkBoxWorldCoordinate_real->isChecked();
}

bool RobotControlForm::getIsRealShowDesk() {
    return ui->checkBoxDesk_real->isChecked();
}

void RobotControlForm::slotCheckStateChanged(bool value) {
    if (sender()->objectName() == "checkBox_showModel") {
        emit sigSetModelRealTimeShow(value);
        return;
    }
    emit sigCheckStateChanged();
}

