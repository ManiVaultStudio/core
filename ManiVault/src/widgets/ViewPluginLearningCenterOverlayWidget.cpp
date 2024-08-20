// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginLearningCenterOverlayWidget.h"

#include <QDebug>

#ifdef _DEBUG
    #define VIEW_PLUGIN_LEARNING_CENTER_OVERLAY_WIDGET_VERBOSE
#endif

namespace mv::gui
{
    
ViewPluginLearningCenterOverlayWidget::ViewPluginLearningCenterOverlayWidget(QWidget* source, const plugin::ViewPlugin* viewPlugin, const Qt::Alignment& alignment /*= Qt::AlignBottom | Qt::AlignRight*/) :
    OverlayWidget(source),
    _viewPlugin(viewPlugin),
    _alignment(alignment),
    _widgetOverlayer(this, this, source),
    _widgetFader(this, this, 1.f, 0.f, 1.f, 120, 60),
    _horizontalLayout(),
    _verticalLayout()
{
    _horizontalLayout.setAlignment(_alignment);

    _verticalLayout.addWidget(&_popupWidget);

    _horizontalLayout.addLayout(&_verticalLayout);

    setLayout(&_horizontalLayout);
}

mv::util::WidgetFader& ViewPluginLearningCenterOverlayWidget::getWidgetFader()
{
    return _widgetFader;
}

ViewPluginLearningCenterOverlayWidget::PopupWidget::PopupWidget(QWidget* parent) :
    QWidget(parent),
    _label("Test", this)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(&_label);

    setLayout(layout);
}

}
