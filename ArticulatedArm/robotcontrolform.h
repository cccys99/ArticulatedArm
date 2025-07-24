#ifndef ROBOTCONTROLFORM_H
#define ROBOTCONTROLFORM_H

#include <array>
#include <QWidget>

namespace Ui {
    class RobotControlForm;
}

class RobotControlForm : public QWidget {
    Q_OBJECT

public:
    explicit RobotControlForm(QWidget *parent = 0);

    ~RobotControlForm();


    /**
   * @brief getIsJointChecked 是否显示关节的坐标系
   * @param jointNum 第几号关节
   * @return
   */

    /// 是否显示网格
    bool getIsRealGridChecked();

    /// 是否显示世界坐标系
    bool getIsRealWorldCoord();

    /// 是否显示桌子
    bool getIsRealShowDesk();

    /// 是否显示XY平台
    bool getIsShowXYPlatform();


public slots:

    void slotDebugRobotConfig(double value);

    void slotCheckStateChanged(bool value);

signals:
    void sigJoinValueChanged(int index, int value); // robot

    void sigDValueChanged(int index, double value);

    void sigAValueChanged(int index, double value);

    void sigAlphaValueChanged(int index, double value);

    void sigCheckStateChanged();                //所有checkbox发生改变

    void sigXYValueChanged(double x, double y);

    void sigSetModelRealTimeShow(bool show);

    void sigMoveToVirtual(const std::array<double, 7> &pos);

private:
    Ui::RobotControlForm *ui;

    //自定义单元格Type的类型，在创建单元格的item时使用
    enum CellType {
        ctJ1, ctJ2, ctJ3, ctJ4, ctJ5, ctJ6, ctX, ctY
    };
    //各字段在表格中的列号
    enum FieldColNum {
        colJ1 = 0,
        colJ2,
        colJ3,
        colJ4,
        colJ5,
        colJ6,
        colJ7,
        colJ8
    };
    int count = 0;

    void initializePointsheet();
    void initializeWindow();

};

#endif // ROBOTCONTROLFORM_H
