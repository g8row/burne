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
        layout->addWidget(progressBar);
    }

    void flash(const QString& path, const QString& blockDevice,ButtonStack* bStack){
        proc = new QProcess();
        procCat = new QProcess();
        procCat->setWorkingDirectory("/usr/bin");
        proc->setWorkingDirectory("/usr/bin");
        qInfo("%d",mkfifo("/tmp/psplash.txt",777));
        QObject::connect(procCat, &QProcess::readyReadStandardOutput, [this]() {
            auto output=procCat->readAllStandardOutput();
            progressBar->setValue(std::stoi(std::string(output)));
        });

        QObject::connect(proc, &QProcess::readyReadStandardOutput, [this]() {
            auto output=proc->readAllStandardOutput();
            console->append(output);
            repaint();
        });
        QObject::connect(proc, &QProcess::readyReadStandardError, [this]() {
            auto output=proc->readAllStandardError();
            console->append(output);
            repaint();
        });
        QObject::connect(proc, &QProcess::finished, [this, bStack]() {
            proc->kill();
            proc->deleteLater();
            procCat->deleteLater();
            progressBar->setValue(100);
            remove("/tmp/psplash.txt");
            bStack->getBackButton()->setEnabled(true);
        });

        QObject::connect(proc, &QProcess::started, [this]() {
            console->clear();
            repaint();
            procCat->start(QString("/bin/sh"), QStringList({"cat.sh"}));
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
    }
};

#endif // BMAPOUTPUT_H
