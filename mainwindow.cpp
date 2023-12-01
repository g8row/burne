#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <libudev.h>
#include <unistd.h>

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

    bStack =  new ButtonStack();

    filesAndBmap = new FilesAndBmap(path,bStack);
    connect(bStack->getFlashButton(),&QPushButton::clicked,filesAndBmap,&FilesAndBmap::flash);
    connect(bStack->getBackButton(),&QPushButton::clicked,filesAndBmap,&FilesAndBmap::back);

    filters = new Filters();
    connect(filters->getImgFilter(),&QPushButton::toggled,filesAndBmap,&FilesAndBmap::filterImg);
    connect(filters->getWicFilter(),&QPushButton::toggled,filesAndBmap,&FilesAndBmap::filterWic);

    ui->centralwidget->layout()->addWidget(filters);
    ui->centralwidget->layout()->addWidget(filesAndBmap);
    ui->centralwidget->layout()->addWidget(bStack);

    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev;

    /* Create the udev object
    udev = udev_new();
    if (!udev) {
        printf("Can't create udev\n");
        exit(1);
    }

    // Set up a monitor to monitor hidraw devices
    struct udev_monitor *mon = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(mon, "block", NULL);
    udev_monitor_enable_receiving(mon);
    // Get the file descriptor (fd) for the monitor. \
           This fd will get passed to select()
    int fd = udev_monitor_get_fd(mon);

    // This section will run continuously, calling usleep() at \
           the end of each pass. This is to demonstrate how to use\
           a udev_monitor in a non-blocking way. \
    while (1) {
        // Set up the call to select(). In this case, select() will \
                   only operate on a single file descriptor, the one \
                   associated with our udev_monitor. Note that the timeval\
                   object is set to 0, which will cause select() to not\
                   block.
        fd_set fds;
        struct timeval tv;
        int ret;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        ret = select(fd+1, &fds, NULL, NULL, &tv);

        // Check if our file descriptor has received data.
        if (ret > 0 && FD_ISSET(fd, &fds)) {
            qInfo("\nselect() says there should be data\n");

            // Make the call to receive the device. select() ensured that this will not block.
            dev = udev_monitor_receive_device(mon);
            if (dev) {
                qInfo("Got Device\n");
                qInfo("   Node: %s\n", udev_device_get_devnode(dev));
                qInfo("   Subsystem: %s\n", udev_device_get_subsystem(dev));
                qInfo("   Devtype: %s\n", udev_device_get_devtype(dev));

                qInfo("   Action: %s\n",udev_device_get_action(dev));
                udev_device_unref(dev);
            }
            else {
                qInfo("No Device from receive_device(). An error occured.\n");
            }
        }
        usleep(250*1000);
        qInfo(".");
    }

    // Free the enumerator object

    udev_unref(udev);
    */

}

MainWindow::~MainWindow()
{
    qInfo("%s", (QString("umount ") + getenv("HOME") + "/taos").toLocal8Bit().toStdString().c_str());
    system((QString("umount ") + getenv("HOME") + "/taos").toLocal8Bit().toStdString().c_str());
    delete bStack;
    delete filesAndBmap;
    delete filters;
    delete layout();
    delete ui;
}

