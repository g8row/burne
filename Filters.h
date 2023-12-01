#ifndef FILTERS_H
#define FILTERS_H

#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <libudev.h>

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
        disks->addItem("<no device>");
        disks->setInsertPolicy(QComboBox::InsertAlphabetically);

        udev = udev_new();
        if (!udev) {
            qInfo("Can't create udev\n");
            exit(1);
        }

        enumerate = udev_enumerate_new(udev);
        udev_enumerate_add_match_subsystem(enumerate, "block");
        udev_enumerate_scan_devices(enumerate);
        devices = udev_enumerate_get_list_entry(enumerate);

        udev_list_entry_foreach(dev_list_entry, devices) {
            const char *path;
            path = udev_list_entry_get_name(dev_list_entry);
            dev = udev_device_new_from_syspath(udev, path);

            //qInfo("Got Device\n");
            if (strcmp(udev_device_get_devtype(dev), "disk") == 0) {
                disks->addItem(udev_device_get_devnode(dev));
                disks->addItem(udev_device_get_sysname(dev));

            }
            //qInfo("   Subsystem: %s\n", udev_device_get_subsystem(dev));
            //qInfo("   Devtype: %s\n", udev_device_get_devtype(dev));
            udev_device_unref(dev);
        }
        udev_enumerate_unref(enumerate);
        udev_unref(udev);

        //change this to a status of sdcard
        disks->setFixedHeight(60);
        //disks->setAlignment(Qt::AlignCenter);
        disks->setStyleSheet("font-size: 20px");
        layout->addWidget(disks);
    }
    QPushButton* getWicFilter(){
        return wicFilter;
    }
    QPushButton* getImgFilter(){
        return imgFilter;
    }

    ~Filters(){
        delete wicFilter;
        delete imgFilter;
        delete disks;
        delete layout;
    }
};

#endif // FILTERS_H
