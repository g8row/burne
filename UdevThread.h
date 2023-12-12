#ifndef UDEVTHREAD_H
#define UDEVTHREAD_H

#include <libudev.h>
#include <QThread>


class UdevThread : public QThread
{
    Q_OBJECT
    void run() {
        struct udev *udev;
        struct udev_device *dev;
        udev = udev_new();
        if (!udev) {
            qInfo("Can't create udev\n");
            exit(1);
        }
        // Set up a monitor to monitor hidraw devices
        struct udev_monitor *mon = udev_monitor_new_from_netlink(udev, "udev");
        udev_monitor_filter_add_match_subsystem_devtype(mon, "block", NULL);
        udev_monitor_enable_receiving(mon);
        while (1) {
            int fd = udev_monitor_get_fd(mon);
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
                //qInfo("\nselect() says there should be data\n");

                // Make the call to receive the device. select() ensured that this will not block.
                dev = udev_monitor_receive_device(mon);
                if (dev) {
                    /* qInfo("Got Device\n");
                        qInfo("   Node: %s\n", udev_device_get_devnode(dev));
                        qInfo("   Subsystem: %s\n", udev_device_get_subsystem(dev));
                        qInfo("   Devtype: %s\n", udev_device_get_devtype(dev));

                        qInfo("   Action: %s\n",udev_device_get_action(dev));
                        */
                    QString udevAction(udev_device_get_action(dev));
                    QString udevNode(udev_device_get_devnode(dev));
                    if (strcmp(udev_device_get_devtype(dev), "disk") == 0 && strstr(udev_device_get_devnode(dev), "sd") != NULL ) {
                        emit resultReady(udevAction, udevNode + " " + udev_device_get_property_value(dev, "ID_MODEL"));
                        //qInfo("%s\n",udev_device_get_property_value(dev, "ID_MODEL"));
                    }

                    udev_device_unref(dev);

                }
                else {
                    qInfo("No Device from receive_device(). An error occured.\n");
                }
            }
            usleep(250*100);
        }

        // Free the enumerator object

        udev_unref(udev);
    }
signals:
    void resultReady(const QString &action, const QString &devnode);
};

#endif // UDEVTHREAD_H
