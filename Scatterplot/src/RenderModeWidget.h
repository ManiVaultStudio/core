#pragma once

#include "ScatterplotWidget.h"

#include "util/WidgetResizeEventProxy.h"
#include "widgets/PopupPushButton.h"

#include <QWidget>
#include <QIcon>
#include <QGroupBox>

class ScatterplotPlugin;

namespace Ui {
    class RenderModeWidget;
}

class QVBoxLayout;
class QPushButton;

template<typename WidgetType>
class StateWidget : public QWidget
{
public:

    enum class State {
        Popup,
        Compact,
        Full
    };

public:
    StateWidget(QWidget* parent = nullptr) :
        QWidget(parent),
        _widgetEventProxy(this),
        _layout(new QHBoxLayout()),
        _popupPushButton(new QPushButton()),
        _widget(new WidgetType(this))
    {
        setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred));
        setLayout(_layout);

        _layout->addWidget(_popupPushButton);
        _layout->addWidget(_widget);

        _popupPushButton->setFixedSize(QSize(22, 22));
    }

    void initialize(const ScatterplotPlugin& plugin) {
        
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
    }

protected:
    hdps::util::WidgetResizeEventProxy  _widgetEventProxy;  /** TODO */
    QHBoxLayout*                        _layout;            /** TODO */
    QPushButton*                        _popupPushButton;   /** TODO */
    WidgetType*                         _widget;            /** TODO */
};

class RenderModeWidget : public QWidget
{
public:
    RenderModeWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);
    
private:

    QIcon getIcon(const ScatterplotWidget::RenderMode& renderMode) const;

private:
    std::unique_ptr<Ui::RenderModeWidget>   _ui;                /** Externally loaded UI */
    QSize                                   _iconSize;          /** Icon size  */

    static const QSize BUTTON_SIZE_COMPACT;
    static const QSize BUTTON_SIZE_FULL;
};