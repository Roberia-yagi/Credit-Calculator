#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <bits/stdc++.h>
#include <QFile>
#include <QTextStream>
#include <QImage>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <math.h>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // イメージファイルを読み込む
    QString path = QCoreApplication::applicationDirPath();
    path.append("/pic.png");
    QPixmap pix(path);

    // イメージをシーンに追加する。
    ui->pic1->setPixmap(pix);
    ui->pic2->setPixmap(pix);
    ui->pic3->setPixmap(pix);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//授業データクラス
class Class {
public:
    string name;
    int liberal;
    int required;
    int type;
    int value;
    int classnum;
    vector<int> year = vector<int>(1);
    vector<int> semester = vector<int>(1);
    bool ac;

    void set(string name, int liberal, int required, int type, int value,
             int year, int semester) {
        this->name = name;
        this->liberal = liberal;
        this->required = required;
        this->type = type;
        this->value = value;
        this->classnum = 1;
        this->year[0] = year;
        this->semester[0] = semester;
        this->ac = false;
    }

    void print() {
        cout << name << " " << liberal << " " << required << " " << type << " "
         << value << endl;
    }
};

//クラスをまとめて管理するクラス
class Syllabus {
public:
    map<string, Class> classes;

    void addClass(string name, Class _class) {
        if (classes.count(name)) {
            classes[name] = _class;
        }
        classes[name] = _class;
    }

    void push(queue<string> &q, Class c) {
        string str;
        if (c.ac)
            str = "<font color=\"#ff6347\">○ " + c.name + "</font>";
        else
            str = "x " + c.name;
        q.push(str);
    }

    void printAll(MainWindow &mw, int l, int r, int t, string name) {
        name = "<font size=\"5\" color=\"#1e90ff\">" + name + "</font>";
        QString s = QString::fromStdString(name);
        mw.print_textBrowser(s);
        mw.print_textBrowser("<br>");
        queue<string> q;
        for (int i = 1; i < 5; i++) {
            for (int j = 1; j < 3; j++) {
                bool flag = false;
            QString output = "";
                for (auto _class : classes) {
                    Class c = _class.second;
                    for (int k = 0; k < c.classnum; k++) {
                        if (c.year[k]!= i || c.semester[k] != j) {
                            continue;
                        }
                        if (c.liberal == l && c.required == r && c.type == t) {
                            push(q, c);
                        }
                    }
                }
                if (!q.empty()) {
                    string tmp =" ";
                    tmp = tmp + "<font size=\"4\"  color=\"#008000\">" + to_string(i) + "年<br></font>";
                    if (j == 1)
                        tmp = tmp + "前期<br>";
                    else if (j == 2)
                        tmp = tmp + "後期<br>";
                    while (!q.empty()) {
                        flag = true;
                        tmp = tmp + q.front() + "<br>";
                        output = QString::fromStdString(tmp);
                        q.pop();
                    }
                }
                mw.print_textBrowser(output);
                if(flag){
                    mw.print_textBrowser("<br>");
                }
            }
        }
        mw.print_textBrowser("<br>");
    }
};

static Syllabus syllabus;

//講義データと実際の成績を管理するクラス
class Credit {
public:
    Class c;
    string grade;
    int score;
    double gp;

    void set(string name, string grade, int score, double gp) {
        c = syllabus.classes[name];
        this->grade = grade;
        this->score = score;
        this->gp = gp;
        if (score >= 60) {
            syllabus.classes[name].ac = true;
        }
    }

    void print() {
        c.print();
        cout << grade << " " << score << " " << gp << endl;
    }
};

vector<Credit> credits;

//classesファイルから静大の授業データを読み込む関数
int init() {
    QString path = QCoreApplication::applicationDirPath();
    path.append("/classes");
    string s = path.toStdString();
    cout << s <<endl;
    QFile file(path);
    if (!file.exists()){
        cout << "Non-exist" <<endl;
        return -1;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        cout << "Couldn't read" <<endl;
        return -1;
    }
    string str;

    cout << "aaa" <<endl;

    QTextStream in(&file);
    in.setCodec("UTF-8");
    while (!in.atEnd()) {
        QString line = in.readLine();
        str = line.toStdString();
        istringstream iss(str);
        string s;
        string name;
        int liberal = 0;
        int required = 0;
        int type = 0;
        int value = 0;
        int year = 0;
        int semester = 0;
        for (int i = 0; i < 7; i++) {
            iss >> s;
            if (i == 0)
                name = s;
            else if (i == 1)
                liberal = stoi(s);
            else if (i == 2)
                required = stoi(s);
            else if (i == 3)
                type = stoi(s);
            else if (i == 4)
                value = stoi(s);
            else if (i == 5)
                year = stoi(s);
            else if (i == 6)
                semester = stoi(s);
        }
        if (syllabus.classes.count(name)) {
            syllabus.classes[name].year.push_back(year);
            syllabus.classes[name].semester.push_back(semester);
            syllabus.classes[name].classnum++;
        } else {
            //cout << name << " " << liberal << " " << required << " " << type << " " << value << " " << year << " " << semester << endl;
            Class c;
            c.set(name, liberal, required, type, value, year, semester);
            syllabus.addClass(name, c);
        }
    }
    return 1;
}

