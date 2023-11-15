#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <QBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedLayout>
#include <QScroller>
#include <QLabel>
#include <QDebug>
#include <QTextEdit>
#include <QTextBrowser>
#include <QProcess>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <QProgressBar>
#include <QTreeView>
#include <QFileSystemModel>
#include <QHeaderView>

#define SHELLSCRIPT "\
while true\
            do\
            cat /tmp/psplash.txt | awk '{print $2}'\
        sleep 0.1\
        done\
"

class Filters : public QWidget{
    QPushButton* wicFilter;
    QPushButton* imgFilter;
    QLabel* status;
public:
    Filters(): QWidget() {
        setLayout(new QHBoxLayout());

        wicFilter = new QPushButton("Wic");
        wicFilter->setFixedHeight(60);
        wicFilter->setStyleSheet("font-size: 20px;");
        wicFilter->setCheckable(true);
        layout()->addWidget(wicFilter);

        imgFilter = new QPushButton("Img");
        imgFilter->setFixedHeight(60);
        imgFilter->setStyleSheet("font-size: 20px;");
        imgFilter->setCheckable(true);
        layout()->addWidget(imgFilter);

        //change this to a status of sdcard
        status = new QLabel("/dev/sdb");
        status->setFixedHeight(60);
        status->setAlignment(Qt::AlignCenter);
        status->setStyleSheet("font-size: 20px; color: green");
        layout()->addWidget(status);
    }
    QPushButton* getWicFilter(){
        return wicFilter;
    }
    QPushButton* getImgFilter(){
        return imgFilter;
    }
};

class ButtonStack : public QWidget{
    QWidget* contextButtonsFlash;
    QWidget* contextButtonsBack;
    QStackedLayout* layout;
    QPushButton* flashButton;
    QPushButton* backButton;
public:
    ButtonStack(): QWidget() {
        layout = new QStackedLayout();
        QWidget::setLayout(layout);

        contextButtonsFlash = new QWidget();
        contextButtonsFlash->setLayout(new QHBoxLayout());

        flashButton = new QPushButton("Flash");
        flashButton->setFixedHeight(60);
        flashButton->setStyleSheet("font-size: 20px;");
        contextButtonsFlash->layout()->addWidget(flashButton);

        /*QPushButton* printButton = new QPushButton("Print");
        printButton->setFixedHeight(60);
        printButton->setStyleSheet("font-size: 20px;");
        contextButtonsWic->layout()->addWidget(printButton);*/

        contextButtonsBack = new QWidget();
        contextButtonsBack->setLayout(new QHBoxLayout());

        backButton = new QPushButton("Back");
        backButton->setFixedHeight(60);
        backButton->setStyleSheet("font-size: 20px;");
        contextButtonsBack->layout()->addWidget(backButton);
        backButton->setEnabled(false);

        layout->addWidget(contextButtonsFlash);
        layout->addWidget(contextButtonsBack);

    }

    QPushButton* getFlashButton(){
        return flashButton;
    }
    QPushButton* getBackButton(){
        return backButton;
    }
    void displayFlash(){
        backButton->setEnabled(false);
        layout->setCurrentWidget(contextButtonsFlash);
    }
    void displayBack(){
        backButton->setEnabled(false);
        layout->setCurrentWidget(contextButtonsBack);
    }


};

class BmapOutput : public QWidget {
    QTextBrowser* console;
    QProgressBar* progressBar;
public:
    BmapOutput(): QWidget(){
        setLayout(new QVBoxLayout());
        console = new QTextBrowser();
        progressBar = new QProgressBar();
        layout()->addWidget(console);
        layout()->addWidget(progressBar);

    }

    void flash(const QString& path, ButtonStack* bStack){

        QProcess* proc = new QProcess();
        QProcess* procCat = new QProcess();
        procCat->setWorkingDirectory("/usr/bin");
        proc->setWorkingDirectory("/usr/bin");

        qInfo("%d",mkfifo("/tmp/psplash.txt",777));


        QObject::connect(procCat, &QProcess::readyReadStandardOutput, [procCat, this]() {
            auto output=procCat->readAllStandardOutput();
            progressBar->setValue(std::stoi(std::string(output)));
        });

        QObject::connect(proc, &QProcess::readyReadStandardOutput, [proc, this]() {
            auto output=proc->readAllStandardOutput();
            console->setText(output);
            repaint();
        });
        QObject::connect(proc, &QProcess::readyReadStandardError, [proc, this]() {
            auto output=proc->readAllStandardError();
            console->setText(output);
            repaint();
        });
        QObject::connect(proc, &QProcess::stateChanged, [proc, procCat, this, bStack]() {
            if(proc->state() == QProcess::ProcessState::Running){
                procCat->start(QString("/bin/bash"), QStringList({"cat.sh"}));
            }else if(proc->state()== QProcess::ProcessState::NotRunning){
                procCat->close();
                progressBar->setValue(100);
                remove("/tmp/psplash.txt");
                bStack->getBackButton()->setEnabled(true);
            }
        });

        proc->start(QString("/bin/bash"),QStringList({"scr.sh", path}));
        proc->waitForStarted();

        //mkfifo("/home/alex/psplash.txt",777);
        /*
        int fd1 = open("/home/alex/psplash.txt",O_RDONLY);

         while(proc->state() != QProcess::ProcessState::NotRunning){
            read(fd1, buffer, 12);
            qInfo("%s", buffer);
            sleep(1);
        }
        //qDebug() << proc->error();
        */
    }
};

