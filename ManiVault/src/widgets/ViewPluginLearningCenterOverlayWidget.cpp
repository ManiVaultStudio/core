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
    _layout(),
    _popupWidget(viewPlugin)
{
    _layout.setAlignment(_alignment);

    _layout.addWidget(&_popupWidget);

    //_layout.addLayout(&_verticalLayout);

    setLayout(&_layout);

    setContentsMargins(4);

    //setStyleSheet("background-color: red;");
}

void ViewPluginLearningCenterOverlayWidget::setTargetWidget(QWidget* targetWidget)
{
    getWidgetOverlayer().setTargetWidget(targetWidget);
}

void ViewPluginLearningCenterOverlayWidget::setContentsMargins(std::int32_t margin)
{
    _layout.setContentsMargins(margin, margin, margin, margin);
}

ViewPluginLearningCenterOverlayWidget::PopupWidget::PopupWidget(const plugin::ViewPlugin* viewPlugin, QWidget* parent) :
    QWidget(parent),
    _viewPlugin(viewPlugin),
    _label("Test", this)
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_label);

    _label.setPixmap(Application::getIconFont("FontAwesome").getIcon("info-circle").pixmap(QSize(16, 16)));

    setLayout(layout);

    //setStyleSheet("background-color: green;");
}

}
