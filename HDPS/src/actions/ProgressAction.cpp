// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProgressAction.h"

#include <QMenu>
#include <QHBoxLayout>

namespace hdps::gui {

ProgressAction::ProgressAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent, title)
{
    setDefaultWidgetFlags(WidgetFlag::Default);
    setConnectionPermissionsToForceNone(true);
}

ProgressAction::ProgressBarWidget::ProgressBarWidget(QWidget* parent, ProgressAction* progressAction, const std::int32_t& widgetFlags) :
    QProgressBar(parent),
    _progressAction(progressAction)
{
    if (widgetFlags & WidgetFlag::Horizontal)
        setOrientation(Qt::Horizontal);

    if (widgetFlags & WidgetFlag::Horizontal)
        setOrientation(Qt::Vertical);
}

QWidget* ProgressAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    if (dynamic_cast<QMenu*>(parent))
        return QWidgetAction::createWidget(parent);

    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(new ProgressAction::ProgressBarWidget(parent, this, widgetFlags));

    widget->setLayout(layout);

    return widget;
}

}
