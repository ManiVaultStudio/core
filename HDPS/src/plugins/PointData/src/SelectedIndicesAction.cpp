#include "SelectedIndicesAction.h"
#include "DataHierarchyItem.h"
#include "PointData.h"

#include <QGridLayout>
#include <QListView>

using namespace hdps;
using namespace hdps::gui;

SelectedIndicesAction::SelectedIndicesAction(QObject* parent, hdps::CoreInterface* core, const QString& datasetName) :
    WidgetAction(parent, "Selected indices"),
    EventListener(),
    _core(core),
    _dataHierarchyItem(nullptr),
    _updateAction(this, "Update"),
    _manualUpdateAction(this, "Manual update"),
    _selectionChangedTimer()
{
    setText("Selected indices");
    setEventCore(_core);
    
    _dataHierarchyItem = _core->getDataHierarchyItem(datasetName);

    _selectionChangedTimer.setSingleShot(true);

    registerDataEventByType(PointType, [this](hdps::DataEvent* dataEvent) {
        if (dataEvent->dataSetName != _dataHierarchyItem->getDatasetName())
            return;

        switch (dataEvent->getType()) {
            case EventType::DataAdded:
            case EventType::DataChanged:
            case EventType::SelectionChanged:
            {
                if (dataEvent->getType() == EventType::SelectionChanged)
                    _selectionChangedTimer.start(100);

                break;
            }
            
            default:
                break;
        }
    });

    connect(&_selectionChangedTimer, &QTimer::timeout, this, [this]() {
        if (_manualUpdateAction.isChecked())
            return;

        emit selectedIndicesChanged(getSelectedIndices());
    });

    const auto updateUpdateAction = [this]() -> void {
        _updateAction.setEnabled(_manualUpdateAction.isChecked());
    };

    connect(&_manualUpdateAction, &ToggleAction::toggled, this, [this, updateUpdateAction]() {
        updateUpdateAction();
    });

    connect(&_updateAction, &TriggerAction::triggered, this, [this]() {
        emit selectedIndicesChanged(getSelectedIndices());
    });

    updateUpdateAction();
}

const std::vector<std::uint32_t>& SelectedIndicesAction::getSelectedIndices() const
{
    auto& points    = _dataHierarchyItem->getDataset<Points>();
    auto& selection = dynamic_cast<Points&>(points.getSelection());

    return selection.indices;
}

SelectedIndicesAction::Widget::Widget(QWidget* parent, SelectedIndicesAction* selectedIndicesAction, const hdps::gui::WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, selectedIndicesAction, state)
{
    auto selectedIndicesListWidget = new QListView();

    selectedIndicesListWidget->setFixedHeight(100);

    auto selectedIndicesLayout = new QHBoxLayout();

    selectedIndicesLayout->setMargin(0);
    selectedIndicesLayout->addWidget(selectedIndicesListWidget);

    auto updateLayout = new QVBoxLayout();

    updateLayout->addWidget(selectedIndicesAction->getUpdateAction().createWidget(this));
    updateLayout->addWidget(selectedIndicesAction->getManualUpdateAction().createCheckBoxWidget(this));
    updateLayout->addStretch(1);

    selectedIndicesLayout->addLayout(updateLayout);

    setLayout(selectedIndicesLayout);

    const auto updateSelectedIndicesWidget = [this, selectedIndicesAction, selectedIndicesListWidget]() -> void {
        QStringList items;

        for (auto selectedIndex : selectedIndicesAction->getSelectedIndices())
            items << QString::number(selectedIndex);

        selectedIndicesListWidget->setModel(new QStringListModel(items));
    };

    connect(selectedIndicesAction, &SelectedIndicesAction::selectedIndicesChanged, this, [this, updateSelectedIndicesWidget]() {
        updateSelectedIndicesWidget();
    });

    updateSelectedIndicesWidget();
}
