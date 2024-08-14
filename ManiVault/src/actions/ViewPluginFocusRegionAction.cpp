// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginFocusRegionAction.h"

using namespace mv::util;

namespace mv::gui {

ViewPluginFocusRegionAction::ViewPluginFocusRegionAction(QObject* parent, const QString& title) :
    HorizontalGroupAction(parent, title),
    _viewPlugin(nullptr),
    _enabledAction(this, "Enabled"),
    _regionSizeAction(this, "Region size", 0.f, 1000.f, 100.f, 1),
    _toolTipDirty(true)
{
    addAction(&_enabledAction);
    addAction(&_regionSizeAction);
}

void ViewPluginFocusRegionAction::initialize(plugin::ViewPlugin* viewPlugin, const SummaryGeneratorFunction& summaryGeneratorFunction)
{
    Q_ASSERT(viewPlugin);

    if (!viewPlugin)
        return;

    if (_viewPlugin)
        _viewPlugin->getWidget().removeEventFilter(this);

    _viewPlugin                 = viewPlugin;
    _summaryGeneratorFunction   = summaryGeneratorFunction;
    _toolTipOverlayWidget       = std::make_unique<OverlayWidget>(&_viewPlugin->getWidget());

    _viewPlugin->getWidget().setMouseTracking(true);
    _viewPlugin->getWidget().installEventFilter(this);

    _toolTipLabel.setParent(_toolTipOverlayWidget.get());
    _toolTipLabel.raise();
    _toolTipLabel.setWindowFlag(Qt::WindowStaysOnTopHint);

    _updateTimer.setInterval(250);
    
    connect(&_updateTimer, &QTimer::timeout, this, [this]() -> void {
        if (!_toolTipDirty || !_summaryGeneratorFunction)
            return;

        setToolTipHtmlString(_summaryGeneratorFunction(_toolTipContext));

        _toolTipDirty = false;
    });

    _updateTimer.start();
}

void ViewPluginFocusRegionAction::requestUpdate(const QVariantMap& toolTipContext)
{
    if (toolTipContext == _toolTipContext)
        return;

    _toolTipContext = toolTipContext;
    _toolTipDirty   = true;
}

QVariantMap ViewPluginFocusRegionAction::getToolTipContext() const
{
    return _toolTipContext;
}

QString ViewPluginFocusRegionAction::getToolTipHtmlString() const
{
    return _toolTipHtmlString;
}

void ViewPluginFocusRegionAction::setToolTipHtmlString(const QString& toolTipHtmlString)
{
    if (toolTipHtmlString == _toolTipHtmlString)
        return;

    const auto previousToolTipHtmlString = _toolTipHtmlString;

    _toolTipHtmlString = toolTipHtmlString;

    _toolTipLabel.setVisible(!_toolTipHtmlString.isEmpty());
    _toolTipLabel.setText(_toolTipHtmlString);
    _toolTipLabel.setMinimumSize(QSize(200, 100));

    drawToolTip();

    emit toolTipHtmlStringChanged(previousToolTipHtmlString, _toolTipHtmlString);
}

void ViewPluginFocusRegionAction::drawToolTip()
{
    //if (!isChecked())
    //    return;

    Q_ASSERT(_viewPlugin);

    if (!_viewPlugin)
        return;

    moveToolTipLabel();
}

void ViewPluginFocusRegionAction::moveToolTipLabel()
{
    _toolTipLabel.move(_viewPlugin->getWidget().mapFromGlobal(_viewPlugin->getWidget().cursor().pos()));
}

bool ViewPluginFocusRegionAction::eventFilter(QObject* target, QEvent* event)
{
    Q_ASSERT(_viewPlugin);

    if (!_viewPlugin)
        return HorizontalGroupAction::eventFilter(target, event);

    if (target == &_viewPlugin->getWidget()) {
        switch (event->type())
        {
            case QEvent::MouseMove:
                moveToolTipLabel();
                break;

        default:
            break;
        }
    }

    return HorizontalGroupAction::eventFilter(target, event);
}
}
