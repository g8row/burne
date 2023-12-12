#ifndef BMAPOUTPUT_H
#define BMAPOUTPUT_H

#include <filesystem>
#include "ButtonStack.h"
#include <iostream>
#include <QBoxLayout>
#include <QScroller>
#include <QTextBrowser>
#include <QProcess>
#include <QProgressBar>
#include <sys/stat.h>

class BmapOutput : public QWidget {
    QTextBrowser* console;
    QProgressBar* progressBar;
    QLayout* layout;
    QProcess* proc;
    QProcess* procCat;
public:
    BmapOutput(): QWidget(){
        layout = new QVBoxLayout();
        setLayout(layout);
        console = new QTextBrowser();
        progressBar = new QProgressBar();
        layout->addWidget(console);
        layout->addWidget(progressBar);\
            proc = new QProcess();
        procCat = new QProcess();
        procCat->setWorkingDirectory("/usr/bin");
        proc->setWorkingDirectory("/usr/bin");
    }

    void flash(const QString& path, const QString& blockDevice,ButtonStack* bStack){
        qInfo("%d",mkfifo("/tmp/psplash.txt",777));
        QObject::connect(procCat, &QProcess::readyReadStandardOutput, [this]() {
            auto output=procCat->readAllStandardOutput();
            progressBar->setValue(std::stoi(std::string(output)));
        });

        QObject::connect(proc, &QProcess::readyReadStandardOutput, [this]() {
            auto output=proc->readAllStandardOutput();
            console->setText(output);
            repaint();
        });
        QObject::connect(proc, &QProcess::readyReadStandardError, [this]() {
            auto output=proc->readAllStandardError();
            console->setText(output);
            repaint();
        });
        QObject::connect(proc, &QProcess::stateChanged, [this, bStack]() {
            if(proc->state() == QProcess::ProcessState::Running){
                procCat->start(QString("/bin/sh"), QStringList({"cat.sh"}));
            }else if(proc->state()== QProcess::ProcessState::NotRunning){
                procCat->close();
                proc->close();
                progressBar->setValue(100);
                remove("/tmp/psplash.txt");
                bStack->getBackButton()->setEnabled(true);
            }
        });
        proc->start(QString("/bin/sh"),QStringList({"scr.sh", path, blockDevice.split(" ")[0]}));
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

    ~BmapOutput(){
        delete console;
        delete progressBar;
        delete layout;
        delete proc;
        delete procCat;
    }
};

#endif // BMAPOUTPUT_H
