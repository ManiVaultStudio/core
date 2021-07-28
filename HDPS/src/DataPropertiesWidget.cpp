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

            auto button = addButton(exposedAction->text());
            auto section = addWidget(button, exposedAction->createWidget(this));

            section->setBackground(0, QColor(240, 0, 240));

            button->addChild(section);

            button->setExpanded(exposedWidgetActionGroup->isExpanded());
        }
    }
    catch (std::exception& e)
    {
    }
}

QTreeWidgetItem* DataPropertiesWidget::addButton(const QString& title)
{
    auto treeWidgetItem = new QTreeWidgetItem();

    _treeWidget->addTopLevelItem(treeWidgetItem);
    _treeWidget->setItemWidget(treeWidgetItem, 0, new SectionExpandButton(treeWidgetItem, title));

    return treeWidgetItem;
}

QTreeWidgetItem* DataPropertiesWidget::addWidget(QTreeWidgetItem* buttonTreeWidgetItem, QWidget* widget)
{
    widget->setAutoFillBackground(true);
    widget->setMinimumHeight(10);

    auto section            = new QTreeWidgetItem(buttonTreeWidgetItem);
    auto containerWidget    = new QWidget();
    auto containerLayout    = new QVBoxLayout();

    containerLayout->setSizeConstraint(QLayout::SetMaximumSize);
    containerLayout->addWidget(widget);
    
    containerWidget->setLayout(containerLayout);
    containerWidget->setAutoFillBackground(true);

    //section->setDisabled(true);
    
    _treeWidget->setItemWidget(section, 0, containerWidget);

    return section;
}

SectionExpandButton::SectionExpandButton(QTreeWidgetItem* treeWidgetItem, const QString& text, QWidget* parent /*= nullptr*/) :
    QPushButton(text, parent),
    _treeWidgetItem(treeWidgetItem)
{
    connect(this, &QPushButton::clicked, this, [this]() {
        _treeWidgetItem->setExpanded(!_treeWidgetItem->isExpanded());
    });
}

}
}
