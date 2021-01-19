#include "SubsetSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QPushButton>
#include <QCheckBox>

using namespace hdps::gui;

SubsetSettingsWidget::SubsetSettingsWidget(QWidget* parent /*= nullptr*/) :
    ResponsiveToolBar::Widget("Subset"),
    _createSubsetPushButton(new QPushButton()),
    _fromSourceCheckBox(new QCheckBox())
{
    _createSubsetPushButton->setToolTip("Create a subset from the selected data points");

    _fromSourceCheckBox->setToolTip("Create a subset from source or derived data");
    _fromSourceCheckBox->setText("From source");
}

void SubsetSettingsWidget::initialize(const ScatterplotPlugin& plugin)
{
    QObject::connect(&plugin, qOverload<>(&ScatterplotPlugin::selectionChanged), [this]() {
    });
}

void SubsetSettingsWidget::updateState()
{
    /*
    if (layout())
        delete layout();

    auto stateLayout = new QHBoxLayout();

    setLayout(stateLayout);

    stateLayout->addWidget(_createSubsetPushButton);
    stateLayout->addWidget(_fromSourceCheckBox);

    switch (_state)
    {
        case State::Popup:
            _createSubsetPushButton->setText("Create");
            _fromSourceCheckBox->setText("From source dataset");
            break;

        case State::Compact:
            _createSubsetPushButton->setText("Subset");
            _fromSourceCheckBox->setText("Source");
            break;

        case State::Full:
            _createSubsetPushButton->setText("Create subset");
            _fromSourceCheckBox->setText("From source");
            break;

        default:
            break;
    }
    */
    //_fromSourceCheckBox->setVisible(_state != WidgetStateMixin::State::Compact);
}