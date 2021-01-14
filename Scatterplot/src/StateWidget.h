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

        _popupPushButton->setFixedSize(22, 22);

        QObject::connect(_popupPushButton, &QPushButton::clicked, [this]() {
            auto popupWidget = new QWidget(this);

            popupWidget->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));

            auto groupBox = new QGroupBox();
            auto groupBoxLayout = new QVBoxLayout();

            groupBoxLayout->addWidget(_widget);

            _widget->show();

            groupBox->setTitle(_widget->getTitle());
            groupBox->setLayout(groupBoxLayout);

            popupWidget->setLayout(new QVBoxLayout());

            popupWidget->layout()->setMargin(7);
            popupWidget->layout()->addWidget(groupBox);

            popupWidget->setWindowFlags(Qt::Popup);
            popupWidget->setObjectName("PopupWidget");
            popupWidget->setStyleSheet("QWidget#PopupWidget { border: 1px solid grey; }");
            popupWidget->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));

            popupWidget->move(mapToGlobal(_popupPushButton->rect().bottomLeft()) - popupWidget->rect().topLeft());
            popupWidget->show();
        });

        addWidget(_popupPushButton);
        addWidget(_widget);
    }

    void setListenWidget(QWidget* listenWidget) {
        _widgetEventProxy.initialize(listenWidget, [this](const QSize& sourceWidgetSize) {
            const auto state = _widget->getState(sourceWidgetSize);

            switch (state)
            {
                case WidgetStateMixin::State::Popup:
                    setCurrentIndex(0);
                    break;

                case WidgetStateMixin::State::Compact:
                case WidgetStateMixin::State::Full:
                    setCurrentIndex(1);
                    break;

                default:
                    break;
            }

            _widget->setState(state);
        });
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