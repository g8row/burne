#ifndef FILTERS_H
#define FILTERS_H

#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QScroller>
#include <QAbstractItemView>
#include <libudev.h>
#include <UdevThread.h>

class Filters : public QWidget{
    QPushButton* wicFilter;
    QPushButton* imgFilter;
    QComboBox* disks;
    QLayout* layout;
public:
    Filters(): QWidget() {
        struct udev *udev;
        struct udev_enumerate *enumerate;
        struct udev_list_entry *devices, *dev_list_entry;
        struct udev_device *dev;

        layout = new QHBoxLayout();
        setLayout(layout);

        wicFilter = new QPushButton("Wic");
        wicFilter->setFixedHeight(60);
        wicFilter->setStyleSheet("font-size: 20px;");
        wicFilter->setCheckable(true);
        layout->addWidget(wicFilter);

        imgFilter = new QPushButton("Img");
        imgFilter->setFixedHeight(60);
        imgFilter->setStyleSheet("font-size: 20px;");
        imgFilter->setCheckable(true);
        layout->addWidget(imgFilter);

        disks = new QComboBox();
        QScroller::grabGesture(disks->view()->viewport(), QScroller::LeftMouseButtonGesture);
        disks->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        disks->view()->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        disks->view()->setAutoScroll(false);

        //disks->setFixedHeight(200);
        //disks->view()->setFixedHeight(200);
        disks->setMaxVisibleItems(7);
        disks->setInsertPolicy(QComboBox::InsertAlphabetically);
        disks->addItem("<no device>");

        udev = udev_new();
        if (!udev) {
            qInfo("Can't create udev\n");
            exit(1);
        }

        enumerate = udev_enumerate_new(udev);
        udev_enumerate_add_match_subsystem(enumerate, "block");
        udev_enumerate_scan_devices(enumerate);
        devices = udev_enumerate_get_list_entry(enumerate);
        QStringList devicesPaths;
        udev_list_entry_foreach(dev_list_entry, devices) {
            const char *path;
            path = udev_list_entry_get_name(dev_list_entry);
            dev = udev_device_new_from_syspath(udev, path);

            //qInfo("Got Device\n");
            if (strcmp(udev_device_get_devtype(dev), "disk") == 0 && strstr(udev_device_get_devnode(dev), "sd") != NULL ) {
                devicesPaths.append(QString(udev_device_get_devnode(dev)) + " " + udev_device_get_property_value(dev, "ID_MODEL"));
            }
            //qInfo("   Subsystem: %s\n", udev_device_get_subsystem(dev));
            //qInfo("   Devtype: %s\n", udev_device_get_devtype(dev));
            udev_device_unref(dev);
        }
        udev_enumerate_unref(enumerate);
        udev_unref(udev);
        devicesPaths.sort();
        disks->addItems(devicesPaths);

        //change this to a status of sdcard
        disks->setFixedHeight(60);
        //disks->setAlignment(Qt::AlignCenter);
        disks->setStyleSheet("font-size: 20px; combobox-popup: 0;");
        layout->addWidget(disks);

        startUdevMonitorThread();
    }



    void startUdevMonitorThread()
    {
        UdevThread *udevThread = new UdevThread();
        connect(udevThread, &UdevThread::resultReady, this, &Filters::handleResults);
        connect(udevThread, &UdevThread::finished, udevThread, &QObject::deleteLater);
        udevThread->start();
    }

    QPushButton* getWicFilter(){
        return wicFilter;
    }

    QPushButton* getImgFilter(){
        return imgFilter;
    }

    QComboBox* getDisksComboBox() {
        return disks;
    }

    ~Filters(){
        delete wicFilter;
        delete imgFilter;
        delete disks;
        delete layout;
    }

public slots:
    void handleResults(QString action, QString devnode){
        qInfo("%s", action.toStdString().c_str());
        qInfo("%s", devnode.toStdString().c_str());
        if (action.compare("add") == 0) {
            disks->addItem(devnode);
        }
        if (action.compare("remove") == 0) {
            disks->removeItem(disks->findText(devnode));
        }
    }
};

#endif // FILTERS_H
