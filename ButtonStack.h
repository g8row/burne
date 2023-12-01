#ifndef BUTTONSTACK_H
#define BUTTONSTACK_H

#include <QPushButton>
#include <QStackedLayout>

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

    ~ButtonStack(){
        delete contextButtonsFlash;
        delete contextButtonsBack;
        delete layout;
        delete flashButton;
        delete backButton;
    }


};

#endif // BUTTONSTACK_H