class FilesAndBmap : public QWidget {
    QStackedLayout* layout;
    BmapOutput* bmapOutput;
    QFileSystemModel *model;
    QTreeView *tree;
    QString path;
    ButtonStack* bStack;
    QStringList filters;
public:
    FilesAndBmap(const QString& _path, ButtonStack* _bStack) : QWidget(){
        path = _path;
        bStack = _bStack;
        bmapOutput = new BmapOutput();
        model = new QFileSystemModel();
        model->setRootPath(path);
        model->setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs);


        filters = QStringList();
        model->setNameFilters((!filters.length()) ? QStringList({"*"}) : filters);
        model->setNameFilterDisables(false);

        tree = new QTreeView();
        tree->setModel(model);
        tree->setRootIndex(model->index(path));
        tree->setColumnHidden(1,1);
        tree->setColumnHidden(2,1);
        tree->setColumnHidden(3,1);
        tree->setStyleSheet("font-size: 40px;");
        tree->setFixedHeight(280);
        tree->resizeColumnToContents(0);
        tree->setHeaderHidden(1);
        tree->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        tree->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        tree->header()->setStretchLastSection(false);
        QScroller::grabGesture(tree, QScroller::LeftMouseButtonGesture);

        layout = new QStackedLayout();
        layout->addWidget(tree);
        layout->addWidget(bmapOutput);

        setLayout(layout);
    }
    void clearFilter(){
        filters = QStringList({"*.img", "*.wic.*"});
        model->setNameFilters(filters);
        model->setNameFilterDisables(false);
    }
public slots:
    void flash(){
        layout->setCurrentWidget(bmapOutput);
        bmapOutput->flash(model->filePath(tree->selectionModel()->selection().at(0).indexes().at(0)), bStack);
        bStack->displayBack();
    }
    void back(){
        layout->setCurrentWidget(tree);
        bStack->displayFlash();
    }
    void filterWic(bool checked){
        if(checked){
            filters.append(QString("*.wic.*"));
        }else{
            filters.removeIf([](QString x){
                return x == QString("*.wic.*");
            });
        }
        model->setNameFilters((!filters.length()) ? QStringList({"*"}) : filters);
        model->setNameFilterDisables(false);
    }
    void filterImg(bool checked){
        if(checked){
            filters.append(QString("*.img"));
        }else{
            filters.removeIf([](QString x){
                return x == QString("*.img");
            });
        }
        model->setNameFilters((!filters.length()) ? QStringList({"*"}) : filters);
        model->setNameFilterDisables(false);
    }

};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(800,480);
    ui->centralwidget->setLayout(new QVBoxLayout());

    QString path = getenv("HOME");
    path.append("/taos/build/tmp-glibc/deploy/images");

    //path.append("/taos");

    if (!system("ping -c 1 -w 1 taos  > /dev/null")){
        qInfo("%s", path.toLocal8Bit().toStdString().c_str());
        qInfo("%s", (QString("sshfs taos:/build/alex/st/stm32mp/poky ") + getenv("HOME") + "/taos -o allow_root").toLocal8Bit().toStdString().c_str());
        system((QString("sshfs taos:/build/alex/st/stm32mp/poky ") + getenv("HOME") + "/taos -o allow_root").toLocal8Bit().toStdString().c_str());
        qInfo("successful");
    }else{
        qInfo("not successful");
    }

    ButtonStack* bStack =  new ButtonStack();

    FilesAndBmap* filesAndBmap = new FilesAndBmap(path,bStack);
    connect(bStack->getFlashButton(),&QPushButton::clicked,filesAndBmap,&FilesAndBmap::flash);
    connect(bStack->getBackButton(),&QPushButton::clicked,filesAndBmap,&FilesAndBmap::back);

    Filters* filters = new Filters();
    connect(filters->getImgFilter(),&QPushButton::toggled,filesAndBmap,&FilesAndBmap::filterImg);
    connect(filters->getWicFilter(),&QPushButton::toggled,filesAndBmap,&FilesAndBmap::filterWic);

    ui->centralwidget->layout()->addWidget(filters);
    ui->centralwidget->layout()->addWidget(filesAndBmap);
    ui->centralwidget->layout()->addWidget(bStack);
}

MainWindow::~MainWindow()
{
    qInfo("%s", (QString("umount ") + getenv("HOME") + "/taos").toLocal8Bit().toStdString().c_str());
    system((QString("umount ") + getenv("HOME") + "/taos").toLocal8Bit().toStdString().c_str());
    delete ui;
}