void testCredit(vector<Credit> credits) {
    for (auto x : credits)
        x.print();
}

//ペーストされた単位情報を入力する関数
//科目名って文字まで読み込んだら実際の成績情報を読み込み始める
//分析した単位はCreditクラスとして保存する
bool read_grade(QStringList sl) {

    string line;
    int i = 0;
    int count = 0;
    string name;
    string grade;
    int score = 0;
    double gp = 0;
    cout << "loop starts" <<endl;
    bool flag = false;
    for(i = 0; i < sl.size(); i++){
        line = sl[i].toStdString();

        istringstream iss(line);
        string s;
        while (iss >> s) {
            if (s == "科目名") {
                cout << "skip finished" <<endl;
                i++;
                flag = true;
                break;
            }
        }
        if(flag) break;
    }
    if(!flag){
        return false;
    }
    for(; i < sl.size(); i++){
        cout << "i:" << i <<endl;
        cout << "reading grade starts" <<endl;
        line = sl[i].toStdString();

        if (line ==
                "Copyright (c) 2013 NTT DATA KYUSHU CORPORATION. All Rights Reserved.")
            return -1;
        istringstream iss(line);
        string s;
        int count_while = 0;

        while (iss >> s) {
            cout << count <<endl;
            if (line.empty()) {
                count++;
                continue;
            }
            switch (count) {
            case 0:
                if (count_while == 0) {
                    name = s;
                }
                break;

            case 1:
                grade = s;
                break;

            case 2:
                score = stoi(s);
                break;

            case 3:
                gp = stoi(s);
                break;
            }
            count_while++;
        }
        count++;
        if (count == 6) {
            Credit new_credit;
             cout << "added credit " << name << " " << grade << " " << score << " " << gp <<endl;
            new_credit.set(name, grade, score, gp);
            credits.push_back(new_credit);
            count = 0;
        }
    }
}

//成績データをGUIで表示させるために型を変える関数
QString toQString(string type, string key, string maxClass, map<string, int> grades){
  string tmp;
  tmp = type + to_string(grades[key]) + "/" + maxClass +  "\n";
  QString ret = QString::fromStdString(tmp);
  return ret;
}

QString toQString(string type, int total, string maxClass){
  string tmp;
  tmp = type + to_string(total) + "/" + maxClass +  "\n";
  QString ret = QString::fromStdString(tmp);
  return ret;
}

QString toQString(string type, string key, string maxClass, string sub1, string key2, string sub2, map<string, int> grades){
  string tmp;
  tmp = type + to_string(grades[key]) + "/" + maxClass + "\n";
  QString ret = QString::fromStdString(tmp);
  return ret;
}

QString toQString(string type, string key, string maxClass, string sub1, int i, string key2, string sub2, map<string, int> grades){
  string tmp;
  tmp = type + to_string(grades[key]) + "/" + maxClass + "\n";
  QString ret = QString::fromStdString(tmp);
  return ret;
}

QString toQString(string type, string key, map<string, int> grades){
  string tmp;
  tmp = type + to_string(grades[key]) + "\n";
  QString ret = QString::fromStdString(tmp);
  return ret;
}

QString toQString(string type, string key, string maxClass, string key2, map<string, int> grades){
  string tmp;
  tmp = type + to_string(grades[key] + min(grades["l_e_to_s_e"] + grades["key2"], 6)) + "/" + maxClass + "\n";
  QString ret = QString::fromStdString(tmp);
  return ret;
}

