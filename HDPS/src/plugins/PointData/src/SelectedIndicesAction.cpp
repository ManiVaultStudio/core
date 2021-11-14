#include "SelectedIndicesAction.h"

#include <QGridLayout>
#include <QListView>

#include <QSet>

using namespace hdps;
using namespace hdps::gui;

SelectedIndicesAction::SelectedIndicesAction(QObject* parent, hdps::CoreInterface* core, Points& points) :
    WidgetAction(parent),
    EventListener(),
    _core(core),
    _points(&points),
    _updateAction(this, "Update"),
    _manualUpdateAction(this, "Manual update"),
    _selectionChangedTimer(),
    _selectedIndices()
{
    setText("Selected indices");
    setEventCore(_core);

    _selectionChangedTimer.setSingleShot(true);

    // Register to points data events
    registerDataEventByType(PointType, [this](hdps::DataEvent* dataEvent) {
        if (!_points.isValid())
            return;

        // Only process points that we reference
        if (dataEvent->getDataset() != _points.getSourceData())
            return;

        switch (dataEvent->getType()) {

            case EventType::DataAdded:
            {
                // Turn manual update on if there are more than one million points (for performance reasons)
                _manualUpdateAction.setChecked(_points->getNumPoints() > MANUAL_UPDATE_THRESHOLD);

                break;
            }

            case EventType::DataChanged:
            case EventType::DataSelectionChanged:
            {
                // Update selected indices
                if (_points->getNumPoints() < MANUAL_UPDATE_THRESHOLD)
                    _selectionChangedTimer.start(100);
                
                break;
            }
            
            default:
                break;
        }
    });

    connect(&_selectionChangedTimer, &QTimer::timeout, this, [this]() {

        // Do not update if manual update is chosen
        if (_manualUpdateAction.isChecked())
            return;

        // Compute the selected indices
        updateSelectedIndices();
    });

    // Update actions
    const auto updateActions = [this]() -> void {
        _updateAction.setEnabled(_manualUpdateAction.isChecked());
    };

    connect(&_manualUpdateAction, &ToggleAction::toggled, this, [this, updateActions]() {
        updateActions();
    });

    connect(&_updateAction, &TriggerAction::triggered, this, [this]() {
        updateSelectedIndices();
        emit selectedIndicesChanged(getSelectedIndices());
    });

    updateActions();
}

const std::vector<std::uint32_t>& SelectedIndicesAction::getSelectedIndices() const
{
    return _selectedIndices;
}

void SelectedIndicesAction::updateSelectedIndices()
{
    // Only proceed if we have a valid points reference
    if (!_points.isValid())
        return;

    // Get points selection
    auto& selection = dynamic_cast<Points&>(_points->getSelection());

    // Generate 
    if (_points->isFull()) {
        _selectedIndices = selection.indices;
    }
    else {
        _selectedIndices.clear();
        _selectedIndices.reserve(_points->indices.size());

        // Create points indices set
        QSet<std::uint32_t> indicesSet(_points->indices.begin(), _points->indices.end());

        // Add selection indices if they belong to the subset
        for (const auto& selectionIndex : selection.indices)
            if (indicesSet.contains(selectionIndex))
                _selectedIndices.push_back(selectionIndex);
    }

    // Notify others that the selection indices changed
    emit selectedIndicesChanged(getSelectedIndices());
}

SelectedIndicesAction::Widget::Widget(QWidget* parent, SelectedIndicesAction* selectedIndicesAction) :
    WidgetActionWidget(parent, selectedIndicesAction)
{
    auto selectedIndicesListWidget = new QListView();

    selectedIndicesListWidget->setFixedHeight(100);

    auto selectedIndicesLayout = new QHBoxLayout();

    selectedIndicesLayout->setMargin(0);
    selectedIndicesLayout->addWidget(selectedIndicesListWidget);

    auto updateLayout = new QVBoxLayout();

    updateLayout->addWidget(selectedIndicesAction->getUpdateAction().createWidget(this));
    updateLayout->addWidget(selectedIndicesAction->getManualUpdateAction().createWidget(this));
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
