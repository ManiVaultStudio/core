#include "DataPropertiesWidget.h"
#include "Application.h"
#include "Core.h"

#include "actions/GroupAction.h"
#include "actions/ColorAction.h"
#include "actions/ColorMapAction.h"

#include <QDebug>
#include <QTreeWidgetItem>
#include <QColor>
#include <QVBoxLayout>
#include <QLabel>

namespace hdps
{

namespace gui
{

DataPropertiesWidget::DataPropertiesWidget(QWidget* parent) :
    QWidget(parent),
    _treeWidget(new QTreeWidget()),
    _dataset()
{
    setAutoFillBackground(true);

    auto layout = new QVBoxLayout();

    setLayout(layout);

    layout->setMargin(0);
    layout->setAlignment(Qt::AlignTop);

    _treeWidget->setHeaderHidden(true);
    _treeWidget->setIndentation(0);
    _treeWidget->setAutoFillBackground(true);
    _treeWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    layout->addWidget(_treeWidget);

    auto colorAction = new ColorAction(this, "Color");
    auto colorMapAction = new ColorMapAction(this, "Color map");

    auto actionLayout = new QHBoxLayout();

    actionLayout->addWidget(colorAction->createWidget(this));
    actionLayout->addWidget(colorMapAction->createWidget(this));

    layout->addLayout(actionLayout);

    connect(&_dataset, &DatasetRef<DataSet>::datasetNameChanged, this, [this](const QString& oldDatasetName, const QString& newDatasetName) {
        loadDataset();
    });

    emit datasetNameChanged("");
}

void DataPropertiesWidget::setDataset(const QString& datasetName)
{
    try
    {
        if (datasetName.isEmpty())
            throw std::runtime_error("data set name is empty");

        if (_dataset.isValid())
            disconnect(&_dataset->getHierarchyItem(), &DataHierarchyItem::actionAdded, this, nullptr);

        _dataset.setDatasetName(datasetName);

        if (!_dataset.isValid())
            return;

        connect(&_dataset->getHierarchyItem(), &DataHierarchyItem::actionAdded, this, [this]() {
            loadDataset();
        });
    }
    catch (std::exception& e)
    {
        qDebug() << QString("Cannot update data properties for %1: %2").arg(datasetName, e.what());
    }
}

void DataPropertiesWidget::loadDataset()
{
    emit datasetNameChanged(_dataset.getDatasetName());

    _treeWidget->clear();

    if (!_dataset.isValid())
        return;

    auto exposedActions = _dataset->getActions();

    for (auto exposedAction : exposedActions) {
        auto exposedGroupAction = dynamic_cast<GroupAction*>(exposedAction);

        if (exposedGroupAction == nullptr)
            continue;

        if (exposedGroupAction->isVisible())
            addButton(exposedGroupAction);
    }
}

QTreeWidgetItem* DataPropertiesWidget::addButton(GroupAction* groupAction)
{
    auto treeWidgetItem = new QTreeWidgetItem();

    _treeWidget->addTopLevelItem(treeWidgetItem);

    auto button = new SectionExpandButton(treeWidgetItem, groupAction, groupAction->text());

    _treeWidget->setItemWidget(treeWidgetItem, 0, button);
    
    return treeWidgetItem;
}

SectionExpandButton::SectionExpandButton(QTreeWidgetItem* treeWidgetItem, GroupAction* groupAction, const QString& text, QWidget* parent /*= nullptr*/) :
    QPushButton(text, parent),
    _widgetActionGroup(groupAction),
    _treeWidgetItem(treeWidgetItem)
{
    auto frameLayout    = new QHBoxLayout();
    auto iconLabel      = new QLabel();

    frameLayout->addWidget(iconLabel);

    setLayout(frameLayout);

    connect(this, &QPushButton::clicked, this, [this]() {
        _widgetActionGroup->toggle();
    });

    const auto update = [this, iconLabel]() -> void {
        if (_widgetActionGroup->isExpanded()) {
            _treeWidgetItem->setExpanded(true);

            auto groupWidget = _widgetActionGroup->createWidget(this);

            groupWidget->setAutoFillBackground(true);

            auto section = new QTreeWidgetItem(_treeWidgetItem);

            _treeWidgetItem->addChild(section);
            _treeWidgetItem->treeWidget()->setItemWidget(section, 0, groupWidget);
        }
        else {
            _treeWidgetItem->setExpanded(false);
            _treeWidgetItem->removeChild(_treeWidgetItem->child(0));
        }

        const auto iconName = _widgetActionGroup->isExpanded() ? "angle-down" : "angle-right";
        const auto icon     = Application::getIconFont("FontAwesome").getIcon(iconName);

        iconLabel->setPixmap(icon.pixmap(QSize(12, 12)));
    };

    connect(_widgetActionGroup, &GroupAction::expanded, this, [this, update]() {
        update();
    });

    connect(_widgetActionGroup, &GroupAction::collapsed, this, [this, update]() {
        update();
    });

    update();
}

}
}
