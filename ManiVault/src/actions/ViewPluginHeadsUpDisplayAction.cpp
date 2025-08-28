// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginHeadsUpDisplayAction.h"

#include <QVBoxLayout>

using namespace mv::util;

namespace mv::gui {

ViewPluginHeadsUpDisplayAction::HeadsUpDisplayWidget::HeadsUpDisplayWidget(QWidget* parent, ViewPluginHeadsUpDisplayAction* viewPluginHeadsUpDisplayAction, const std::int32_t& widgetFlags) :
    QWidget(parent),
    _viewPluginHeadsUpDisplayAction(viewPluginHeadsUpDisplayAction),
    _widgetFlags(widgetFlags)
{
    Q_ASSERT(_viewPluginHeadsUpDisplayAction);

    if (!_viewPluginHeadsUpDisplayAction)
        return;

    setStyleSheet("background-color: red;");

	auto layout = new QVBoxLayout();

    _contentLabel.setStyleSheet("color: black;");

    layout->addWidget(&_contentLabel);

    setLayout(layout);

    const auto updateLabel = [this]() -> void {
        qDebug() << "Updating HUD content label (before)" << _contentLabel.size();
        _contentLabel.setText(_viewPluginHeadsUpDisplayAction->getContent());
        _contentLabel.adjustSize();

        adjustSize();

        qDebug() << "Updating HUD content label (after)" << _contentLabel.size();
	};

    updateLabel();

    connect(_viewPluginHeadsUpDisplayAction, &ViewPluginHeadsUpDisplayAction::contentChanged, this, updateLabel);

    //setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
}

QSize ViewPluginHeadsUpDisplayAction::HeadsUpDisplayWidget::minimumSizeHint() const
{
	return _contentLabel.minimumSizeHint();
}

QWidget* ViewPluginHeadsUpDisplayAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    if (dynamic_cast<QMenu*>(parent))
        return QWidgetAction::createWidget(parent);

    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(new HeadsUpDisplayWidget(parent, this, widgetFlags));

    widget->setLayout(layout);

    return widget;
}

ViewPluginHeadsUpDisplayAction::ViewPluginHeadsUpDisplayAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title)
{
}

void ViewPluginHeadsUpDisplayAction::setContent(const QString& content)
{
    if (content == getContent())
        return;

    const auto previousContent = getContent();

    _content = content;

    emit contentChanged(previousContent, content);
}

QString ViewPluginHeadsUpDisplayAction::getContent() const
{
    return _content;
}

void ViewPluginHeadsUpDisplayAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);
}

QVariantMap ViewPluginHeadsUpDisplayAction::toVariantMap() const
{
	return WidgetAction::toVariantMap();
}

}
