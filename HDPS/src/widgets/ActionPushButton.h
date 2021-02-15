#pragma once

#include "WidgetAction.h"

#include <QHBoxLayout>
#include <QPushButton>

class QWidget;

namespace hdps {

namespace gui {

class ActionPushButton : public WidgetAction::Widget {
public:
    ActionPushButton(QWidget* parent, QAction* action);

private:
    QHBoxLayout     _layout;
    QPushButton     _pushButton;
};

}
}