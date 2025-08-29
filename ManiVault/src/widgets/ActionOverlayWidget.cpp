// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ActionOverlayWidget.h"

#include <QDebug>
#include <QMainWindow>

#ifdef _DEBUG
    #define ACTION_OVERLAY_WIDGET_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui
{

ActionOverlayWidget::ActionOverlayWidget(QWidget* target, const QPointer<WidgetAction>& action, const Qt::Alignment& alignment /*= Qt::AlignTop | Qt::AlignLeft*/) :
    OverlayWidget(target),
    _action(action),
    _actionWidget(action->createWidget(this))
{
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    _actionWidget->setMouseTracking(true);
    getWidgetOverlayer().getTargetWidget()->setMouseTracking(true);

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(alignment);

    //_actionWidget->setStyleSheet("background-color: red;");

    //addMouseEventReceiverWidget(_actionWidget);

    layout->addWidget(_actionWidget);

	setLayout(layout);

    installEventFilter(this);
    getWidgetOverlayer().getTargetWidget()->installEventFilter(this);
}

void ActionOverlayWidget::setTargetWidget(QWidget* targetWidget)
{
    Q_ASSERT(targetWidget);

    if (targetWidget == nullptr)
        return;

    setParent(targetWidget);

    getWidgetOverlayer().setTargetWidget(targetWidget);
}

bool ActionOverlayWidget::eventFilter(QObject* obj, QEvent* event)
{
    const auto transparentForMouseEvents = testAttribute(Qt::WA_TransparentForMouseEvents);

    if (event->type() == QEvent::MouseMove) {
        if (transparentForMouseEvents && obj == getWidgetOverlayer().getTargetWidget()) {
            qDebug() << "ActionOverlayWidget::eventFilter: Mouse move over target widget";
            if (_actionWidget->underMouse()) {
                qDebug() << "ActionOverlayWidget::eventFilter: Mouse entered action widget, disabling mouse-through";
                setAttribute(Qt::WA_TransparentForMouseEvents, false);
                return false;
            }
        }

        if (!transparentForMouseEvents && obj == _actionWidget) {
            qDebug() << "ActionOverlayWidget::eventFilter: Mouse move over action widget";
            if (!_actionWidget->underMouse()) {
                qDebug() << "ActionOverlayWidget::eventFilter: Mouse left action widget, enabling mouse-through";
                setAttribute(Qt::WA_TransparentForMouseEvents, true);
                return false;
            }
        }
    }
	return OverlayWidget::eventFilter(obj, event);
}

}
