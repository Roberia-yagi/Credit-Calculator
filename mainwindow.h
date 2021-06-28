#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void print_textBrowser(QString arg);

    void print_label_l_r(QString arg, int value, int max);

    void print_label_l_re(QString arg, int value, int max);

    void print_label_l_e(QString arg, int value, int max);

    void print_label_o(QString arg);

    void print_label_s_r_b(QString arg, int value, int max);

    void print_label_s_r_k(QString arg, int value, int max);

    void print_label_s_re_k(QString arg, int value, int max);

    void print_label_s_e_k(QString arg, int value, int max);

    void print_description(QString arg1, QString arg2, QString arg3, QString arg4, QString arg5);

    void print_total_progress(int value1, int max1, int value2, int max2, int value3, int max3);
private slots:
    void on_Send_clicked();

    void on_pushButton_l_r_clicked();

    void on_pushButton_prev_clicked();

    void on_pushButton_l_re_clicked();

    void on_pushButton_l_e_clicked();

    void on_pushButton_o_clicked();

    void on_pushButton_s_r_b_clicked();

    void on_pushButton_s_r_k_clicked();

    void on_pushButton_s_re_k_clicked();

    void on_pushButton_s_e_k_clicked();

private:
    Ui::MainWindow *ui;
    void insertGrade();
    void goPrev();
};
#endif // MAINWINDOW_H
