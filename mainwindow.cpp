#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <libudev.h>
#include <unistd.h>
#include <QSettings>

#define SHELLSCRIPT "\
while true\
            do\
            cat /tmp/psplash.txt | awk '{print $2}'\
        sleep 0.1\
        done\
"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(800,480);
    ui->centralwidget->setLayout(new QVBoxLayout());
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "burne");
    QString path;
    if (!settings.contains("path")) {
        path = getenv("HOME");
        settings.setValue("path",path);
    } else {
        path = settings.value("path").toString();
    }

    /*path.append("/taos/build/tmp-glibc/deploy/images");

    //path.append("/taos");

    if (!system("ping -c 1 -w 1 taos  > /dev/null")){
        qInfo("%s", path.toLocal8Bit().toStdString().c_str());
        qInfo("%s", (QString("sshfs taos:/build/alex/st/stm32mp/poky ") + getenv("HOME") + "/taos -o allow_root").toLocal8Bit().toStdString().c_str());
        system((QString("sshfs taos:/build/alex/st/stm32mp/poky ") + getenv("HOME") + "/taos -o allow_root").toLocal8Bit().toStdString().c_str());
        qInfo("successful");
    }else{
        qInfo("not successful");
    }*/

    bStack =  new ButtonStack();

    filesAndBmap = new FilesAndBmap(path,bStack);
    connect(bStack->getFlashButton(),&QPushButton::clicked,filesAndBmap,&FilesAndBmap::flash);
    connect(bStack->getBackButton(),&QPushButton::clicked,filesAndBmap,&FilesAndBmap::back);

    filters = new Filters();
    connect(filters->getImgFilter(),&QPushButton::toggled,filesAndBmap,&FilesAndBmap::filterImg);
    connect(filters->getWicFilter(),&QPushButton::toggled,filesAndBmap,&FilesAndBmap::filterWic);
    connect(filters->getDisksComboBox(), &QComboBox::textActivated, filesAndBmap, &FilesAndBmap::setBlockDevice);

    ui->centralwidget->layout()->addWidget(filters);
    ui->centralwidget->layout()->addWidget(filesAndBmap);
    ui->centralwidget->layout()->addWidget(bStack);
}

MainWindow::~MainWindow()
{
    //qInfo("%s", (QString("umount ") + getenv("HOME") + "/taos").toLocal8Bit().toStdString().c_str());
    //system((QString("umount ") + getenv("HOME") + "/taos").toLocal8Bit().toStdString().c_str());
    delete bStack;
    delete filesAndBmap;
    delete filters;
    delete layout();
    delete ui;
}