//GUIで成績情報を表示させるための関数
void description(MainWindow &mw, map<string, int> grades, int liberal, int specialize){
    string l_des;
    string l_total;
    string s_des;
    string s_total;
    string total;
    l_des = "教養\n";
    l_des = l_des + "選択必修の超過分は選択に移行可能\n 現在" + to_string(grades["l_re_kj_to_l_e"] + grades["l_re_ks_to_l_e"] + grades["l_re_kg_to_l_e"]) + "移行\n";
    l_des = l_des + "教養選択は専門選択へ6単位移行可能\n 残り" + to_string(min(6, 6 - grades["l_e_to_s_e"])) + "移行可能\n";
    l_total = l_total + "教養全体 " + to_string(liberal) + "/32\n\n";

    s_des = s_des + "専門\n";
    s_des = s_des + "専門選択は教養選択とその他から\n6単位まで移行可能\n 現在" + to_string(min(6, grades["l_e_to_s_e"] + grades["o"])) + "移行\n";
    s_total = s_total + "専門全体 "  + to_string(specialize) + "/92\n\n";

    total = total + "全体 " + to_string(grades["total"]) + "取得済み\n";
    total = total + "うち，卒業に有効な単位は\n                           " + to_string(grades["total_valid"]) + "/124";
    QString arg1 = QString::fromStdString(l_des);
    QString arg2 = QString::fromStdString(l_total);
    QString arg3 = QString::fromStdString(s_des);
    QString arg4 = QString::fromStdString(s_total);
    QString arg5 = QString::fromStdString(total);
    mw.print_description(arg1, arg2, arg3, arg4, arg5);
}

void print(MainWindow &mw, map<string, int> grades) {
    int liberal_total = grades["l_r"] + grades["l_re"] + grades["l_re_kj"] +
            grades["l_re_ks"] + grades["l_re_kg"] + grades["l_e"];
    int specialized_total =
            grades["s_r_b"] + grades["s_r_k"] + grades["s_re_k"] + grades["s_e_k"];

    //教養
    QString l_r;
    QString l_re;
    QString l_e;
    QString s_r_b;
    QString s_r_k;
    QString s_re_k;
    QString s_e_k;
    QString o;
    QString output;
    l_r.append(toQString("教養 必修 ", "l_r", "4", grades));
    mw.print_label_l_r(l_r, grades["l_r"], 4);
    l_re.append(toQString("教養 選択必修 ", "l_re", "1", grades));
    l_re.append(toQString("人文 ", "l_re_kj", "4", "(", "l_re_kj_to_l_e", "単位 教養選択へ", grades));
    l_re.append(toQString("自然 ", "l_re_ks", "4", "(", "l_re_ks_to_l_e", "単位 教養選択へ", grades));
    l_re.append(toQString("学際 ", "l_re_kg", "4", "(", "l_re_kg_to_l_e", "単位 教養選択へ", grades));
    int sum = (grades["l_re_kj"] + grades["l_re_ks"] + grades["l_re_kg"]);
    mw.print_label_l_re(l_re, sum, 12);
    l_e.append(toQString("教養 選択 ", "l_e", "15", "(残り", 6, "l_e_to_s_e", "単位 専門選択へ持ち越し可", grades));
    mw.print_label_l_e(l_e, grades["l_e"], 15);

    //専門
    s_r_b.append(toQString("専門 必修 学部", "s_r_b", "16", grades));
    mw.print_label_s_r_b(s_r_b, grades["s_r_b"], 16);
    s_r_k.append(toQString("専門 必修 学科", "s_r_k", "48", grades));
    mw.print_label_s_r_k(s_r_k, grades["s_r_k"], 48);
    s_re_k.append(toQString("専門 選択必修", "s_re_k", "8", grades));
    mw.print_label_s_re_k(s_re_k, grades["s_re_k"], 8);
    s_e_k.append(toQString("専門 選択", "s_e_k", "20", "o", grades));
    mw.print_label_s_e_k(s_e_k, grades["s_e_k"], 20);

    o.append(toQString("その他の科目", "o", grades));
    mw.print_label_o(o);

    //説明
    description(mw, grades, liberal_total, specialized_total);
    mw.print_total_progress(liberal_total, 32, specialized_total, 92, grades["total_valid"], 124);
    /*
    //合計
    cout << "全体で " << grades["total"] << "習得済" << endl;
    cout << "うち卒業に有効なもの " << grades["total_valid"] << "/" << 126
         << "習得済" << endl;
    */
}

void add_to_l_e(map<string, int> &grades, bool &valid, Class &c) {
    if (grades["l_e"] + grades["o"] < 21)
        valid = true;
    grades["l_e"] += c.value;
    if (grades["l_e"] >= 15) {
        grades["l_e_to_s_e"] += c.value;
    }
}

