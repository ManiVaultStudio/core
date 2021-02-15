#pragma once

#include "WidgetAction.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>

class QWidget;

namespace hdps {

namespace gui {

class StandardAction : public WidgetAction
{
    Q_OBJECT

public:
    enum class WidgetType {
        PushButton,
        CheckBox
    };

public:
    class PushButton : public WidgetAction::Widget {
    public:
        PushButton(QWidget* parent, StandardAction* standardAction);

    private:
        QHBoxLayout     _layout;
        QPushButton     _pushButton;
    };

    class CheckBox : public WidgetAction::Widget {
    public:
        CheckBox(QWidget* parent, StandardAction* standardAction);

    private:
        QHBoxLayout     _layout;
        QCheckBox       _checkBox;
    };

public:
    StandardAction(QObject* parent, const QString& title = "");

    QWidget* createWidget(QWidget* parent, const WidgetType& widgetType = WidgetType::PushButton);
};

}
}