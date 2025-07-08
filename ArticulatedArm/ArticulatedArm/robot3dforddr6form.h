#ifndef ROBOT3DFORDDR6FORM_H
#define ROBOT3DFORDDR6FORM_H

#include <QWidget>

namespace Ui {
    class Robot3DForDDR6Form;
}

class Robot3DForDDR6Form : public QWidget {
    Q_OBJECT

public:
    explicit Robot3DForDDR6Form(QWidget *parent = nullptr);

    ~Robot3DForDDR6Form();

    /// 模型隐藏
    void hideRobot3D(bool);

    /// 按照接收数据显示实时的关节
    void showRealTimePos(QVector<double> joints);

    /// 控制模型移动
    void setControlModleJoints(QVector<double> joints);

private:
    Ui::Robot3DForDDR6Form *ui;
    /// 实物模型运行状态
    bool isModelShow = false;

private slots:
    /// 关节Z方向旋转
    void slotJVarsValueChange(int index, int value);

    /// 关节Z方向平移
    void slotDValueChanged(int index, double value);

    /// 关节X方向平移
    void slotAValueChanged(int index, double value);

    /// 关节X方向旋转
    void slotAlphaValueChanged(int index, double value);

    /// 全局场景显示状态配置
    void slotUpdateGlobalConfig();

signals:
    /// 信号：控制机械臂移动到控制模型当前角度
    void signal_moveToVirtual(const std::array<double, 7> &pos);
};

#endif // ROBOT3DFORDDR6FORM_H
