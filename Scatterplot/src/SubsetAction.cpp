#include "SubsetAction.h"
#include "ScatterplotPlugin.h"
#include "PointData.h"

#include <QMenu>

using namespace hdps;
using namespace hdps::gui;

SubsetAction::SubsetAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin, "Subset"),
    _subsetNameAction(this, "Subset name"),
    _createSubsetAction(this, "Create subset"),
    _sourceDataAction(this, "Source data")
{
    _subsetNameAction.setToolTip("Name of the subset");
    _createSubsetAction.setToolTip("Create subset from selected data points");

    const auto updateActions = [this]() -> void {
        setEnabled(_scatterplotPlugin->getNumberOfSelectedPoints() >= 1);
    };

    connect(_scatterplotPlugin, qOverload<>(&ScatterplotPlugin::selectionChanged), this, updateActions);

    connect(&_createSubsetAction, &QAction::triggered, this, [this]() {
        _scatterplotPlugin->createSubset(_sourceDataAction.getCurrentIndex() == 1, _subsetNameAction.getString());
    });

    const auto onCurrentDatasetChanged = [this]() -> void {
        const auto datasetName = _scatterplotPlugin->getCurrentDataset();

        QStringList sourceDataOptions;

        if (!datasetName.isEmpty()) {
            const auto sourceDatasetName = DataSet::getSourceData(_scatterplotPlugin->getCore()->requestData<Points>(datasetName)).getName();

            sourceDataOptions << datasetName;

            if (sourceDatasetName != datasetName)
                sourceDataOptions << QString("%1 (source data)").arg(sourceDatasetName);
        }

        _sourceDataAction.setOptions(sourceDataOptions);
        _sourceDataAction.setEnabled(sourceDataOptions.count() >= 2);
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
    menu->addAction(&_sourceDataAction);

    return menu;
}

SubsetAction::Widget::Widget(QWidget* parent, SubsetAction* subsetAction) :
    WidgetAction::Widget(parent, subsetAction),
    _layout()
{
    _layout.addWidget(new StandardAction::PushButton(this, &subsetAction->_createSubsetAction));
    _layout.addWidget(new OptionAction::Widget(this, &subsetAction->_sourceDataAction));

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