//入力された成績データから取得単位数をまとめる関数
void calc_grade(MainWindow &mw) {
    map<string, int> grades;
    for (auto credit : credits) {
        bool valid = false;
        Class c = credit.c;
        if (credit.grade == "不可" || credit.grade == "その他")
            continue;
        if (c.liberal == 0) {
            //教養必修
            if (c.required == 0) {
                if (grades["l_r"] < 4)
                    valid = true;
                grades["l_r"] += c.value;
            } else if (c.required == 1) {
                //教養選択必修
                if (c.type == 0) {
                    if (grades["l_re"] < 1)
                        valid = true;
                    grades["l_re"] += c.value;
                    //教養選択必修 個別 人文
                } else if (c.type == 1) {
                    if (grades["l_re_kj"] < 4)
                        valid = true;
                    else if (grades["l_re_kj"] >= 4) {
                        grades["l_re_kj_to_l_e"] += c.value;
                        add_to_l_e(grades, valid, c);
                    }
                    grades["l_re_kj"] += c.value;
                    //教養選択必修 個別 自然
                } else if (c.type == 2) {
                    if (grades["l_re_ks"] < 4)
                        valid = true;
                    else if (grades["l_re_ks"] >= 4) {
                        add_to_l_e(grades, valid, c);
                        grades["l_re_ks_to_l_e"] += c.value;
                    }
                    grades["l_re_ks"] += c.value;
                    //教養選択必修 個別 学際
                } else if (c.type == 3) {
                    if (grades["l_re_g"] < 4)
                        valid = true;
                    else if (grades["l_re_kg"] >= 4) {
                        add_to_l_e(grades, valid, c);
                        grades["l_re_kg_to_l_e"] += c.value;
                    }
                    grades["l_re_g"] += c.value;
                }
                //教養選択
            } else if (c.required == 2) {
                add_to_l_e(grades, valid, c);
            }
        } else if (c.liberal == 1) {
            if (c.required == 0) {
                //専門 必修 学部
                if (c.type == 0) {
                    if (grades["s_r_b"] < 16)
                        valid = true;
                    grades["s_r_b"] += c.value;
                    //専門 必修 学科
                } else if (c.type == 1) {
                    if (grades["s_r_k"] < 48)
                        valid = true;
                    grades["s_r_k"] += c.value;
                }
                //専門 選択必修
            } else if (c.required == 1) {
                if (grades["s_re_k"] < 8)
                    valid = true;
                grades["s_re_k"] += c.value;
                //専門 選択
            } else if (c.required == 2) {
                if (grades["s_e_k"] < 20)
                    valid = true;
                grades["s_e_k"] += c.value;
            }
            //その他
        } else if (c.liberal == 2) {
            grades["o"] += c.value;
            if (grades["l_e"] + grades["o"] < 21)
                valid = true;
        }
        grades["total"] += c.value;
        if (valid)
            grades["total_valid"] += c.value;
    }
    print(mw, grades);
}

void printSyllabus() {
    for (auto x : syllabus.classes) {
        Class c = x.second;
        cout << c.name << endl;
        for (int i = 0; i < c.classnum; i++) {
            cout << c.liberal << " " << c.required << " " << c.type << " "
           << c.year[i] << "年 " << c.semester[i] << endl;
        }
    }
}

//テスト用の関数
bool test(){
    map<string, int> mp;
    mp["テスト"] = 1;
    QString path = QCoreApplication::applicationDirPath();
    path.append("/test.txt");
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        cout << "Couldn't read" <<endl;
        return -1;
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");
    while (!in.atEnd()) {
        QString line = in.readLine();
        string str = line.toStdString();
        if(mp[str] == 1) return 1;
        else return -1;
    }
}

//以下GUI用の関数

void MainWindow::on_Send_clicked()
{
    if(test()){
      cout << "test clear" <<endl;
    }
    else{
        cout << "test unclear" <<endl;
    }
    init();
    cout << "clicked" <<endl;
    QString inputString = ui->textEdit->toPlainText();
    QStringList list = inputString.split(QRegExp("\n"));
    if(read_grade(list)){
         auto idx = ui->stackedWidget->currentIndex();
         ui->stackedWidget->setCurrentIndex(++idx);
         insertGrade();
        calc_grade(*this);
    }
    else{
        ui->Error->setText("入力が不正です．");
    }
}

void MainWindow::print_label_l_r(QString arg, int value, int max){
    ui->pushButton_l_r->setText(arg);
    ui->progressBar_l_r->setValue(value);
    ui->progressBar_l_r->setMaximum(max);
}

void MainWindow::print_label_l_re(QString arg, int value, int max){
    ui->pushButton_l_re->setText(arg);
    ui->progressBar_l_re->setValue(value);
    ui->progressBar_l_re->setMaximum(max);
}

void MainWindow::print_label_l_e(QString arg, int value, int max){
    ui->pushButton_l_e->setText(arg);
    ui->progressBar_l_e->setValue(value);
    ui->progressBar_l_e->setMaximum(max);
}

