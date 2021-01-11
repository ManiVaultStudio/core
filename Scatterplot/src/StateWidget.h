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
        setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred));

        _popupPushButton->setFixedSize(22, 22);

        QObject::connect(_popupPushButton, &QPushButton::clicked, [this]() {
            auto popupWidget = new QWidget(this);

            popupWidget->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));

            auto groupBox = new QGroupBox();
            auto groupBoxLayout = new QVBoxLayout();

            groupBoxLayout->addWidget(_widgets.first());

            groupBox->setTitle(_widgets.first()->getTitle());
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