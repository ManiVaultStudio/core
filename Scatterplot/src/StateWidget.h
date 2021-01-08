#pragma once

#include "WidgetStateMixin.h"

#include "util/WidgetResizeEventProxy.h"

#include <QStackedWidget>
#include <QPushButton>

template<typename WidgetType>
class StateWidget : public QStackedWidget
{
public:

    typedef std::function<WidgetStateMixin::State(const QSize& sourceWidgetSize)> StateFn;
    typedef std::function<void(WidgetType* widget)> WidgetInitializerFn;

public:

    StateWidget(QWidget* parent = nullptr) :
        QStackedWidget(parent),
        _widgetEventProxy(this),
        _stateFn(),
        _widgetInitializerFn(),
        _widgets(),
        _popupPushButton(new QPushButton())
    {
        _widgets << new WidgetType(WidgetStateMixin::State::Popup) << new WidgetType(WidgetStateMixin::State::Compact) << new WidgetType(WidgetStateMixin::State::Full);

        addWidget(_popupPushButton);
        addWidget(_widgets[1]);
        addWidget(_widgets[2]);
    }

    void initialize(QWidget* sourceWidget, StateFn stateFn, WidgetInitializerFn widgetInitializerFn) {
        _stateFn = stateFn;
        _widgetInitializerFn = widgetInitializerFn;

        _widgetEventProxy.initialize(sourceWidget, [this](const QSize& sourceWidgetSize) {
            if (!_stateFn)
                return;

            setCurrentIndex(static_cast<int>(_stateFn(sourceWidgetSize)));
        });

        for (auto widget : _widgets)
            _widgetInitializerFn(widget);


        /*
        _widget->initialize(plugin);

        QObject::connect(_popupPushButton, &QPushButton::clicked, [this, &plugin]() {
            auto popupWidget = new QWidget(this);

            auto groupBox = new QGroupBox();
            auto groupBoxLayout = new QVBoxLayout();

            auto stateWidget = new WidgetType(this);

            groupBoxLayout->addWidget(stateWidget);

            stateWidget->initialize(plugin);

            groupBox->setLayout(groupBoxLayout);

            popupWidget->setLayout(new QVBoxLayout());
            popupWidget->layout()->addWidget(groupBox);

            popupWidget->setWindowFlags(Qt::Popup);
            popupWidget->setObjectName("PopupWidget");
            popupWidget->setStyleSheet("QWidget#PopupWidget { border: 1px solid grey; }");
            popupWidget->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));

            popupWidget->move(mapToGlobal(_popupPushButton->rect().bottomLeft()) - popupWidget->rect().topLeft());
            popupWidget->show();
        });
        */
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
    StateFn                                 _stateFn;                   /** TODO */
    WidgetInitializerFn                     _widgetInitializerFn;       /** TODO */
    QList<WidgetType*>                      _widgets;                   /** TODO */
    QPushButton*                            _popupPushButton;           /** TODO */
};