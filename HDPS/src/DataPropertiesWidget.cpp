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
    _treeWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

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

            if (exposedWidgetActionGroup->isVisible())
                addButton(exposedWidgetActionGroup);
        }
    }
    catch (std::exception& e)
    {
    }
}

QTreeWidgetItem* DataPropertiesWidget::addButton(WidgetActionGroup* widgetActionGroup)
{
    auto treeWidgetItem = new QTreeWidgetItem();

    _treeWidget->addTopLevelItem(treeWidgetItem);

    auto button = new SectionExpandButton(treeWidgetItem, widgetActionGroup, widgetActionGroup->text());

    _treeWidget->setItemWidget(treeWidgetItem, 0, button);
    
    return treeWidgetItem;
}

SectionExpandButton::SectionExpandButton(QTreeWidgetItem* treeWidgetItem, WidgetActionGroup* widgetActionGroup, const QString& text, QWidget* parent /*= nullptr*/) :
    QPushButton(text, parent),
    _widgetActionGroup(widgetActionGroup),
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

    connect(_widgetActionGroup, &WidgetActionGroup::expanded, this, [this, update]() {
        update();
    });

    connect(_widgetActionGroup, &WidgetActionGroup::collapsed, this, [this, update]() {
        update();
    });

    update();
}

}
}
