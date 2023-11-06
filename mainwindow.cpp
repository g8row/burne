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

namespace fs = std::filesystem;

class ButtonStack : public QWidget{
    QWidget* contextButtonsPdf;
    QWidget* contextButtonsImg;
    QStackedLayout* layout;
public:
    ButtonStack(): QWidget() {
        layout = new QStackedLayout();
        QWidget::setLayout(layout);

        contextButtonsPdf = new QWidget();
        contextButtonsPdf->setLayout(new QHBoxLayout());

        QPushButton* openButton = new QPushButton("Open");
        openButton->setFixedHeight(60);
        openButton->setStyleSheet("font-size: 20px;");

        contextButtonsPdf->layout()->addWidget(openButton);
        QPushButton* printButton = new QPushButton("Print");
        printButton->setFixedHeight(60);
        printButton->setStyleSheet("font-size: 20px;");
        contextButtonsPdf->layout()->addWidget(printButton);

        contextButtonsImg = new QWidget();
        contextButtonsImg->setLayout(new QHBoxLayout());

        QPushButton* flashButton = new QPushButton("Flash");
        flashButton->setFixedHeight(60);
        flashButton->setStyleSheet("font-size: 20px;");
        contextButtonsImg->layout()->addWidget(flashButton);

        layout->addWidget(contextButtonsImg);
        layout->addWidget(contextButtonsPdf);
    }

public slots:
    void displayPdf(){
        layout->setCurrentWidget(contextButtonsPdf);
    }
    void displayImg(){
        layout->setCurrentWidget(contextButtonsImg);
    }
};


bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

QVBoxLayout* filteredLayout(std::string filter, QVector<QPushButton*> buttons){
    QVBoxLayout* layout = new QVBoxLayout();
    for(QPushButton* button : buttons){
        if(hasEnding(((QLabel*)(button->children().at(1)))->text().toStdString(),filter)){
            layout->addWidget(button);
            button->setVisible(true);
        }
    }
    return layout;
}

class ButtonList : public QScrollArea {
    QWidget* blist = nullptr;
    QVector<QPushButton*> buttons;
public:
    ButtonList(QVector<QPushButton*> buttonsVec): QScrollArea(), buttons(buttonsVec){
        setAlignment(Qt::AlignTop);
        QScroller::grabGesture(this, QScroller::LeftMouseButtonGesture);

        blist = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout();
        for(QPushButton* button : buttons){
            layout->addWidget(button);
        }
        blist->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        blist->setLayout(layout);
        setWidgetResizable(true);
        setWidget(blist);
    }

public slots:
    void setButtonsPdf(){
        for(QPushButton* button : buttons){
            blist->layout()->removeWidget(button);
            button->setVisible(false);
        }
        delete blist->layout();
        QVBoxLayout* layout = filteredLayout("pdf",buttons);
        blist->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        blist->setLayout(layout);
    }
    void setButtonsClear(){
        for(QPushButton* button : buttons){
            blist->layout()->removeWidget(button);
            button->setVisible(false);
        }
        delete blist->layout();
        QVBoxLayout* layout = filteredLayout("",buttons);
        blist->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        blist->setLayout(layout);
    }
    void setButtonsImg(){
        for(QPushButton* button : buttons){
            blist->layout()->removeWidget(button);
            button->setVisible(false);
        }
        delete blist->layout();
        QVBoxLayout* layout = filteredLayout("img",buttons);
        blist->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        blist->setLayout(layout);
    }
};

bool lessThan(QPushButton* b1, QPushButton* b2){
    return ((QLabel*)(b1->children().at(1)))->text().compare(((QLabel*)(b2->children().at(1)))->text())<0;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(800,480);
    std::string path = getenv("HOME");
    path.append("/Downloads");
    struct stat sb;
    QVector<QPushButton*> buttonsVec;

    ui->centralwidget->setLayout(new QVBoxLayout());

    ButtonStack* bStack =  new ButtonStack();


    for (const auto& entry : fs::directory_iterator(path)) {

        // Converting the path to const char * in the
        // subsequent lines
        std::filesystem::path outfilename = entry.path();
        std::string outfilename_str = outfilename.string();
        const char* path = outfilename_str.c_str();

        // Testing whether the path points to a
        // non-directory or not If it does, displays path
        if (stat(path, &sb) == 0 && !(sb.st_mode & S_IFDIR)){
            QPushButton* button = new QPushButton();
            if(hasEnding(outfilename_str,".pdf")){
                QObject::connect(button,&QPushButton::clicked,bStack,&ButtonStack::displayPdf);
            }
            if(hasEnding(outfilename_str,".img")){
                QObject::connect(button,&QPushButton::clicked,bStack,&ButtonStack::displayImg);
            }
            auto label = new QLabel(outfilename.filename().c_str());
            label->setWordWrap(true);
            label->setStyleSheet("font-size: 20px;");
            auto layout = new QHBoxLayout(button);
            layout->addWidget(label,0,Qt::AlignCenter);

            button->setMinimumHeight(95);
            button->setFixedWidth(740);
            buttonsVec.append(button);
        }
    }

    std::sort(buttonsVec.begin(),buttonsVec.end(),lessThan);

    ButtonList* buttons = new ButtonList(buttonsVec);

    QWidget* filters = new QWidget();
    filters->setLayout(new QHBoxLayout());

    QPushButton* pdfFilter = new QPushButton("Pdf");
    pdfFilter->setFixedHeight(60);
    pdfFilter->setStyleSheet("font-size: 20px;");
    connect(pdfFilter,&QPushButton::clicked,buttons,&ButtonList::setButtonsPdf);
    filters->layout()->addWidget(pdfFilter);

    QPushButton* imgFilter = new QPushButton("Img");
    imgFilter->setFixedHeight(60);
    imgFilter->setStyleSheet("font-size: 20px;");
    connect(imgFilter,&QPushButton::clicked,buttons,&ButtonList::setButtonsImg);
    filters->layout()->addWidget(imgFilter);

    QPushButton* clearFilter = new QPushButton("Clear");
    clearFilter->setFixedHeight(60);
    clearFilter->setStyleSheet("font-size: 20px;");
    connect(clearFilter,&QPushButton::clicked,buttons,&ButtonList::setButtonsClear);
    filters->layout()->addWidget(clearFilter);

    ui->centralwidget->layout()->addWidget(filters);
    ui->centralwidget->layout()->addWidget(buttons);
    ui->centralwidget->layout()->addItem(new QSpacerItem(0,0));
    ui->centralwidget->layout()->addWidget(bStack);

}

MainWindow::~MainWindow()
{
    delete ui;
}

