// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginHeadsUpDisplayAction.h"

#include <QVBoxLayout>
#include <QHeaderView>

#include "models/AbstractHeadsUpDisplayModel.h"

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

    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TransparentForMouseEvents, true); // mouse-through by default

    _treeView.setModel(&_viewPluginHeadsUpDisplayAction->getHeadsUpDisplayTreeModel());
    _treeView.setFixedWidth(300);
    //_treeView.setHeaderHidden(true);
    //_treeView.setRootIsDecorated(false);
    _treeView.setFrameShape(QFrame::NoFrame);
    _treeView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _treeView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _treeView.setStyleSheet(
        "QTreeView { background: transparent; }"
        "QTreeView::item { color: black; padding: 2px 4px; }"
    );
    _treeView.viewport()->setAttribute(Qt::WA_TranslucentBackground, true);
    _treeView.viewport()->setStyleSheet("background: transparent;");

    _treeView.header()->hideSection(static_cast<int>(AbstractHeadsUpDisplayModel::Column::Id));
    _treeView.header()->hideSection(static_cast<int>(AbstractHeadsUpDisplayModel::Column::Description));

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
    //setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
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

HeadsUpDisplayItemSharedPtr ViewPluginHeadsUpDisplayAction::addHeadsUpDisplayItem(const QString& title, const QString& value, const QString& description, const util::HeadsUpDisplayItemSharedPtr& sharedParent /*= nullptr*/)
{
    auto sharedHeadsUpDisplayItem = std::make_shared<HeadsUpDisplayItem>(title, value, description, sharedParent);

    _headsUpDisplayTreeModel.addHeadsUpDisplayItem(sharedHeadsUpDisplayItem);

    return sharedHeadsUpDisplayItem;
}

void ViewPluginHeadsUpDisplayAction::removeHeadsUpDisplayItem(const HeadsUpDisplayItemSharedPtr& headsUpDisplayItem)
{
    _headsUpDisplayTreeModel.removeHeadsUpDisplayItem(headsUpDisplayItem);
}

void ViewPluginHeadsUpDisplayAction::removeAllHeadsUpDisplayItems()
{
    _headsUpDisplayTreeModel.setRowCount(0);
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
