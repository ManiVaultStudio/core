#include "DataPropertiesWidget.h"
#include "Core.h"
#include "actions/WidgetActionGroup.h"

#include <QDebug>
#include <QTreeWidgetItem>
#include <QColor>

namespace hdps
{

namespace gui
{

DataPropertiesWidget::DataPropertiesWidget(QWidget* parent, Core* core) :
    QWidget(parent),
    _core(core),
    _treeWidget(new QTreeWidget()),
    _dataWidget(nullptr)
{
    setAutoFillBackground(true);

    auto layout = new QVBoxLayout();

    setLayout(layout);

    layout->setMargin(0);
    layout->setAlignment(Qt::AlignTop);

    _treeWidget->setHeaderHidden(true);
    _treeWidget->setIndentation(0);
    _treeWidget->setAutoFillBackground(true);

    //itemExpanded(QTreeWidgetItem *item)

    layout->addWidget(_treeWidget);
}

void DataPropertiesWidget::setDataset(const QString& datasetName)
{
    try
    {
        if (datasetName.isEmpty())
            return;

        if (_dataWidget != nullptr)
            delete _dataWidget;

        auto& dataset = _core->requestData(datasetName);

        _treeWidget->clear();

        auto exposedActions = dataset.getExposedActions();

        for (auto exposedAction : exposedActions) {
            auto exposedWidgetActionGroup = dynamic_cast<WidgetActionGroup*>(exposedAction);

            if (exposedWidgetActionGroup == nullptr)
                continue;

            addButton(exposedAction->text(), exposedWidgetActionGroup);
        }
    }
    catch (std::exception& e)
    {
    }
}

QTreeWidgetItem* DataPropertiesWidget::addButton(const QString& title, WidgetActionGroup* widgetActionGroup)
{
    auto treeWidgetItem = new QTreeWidgetItem();

    _treeWidget->addTopLevelItem(treeWidgetItem);
    _treeWidget->setItemWidget(treeWidgetItem, 0, new SectionExpandButton(treeWidgetItem, widgetActionGroup, title));
    
    treeWidgetItem->setExpanded(widgetActionGroup->isExpanded());

    return treeWidgetItem;
}

SectionExpandButton::SectionExpandButton(QTreeWidgetItem* treeWidgetItem, WidgetActionGroup* widgetActionGroup, const QString& text, QWidget* parent /*= nullptr*/) :
    QPushButton(text, parent),
    _widgetActionGroup(widgetActionGroup),
    _treeWidgetItem(treeWidgetItem)
{
    connect(_treeWidgetItem->treeWidget(), &QTreeWidget::itemExpanded, this, [this](QTreeWidgetItem* treeWidgetItem) {
        if (treeWidgetItem != _treeWidgetItem)
            return;

        auto section = new QTreeWidgetItem(_treeWidgetItem);

        _treeWidgetItem->addChild(section);
        _treeWidgetItem->treeWidget()->setItemWidget(section, 0, _widgetActionGroup->createWidget(this));
    });

    connect(_treeWidgetItem->treeWidget(), &QTreeWidget::itemCollapsed, this, [this](QTreeWidgetItem* treeWidgetItem) {
        if (treeWidgetItem != _treeWidgetItem)
            return;

        _treeWidgetItem->removeChild(_treeWidgetItem->child(0));
    });

    connect(this, &QPushButton::clicked, this, [this]() {
        if (_treeWidgetItem->isExpanded())
            _treeWidgetItem->treeWidget()->collapseItem(_treeWidgetItem);
        else
            _treeWidgetItem->treeWidget()->expandItem(_treeWidgetItem);
    });
}

}
}
