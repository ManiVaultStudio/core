// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionHighlightWidget.h"

namespace mv::gui {

WidgetActionHighlightWidget::WidgetActionHighlightWidget(QWidget* parent, WidgetAction* action) :
    OverlayWidget(parent, 0.0f),
    _action(action),
    _widgetFader(this, this, 0.f, 0.f, 1.f, 120, 60)
{
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setStyleSheet(QString("background-color: %1;").arg(palette().highlight().color().name()));

    setAttribute(Qt::WA_TransparentForMouseEvents);

    auto layout = new QVBoxLayout();

    layout->addWidget(&_descriptionLabel);

    setLayout(layout);
}

WidgetAction* WidgetActionHighlightWidget::getAction()
{
    return _action;
}

void WidgetActionHighlightWidget::setAction(WidgetAction* action)
{
    if (_action != nullptr) {
        disconnect(_action, &WidgetAction::highlightVisibilityChanged, this, nullptr);
    	disconnect(_action, &WidgetAction::highlightModeChanged, this, nullptr);
    	disconnect(_action, &WidgetAction::highlightDescriptionChanged, this, nullptr);
    }

    _action = action;

    if (_action == nullptr)
        return;

    if (_action->isPublic())
        return;

    connect(_action, &WidgetAction::highlightVisibilityChanged, this, &WidgetActionHighlightWidget::highlightVisibilityChanged);
    connect(_action, &WidgetAction::highlightModeChanged, this, &WidgetActionHighlightWidget::highlightModeChanged);
    connect(_action, &WidgetAction::highlightDescriptionChanged, this, &WidgetActionHighlightWidget::highlightDescriptionChanged);

    highlightModeChanged(_action->getHighlightMode());
}

QString WidgetActionHighlightWidget::getDescription() const
{
    return _description;
}

void WidgetActionHighlightWidget::setHighlightDescription(const QString& description)
{
    if (description == _description)
        return;

    _description = description;
    
    _descriptionLabel.setText(_description);
}

void WidgetActionHighlightWidget::highlightVisibilityChanged(bool highlightVisible)
{
    updateHighlight();
}

void WidgetActionHighlightWidget::highlightModeChanged(const WidgetAction::HighlightMode& highlightMode)
{
    updateHighlight();
}

void WidgetActionHighlightWidget::highlightDescriptionChanged(const QString& highlightDescription)
{
    _descriptionLabel.setText(highlightDescription);
}

void WidgetActionHighlightWidget::updateHighlight()
{
    if (_action->isHighlightVisible()) {
        switch (_action->getHighlightMode())
        {
	        case WidgetAction::HighlightMode::Light:
	        {
	            _widgetFader.setOpacity(0.2f, 500);

	            break;
	        }

	        case WidgetAction::HighlightMode::Moderate:
	        {
	            _widgetFader.setOpacity(0.5f, 200);

	            break;
	        }

	        case WidgetAction::HighlightMode::Strong:
	        {
	            _widgetFader.setOpacity(0.9f, 200);

	            break;
	        }
        }
    }
    else {
        _widgetFader.setOpacity(0.0f, 500);
    }
}

}
