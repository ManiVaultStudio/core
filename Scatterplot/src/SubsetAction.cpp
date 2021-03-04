#include "SubsetAction.h"

#include "ScatterplotPlugin.h"

using namespace hdps::gui;

SubsetAction::SubsetAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin, "Subset"),
    _subsetNameAction(this, "Subset name"),
    _createSubsetAction(this, "Create subset"),
    _fromSourceDataAction(this, "From source data")
{
    _subsetNameAction.setToolTip("Name of the subset");
    _createSubsetAction.setToolTip("Create subset from selected data points");
    _fromSourceDataAction.setToolTip("Create subset from source data");

    _fromSourceDataAction.setCheckable(true);
    _fromSourceDataAction.setChecked(true);

    const auto updateActions = [this]() -> void {
        setEnabled(_scatterplotPlugin->getNumberOfSelectedPoints() >= 1);
    };

    connect(_scatterplotPlugin, qOverload<>(&ScatterplotPlugin::selectionChanged), this, updateActions);

    connect(&_createSubsetAction, &QAction::triggered, this, [this]() {
        _scatterplotPlugin->createSubset(_fromSourceDataAction.isChecked(), _subsetNameAction.getString());
    });

    const auto onCurrentDatasetChanged = [this]() -> void {
        const auto currentDatasetName = _scatterplotPlugin->getCurrentDataset();

        if (currentDatasetName.isEmpty())
            _subsetNameAction.setString("");
        else
            _subsetNameAction.setString(QString("%1_subset").arg(currentDatasetName));
    };

    connect(scatterplotPlugin, &ScatterplotPlugin::currentDatasetChanged, this, [this, onCurrentDatasetChanged](const QString& datasetName) {
        onCurrentDatasetChanged();
    });

    onCurrentDatasetChanged();
    updateActions();
}

QMenu* SubsetAction::getContextMenu()
{
    auto menu = new QMenu("Subset");

    menu->addAction(&_createSubsetAction);
    menu->addAction(&_fromSourceDataAction);

    return menu;
}

SubsetAction::Widget::Widget(QWidget* parent, SubsetAction* subsetAction) :
    WidgetAction::Widget(parent, subsetAction),
    _layout()
{
    //_layout.addWidget(new StringAction::Widget(this, &subsetAction->_subsetNameAction, false));
    _layout.addWidget(new StandardAction::PushButton(this, &subsetAction->_createSubsetAction));
    _layout.addWidget(new StandardAction::CheckBox(this, &subsetAction->_fromSourceDataAction));

    //_layout.itemAt(0)->widget()->setFixedWidth(80);

    setLayout(&_layout);
}

SubsetAction::PopupWidget::PopupWidget(QWidget* parent, SubsetAction* subsetAction) :
    WidgetAction::PopupWidget(parent, subsetAction)
{
    auto layout = new QVBoxLayout();

    layout->addWidget(new SubsetAction::Widget(this, subsetAction));

    setLayout(layout);
}