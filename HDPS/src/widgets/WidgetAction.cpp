#include "WidgetAction.h"

#include <QMenu>
#include <QToolBar>
#include <QEvent>

namespace hdps {

namespace gui {

WidgetAction::PopupWidget::PopupWidget(QWidget* parent, const QString& title) :
    QWidget(parent),
    _mainLayout(),
    _groupBox(title)
{
    _mainLayout.setMargin(4);
    _mainLayout.addWidget(&_groupBox);

    setLayout(&_mainLayout);
}

void WidgetAction::PopupWidget::setContentLayout(QLayout* layout)
{
    _groupBox.setLayout(layout);
}

WidgetAction::Widget::Widget(QWidget* parent, QAction* action) :
    QWidget(parent),
    _action(action)
{
    const auto updateAction = [this, action]() -> void {
        setEnabled(action->isEnabled());
        setVisible(action->isVisible());
    };

    connect(action, &QAction::changed, this, [this, updateAction]() {
        updateAction();
    });

    updateAction();
}

void WidgetAction::Widget::setLayout(QLayout* layout)
{
    layout->setMargin(0);
    layout->setSpacing(4);

    QWidget::setLayout(layout);
}

bool WidgetAction::Widget::isChildOfMenu() const
{
    return dynamic_cast<QMenu*>(parent());
}

bool WidgetAction::Widget::childOfToolbar() const
{
    return dynamic_cast<QToolBar*>(parent());
}

bool WidgetAction::Widget::childOfWidget() const
{
    return isChildOfMenu() || childOfToolbar();
}

WidgetAction::WidgetAction(QObject* parent) :
    QWidgetAction(parent)
{
}

}
}