void MainWindow::print_label_o(QString arg){
    ui->pushButton_o->setText(arg);
}

void MainWindow::print_label_s_r_b(QString arg, int value, int max){
    ui->pushButton_s_r_b->setText(arg);
    ui->progressBar_s_r_b->setValue(value);
    ui->progressBar_s_r_b->setMaximum(max);
}

void MainWindow::print_label_s_r_k(QString arg, int value, int max){
    ui->pushButton_s_r_k->setText(arg);
    ui->progressBar_s_r_k->setValue(value);
    ui->progressBar_s_r_k->setMaximum(max);
}

void MainWindow::print_label_s_re_k(QString arg, int value, int max){
    ui->pushButton_s_re_k->setText(arg);
    ui->progressBar_s_re_k->setValue(value);
    ui->progressBar_s_re_k->setMaximum(max);
}

void MainWindow::print_label_s_e_k(QString arg, int value, int max){
    ui->pushButton_s_e_k->setText(arg);
    ui->progressBar_s_e_k->setValue(value);
    ui->progressBar_s_e_k->setMaximum(max);
}

void MainWindow::print_textBrowser(QString arg){
    ui->state_classes->insertHtml(arg);
}


void MainWindow::insertGrade()
{
    auto idx = ui->stackedWidget->currentIndex();

    if (idx == 1) {
        ui->Send->setEnabled(false);
        return;
    } else if (idx == 0) {
        ui->Send->setEnabled(true);
        return;
    }

    ui->Send->setEnabled(true);
}

void MainWindow::goPrev()
{
    auto idx = ui->stackedWidget->currentIndex();

    if (idx == 1) {
        ui->pushButton_l_r->setEnabled(true);
        ui->pushButton_prev->setEnabled(false);
        return;
    } else if (idx == 0) {
        ui->pushButton_l_r->setEnabled(false);
        ui->pushButton_prev->setEnabled(true);
        return;
    }

    ui->pushButton_l_r->setEnabled(true);
    ui->pushButton_prev->setEnabled(true);
}


void MainWindow::on_pushButton_prev_clicked()
{
    auto idx = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(--idx);
    ui->state_classes->clear();
}

void MainWindow::on_pushButton_l_r_clicked()
{
    auto idx = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(++idx);
    syllabus.printAll(*this, 0, 0, 0, "教養 必修");
}

void MainWindow::on_pushButton_l_re_clicked()
{
    auto idx = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(++idx);
    syllabus.printAll(*this, 0, 1, 0, "教養 選択必修");
    syllabus.printAll(*this, 0, 1, 1, "教養 選択必修 人文");
    syllabus.printAll(*this, 0, 1, 2, "教養 選択必修 自然");
    syllabus.printAll(*this, 0, 1, 3, "教養 選択必修 学際");
}

void MainWindow::on_pushButton_l_e_clicked()
{
    auto idx = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(++idx);
    syllabus.printAll(*this, 0, 2, 0, "教養 選択");
}

void MainWindow::on_pushButton_o_clicked()
{
    auto idx = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(++idx);
    syllabus.printAll(*this, 2, 0, 0, "その他");
}

void MainWindow::on_pushButton_s_r_b_clicked()
{
    auto idx = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(++idx);
    syllabus.printAll(*this, 1, 0, 0, "専門 必修 学部");
}

void MainWindow::on_pushButton_s_r_k_clicked()
{
    auto idx = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(++idx);
    syllabus.printAll(*this, 1, 0, 1, "専門 必修 学科");
}

void MainWindow::on_pushButton_s_re_k_clicked()
{
    auto idx = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(++idx);
    syllabus.printAll(*this, 1, 1, 1, "専門 選択必修");
}

void MainWindow::on_pushButton_s_e_k_clicked()
{
    auto idx = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(++idx);
    syllabus.printAll(*this, 1, 2, 1, "専門 選択");
}


void MainWindow::print_description(QString arg1, QString arg2, QString arg3, QString arg4, QString arg5){
    ui->label_l_des->setText(arg1);
    ui->label_l_total->setText(arg2);
    ui->label_s_des->setText(arg3);
    ui->label_s_total->setText(arg4);
    ui->label_total->setText(arg5);
}

void MainWindow::print_total_progress(int value1, int max1, int value2, int max2, int value3, int max3){
    ui->progressBar_l_total->setValue(value1);
    ui->progressBar_l_total->setMaximum(max1);
    ui->progressBar_s_total->setValue(value2);
    ui->progressBar_s_total->setMaximum(max2);
    ui->progressBar_total->setValue(value3);
    ui->progressBar_total->setMaximum(max3);
}
