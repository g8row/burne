#ifndef FILESANDBMAP_H
#define FILESANDBMAP_H

#include "BmapOutput.h"
#include "ButtonStack.h"
#include <QHeaderView>
#include <QScroller>
#include <QTreeView>
#include <QFileSystemModel>
#include <QDebug>


class FilesAndBmap : public QWidget {
    QStackedLayout* layout;
    BmapOutput* bmapOutput;
    QFileSystemModel *model;
    QTreeView *tree;
    QString path;
    ButtonStack* bStack;
    QStringList* filters;
public:
    FilesAndBmap(const QString& _path, ButtonStack* _bStack) : QWidget(){
        path = _path;
        bStack = _bStack;
        bmapOutput = new BmapOutput();
        model = new QFileSystemModel();
        model->setRootPath(path);
        model->setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs);

        filters = new QStringList({"*"});
        model->setNameFilters(*filters);
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
        //tree->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        tree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        tree->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

        tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        tree->header()->setStretchLastSection(false);
        //tree->setAutoScroll(true);
        tree->setExpandsOnDoubleClick(true);

        QScroller::grabGesture(tree, QScroller::LeftMouseButtonGesture);
        tree->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        tree->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

        layout = new QStackedLayout();
        layout->addWidget(tree);
        layout->addWidget(bmapOutput);

        setLayout(layout);
    }

    ~FilesAndBmap(){
        delete layout;
        delete bmapOutput;
        delete model;
        delete filters;
    }

    void clearFilter(){
        delete filters;
        filters = new QStringList({"*.img", "*.wic.*"});
        model->setNameFilters(*filters);
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
            filters->removeIf([](QString x){
                return x == QString("*");
            });
            filters->append(QString("*.wic.*"));
        }else{
            filters->removeIf([](QString x){
                return x == QString("*.wic.*");
            });
        }
        if(!filters->length()){
            delete filters;
            filters = new QStringList({"*"});
        }
        model->setNameFilters(*filters);
        model->setNameFilterDisables(false);
    }
    void filterImg(bool checked){
        if(checked){
            filters->removeIf([](QString x){
                return x == QString("*");
            });
            filters->append(QString("*.img"));
        }else{
            filters->removeIf([](QString x){
                return x == QString("*.img");
            });
        }
        if(!filters->length()){
            delete filters;
            filters = new QStringList({"*"});
        }
        model->setNameFilters(*filters);
        model->setNameFilterDisables(false);
    }

};

#endif // FILESANDBMAP_H
