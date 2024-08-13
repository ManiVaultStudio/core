// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginToolTipAction.h"

using namespace mv::util;

namespace mv::gui {

ViewPluginToolTipAction::ViewPluginToolTipAction(QObject* parent, const QString& title) :
    VerticalGroupAction(parent, title),
    _viewPlugin(nullptr),
    _roiSizeAction(this, "ROI size")
{
    addAction(&_roiSizeAction);

    //_toolTipLabel.setWindowFlag(Qt::Popup);
    //_toolTipLabel.setWindowFlag(Qt::WindowStaysOnTopHint);
}

void ViewPluginToolTipAction::initialize(plugin::ViewPlugin* viewPlugin, const TooltipGeneratorFunction& tooltipGeneratorFunction)
{
    Q_ASSERT(viewPlugin);

    if (!viewPlugin)
        return;

    if (_viewPlugin)
        _viewPlugin->getWidget().removeEventFilter(this);

    _viewPlugin                 = viewPlugin;
    _tooltipGeneratorFunction   = tooltipGeneratorFunction;
    _toolTipOverlayWidget       = std::make_unique<OverlayWidget>(&_viewPlugin->getWidget());

    _viewPlugin->getWidget().setMouseTracking(true);
    _viewPlugin->getWidget().installEventFilter(this);

    _toolTipLabel.setParent(_toolTipOverlayWidget.get());
    _toolTipLabel.raise();
    _toolTipLabel.setWindowFlag(Qt::WindowStaysOnTopHint);
}

void ViewPluginToolTipAction::requestUpdate()
{
    if (!_tooltipGeneratorFunction)
        return;

    setToolTipHtmlString(_tooltipGeneratorFunction(_viewPlugin));
}

QString ViewPluginToolTipAction::getToolTipHtmlString() const
{
    return _toolTipHtmlString;
}

void ViewPluginToolTipAction::setToolTipHtmlString(const QString& toolTipHtmlString)
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

void ViewPluginToolTipAction::drawToolTip()
{
    //if (!isChecked())
    //    return;

    Q_ASSERT(_viewPlugin);

    if (!_viewPlugin)
        return;

    moveToolTipLabel();
}

void ViewPluginToolTipAction::moveToolTipLabel()
{
    _toolTipLabel.move(_viewPlugin->getWidget().mapFromGlobal(_viewPlugin->getWidget().cursor().pos()));
}

bool ViewPluginToolTipAction::eventFilter(QObject* target, QEvent* event)
{
    Q_ASSERT(_viewPlugin);

    if (!_viewPlugin)
        return VerticalGroupAction::eventFilter(target, event);

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

    return VerticalGroupAction::eventFilter(target, event);
}
}
