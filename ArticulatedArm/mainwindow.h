#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "robotdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    ///窗口大小变化
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    Ui::MainWindow *ui;
    RobotDialog *robotDialog;

private:
    /// 初始化窗口
    void initializeWindow();

};
#endif // MAINWINDOW_H
