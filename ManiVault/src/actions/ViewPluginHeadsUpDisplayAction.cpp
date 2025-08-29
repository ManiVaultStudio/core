// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginHeadsUpDisplayAction.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QProxyStyle>

#include "models/AbstractHeadsUpDisplayModel.h"

using namespace mv::util;

namespace mv::gui {

    class BranchIconStyle : public QProxyStyle {
    public:
        BranchIconStyle(QStyle* base = nullptr)
            : QProxyStyle(base) {
        }

        void drawPrimitive(PrimitiveElement elem,
            const QStyleOption* opt,
            QPainter* p,
            const QWidget* w) const override
        {
            if (elem == PE_IndicatorBranch) {
                const auto* vopt = qstyleoption_cast<const QStyleOptionViewItem*>(opt);
                if (!vopt) {
                    QProxyStyle::drawPrimitive(elem, opt, p, w);
                    return;
                }

                const QRect r = opt->rect;
                const bool expanded = vopt->state & State_Open;
                const bool hasChildren = vopt->state & State_Children;
                if (hasChildren) {
                    const QIcon& icon = expanded ? StyledIcon("caret-down").withColor("black") : StyledIcon("caret-right").withColor("black");
                    icon.paint(p, r, Qt::AlignCenter, QIcon::Normal);
                    return;
                }
            }
            QProxyStyle::drawPrimitive(elem, opt, p, w);
        }

    private:
        QIcon closed_;
        QIcon open_;
    };

ViewPluginHeadsUpDisplayAction::HeadsUpDisplayWidget::HeadsUpDisplayWidget(QWidget* parent, ViewPluginHeadsUpDisplayAction* viewPluginHeadsUpDisplayAction, const std::int32_t& widgetFlags) :
    QWidget(parent),
    _viewPluginHeadsUpDisplayAction(viewPluginHeadsUpDisplayAction),
    _widgetFlags(widgetFlags)
{
    Q_ASSERT(_viewPluginHeadsUpDisplayAction);

    if (!_viewPluginHeadsUpDisplayAction)
        return;

    //setAttribute(Qt::WA_TranslucentBackground, true);
    //setAttribute(Qt::WA_NoSystemBackground, true);
    //setAttribute(Qt::WA_TransparentForMouseEvents, true); // mouse-through by default

    _treeView.setModel(&_viewPluginHeadsUpDisplayAction->getHeadsUpDisplayTreeModel());
    _treeView.setFixedWidth(300);
    _treeView.setHeaderHidden(true);
    _treeView.setRootIsDecorated(true);
    _treeView.setFrameShape(QFrame::NoFrame);
    _treeView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _treeView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _treeView.setIconSize(QSize(12, 12));
    _treeView.setStyle(new BranchIconStyle(_treeView.style()));
    _treeView.setMinimumSize(100, 100);
    _treeView.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    _treeView.adjustSize();

    QPalette palette = _treeView.palette();

	palette.setColor(QPalette::Text, Qt::black);

	_treeView.setPalette(palette);

    if (auto viewport = _treeView.viewport()) {
        viewport->setAttribute(Qt::WA_TranslucentBackground, true);
        viewport->setStyleSheet("background: transparent;");
    }

    if (auto header = _treeView.header()) {
        header->hideSection(static_cast<int>(AbstractHeadsUpDisplayModel::Column::Id));
        header->hideSection(static_cast<int>(AbstractHeadsUpDisplayModel::Column::Description));
    }

	auto layout = new QVBoxLayout();

    layout->addWidget(&_treeView);

    setLayout(layout);
}

void ViewPluginHeadsUpDisplayAction::HeadsUpDisplayWidget::mousePressEvent(QMouseEvent* event)
{
    QPoint treePos = _treeView.mapFromParent(event->pos());
    QModelIndex idx = _treeView.indexAt(treePos);

    if (idx.isValid()) {
        // Let QTreeView handle the event
        QWidget::mousePressEvent(event);
    }
    else {
        // Forward event to underlying widget
        auto underlying = parentWidget(); // or get the correct widget
        if (underlying) {
            QMouseEvent forwarded(event->type(), underlying->mapFromGlobal(mapToGlobal(event->pos())),
                event->button(), event->buttons(), event->modifiers());
            QCoreApplication::sendEvent(underlying, &forwarded);
        }
    }
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
