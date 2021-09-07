#include "DataPropertiesWidget.h"
#include "Application.h"
#include "Core.h"

#include "actions/GroupAction.h"

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
    _dataWidget(nullptr),
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
}

void DataPropertiesWidget::setDataset(const QString& datasetName)
{
    try
    {
        if (datasetName.isEmpty())
            throw std::runtime_error("data set name is empty");

        if (_dataWidget != nullptr)
            delete _dataWidget;

        if (_dataset.isValid()) {
            const auto hierarchyItem = &_dataset->getHierarchyItem();

            disconnect(hierarchyItem, &DataHierarchyItem::actionAdded, this, nullptr);
            disconnect(hierarchyItem, &DataHierarchyItem::datasetNameChanged, this, nullptr);
        }

        _dataset.setDatasetName(datasetName);

        auto createWidgets = [this, datasetName]() -> void {
            _treeWidget->clear();

            auto exposedActions = _dataset->getActions();

            for (auto exposedAction : exposedActions) {
                auto exposedGroupAction = dynamic_cast<GroupAction*>(exposedAction);

                if (exposedGroupAction == nullptr)
                    continue;

                if (exposedGroupAction->isVisible())
                    addButton(exposedGroupAction);
            }
        };

        connect(&_dataset->getHierarchyItem(), &DataHierarchyItem::actionAdded, this, [this, createWidgets]() {
            createWidgets();
        });

        const auto updateWindowTitle = [this]() {
            emit datasetNameChanged(_dataset.getDatasetName());
        };

        connect(&_dataset, &DatasetRef<DataSet>::datasetNameChanged, this, [this, updateWindowTitle](const QString& datasetName) {
            updateWindowTitle();
        });

        updateWindowTitle();
        createWidgets();
    }
    catch (std::exception& e)
    {
        qDebug() << QString("Cannot update data properties: ").arg(datasetName, e.what());
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
