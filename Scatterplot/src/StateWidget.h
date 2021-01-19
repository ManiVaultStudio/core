#pragma once

#include "WidgetStateMixin.h"

#include "util/WidgetResizeEventProxy.h"

#include <QStackedWidget>
#include <QPushButton>
#include <QGroupBox>

template<typename WidgetType>
class StateWidget : public QStackedWidget
{
public:
    StateWidget(QWidget* parent = nullptr) :
        QStackedWidget(parent),
        _widgetEventProxy(this),
        _widget(new WidgetType(this)),
        _popupPushButton(new QPushButton())
    {
        setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred));

        _widget->hide();

        _popupPushButton->setFixedHeight(22);
        _popupPushButton->setText(_widget->getTitle());

        QObject::connect(_popupPushButton, &QPushButton::clicked, [this]() {
            auto popupWidget    = new QWidget(this);
            auto popupLayout    = new QVBoxLayout();
            auto groupBox       = new QGroupBox();
            auto groupBoxLayout = new QVBoxLayout();

            popupWidget->setWindowFlags(Qt::Popup);
            popupWidget->setObjectName("PopupWidget");
            popupWidget->setStyleSheet("QWidget#PopupWidget { border: 1px solid grey; }");
            popupWidget->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
            popupWidget->setLayout(popupLayout);
            
            popupLayout->setMargin(7);
            popupLayout->addWidget(groupBox);

            groupBox->setTitle(_widget->getTitle());
            groupBox->setLayout(groupBoxLayout);

            groupBoxLayout->addWidget(_widget);

            _widget->show();
            
            popupWidget->move(mapToGlobal(_popupPushButton->rect().bottomLeft()) - popupWidget->rect().topLeft());
            popupWidget->show();
        });

        addWidget(_popupPushButton);
        addWidget(_widget);
    }

    void setListenWidget(QWidget* listenWidget) {
        /*
        _widgetEventProxy.initialize(listenWidget, [this](const QSize& sourceWidgetSize) {
            const auto state = WidgetStateMixin::State::Compact;

            //_widget->hide();
            _widget->setState(state);
            
            switch (state)
            {
                
                case WidgetStateMixin::State::Popup:
                {
                    removeWidget(_widget);
                    setCurrentIndex(0);
                    break;
                }

                case WidgetStateMixin::State::Compact:
                case WidgetStateMixin::State::Full:
                {
                    addWidget(_widget);
                    setCurrentIndex(1);
                    break;
                }

                default:
                    break;
            }
        });
        */
    }

    WidgetType* getWidget() {
        return _widget;
    }

public:
    
    QSize sizeHint() const override {
        return currentWidget()->sizeHint();
    }

    QSize minimumSizeHint() const override {
        return currentWidget()->minimumSizeHint();
    }

protected:
    hdps::util::WidgetResizeEventProxy      _widgetEventProxy;          /** TODO */
    WidgetType*                             _widget;                    /** TODO */
    QPushButton*                            _popupPushButton;           /** TODO */
};