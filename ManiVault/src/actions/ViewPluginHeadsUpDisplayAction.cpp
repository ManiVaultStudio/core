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

    _treeView.setModel(&_viewPluginHeadsUpDisplayAction->getHeadsUpDisplayTreeModel());

	auto layout = new QVBoxLayout();

    layout->addWidget(&_treeView);

 //   _contentLabel.setStyleSheet("color: black;");
 //   //_contentLabel.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
 //   _contentLabel.setWordWrap(true);
 //   _contentLabel.setStyleSheet("background-color: green;");
 //   //layout->setSizeConstraint(QLayout::SetMaximumSize);

 //   layout->addWidget(&_contentLabel);

 //   setLayout(layout);

 //   const auto updateLabel = [this, layout]() -> void {
 //       qDebug() << "Updating HUD content label (before)" << size() << _contentLabel.size();
 //       _contentLabel.setText(_viewPluginHeadsUpDisplayAction->getContent());

 //       QTimer::singleShot(10, [this]() -> void {
 //           _contentLabel.updateGeometry();
 //           _contentLabel.adjustSize();

 //           adjustSize();
 //       });
 //       

 //       //layout->activate();
 //       //adjustSize();

 //       qDebug() << "Updating HUD content label (after)" << size() << _contentLabel.size();
	//};

 //   updateLabel();

 //   connect(_viewPluginHeadsUpDisplayAction, &ViewPluginHeadsUpDisplayAction::contentChanged, this, updateLabel);

    setLayout(layout);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

//QSize ViewPluginHeadsUpDisplayAction::HeadsUpDisplayWidget::minimumSizeHint() const
//{
//	return _contentLabel.sizeHint();
//}

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

HeadsUpDisplayTreeModel& ViewPluginHeadsUpDisplayAction::getHeadsUpDisplayTreeModel()
{
    return _headsUpDisplayTreeModel;
}

void ViewPluginHeadsUpDisplayAction::addHeadsUpDisplayItem(const HeadsUpDisplayItemSharedPtr& headsUpDisplayItem)
{
    _headsUpDisplayTreeModel.addHeadsUpDisplayItem(headsUpDisplayItem);
}

void ViewPluginHeadsUpDisplayAction::removeHeadsUpDisplayItem(const HeadsUpDisplayItemSharedPtr& headsUpDisplayItem)
{
    _headsUpDisplayTreeModel.removeHeadsUpDisplayItem(headsUpDisplayItem);
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
