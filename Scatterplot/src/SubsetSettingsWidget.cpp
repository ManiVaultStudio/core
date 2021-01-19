#include "SubsetSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QPushButton>
#include <QCheckBox>

using namespace hdps::gui;

SubsetSettingsWidget::SubsetSettingsWidget(QWidget* parent /*= nullptr*/) :
    ResponsiveToolBar::Widget("Subset", 800),
    _createSubsetPushButton(new QPushButton()),
    _fromSourceCheckBox(new QCheckBox())
{
    _popupPushButton->setIcon(Application::getIconFont("FontAwesome").getIcon("crop-alt"));

    _createSubsetPushButton->setToolTip("Create a subset from the selected data points");

    _fromSourceCheckBox->setToolTip("Create a subset from source or derived data");
    _fromSourceCheckBox->setText("From source");

    computeStateSizes();
}

void SubsetSettingsWidget::initialize(const ScatterplotPlugin& plugin)
{
    const auto updateUI = [this, &plugin]() {
        setEnabled(const_cast<ScatterplotPlugin&>(plugin).getNumSelectedPoints() >= 1);
    };

    QObject::connect(&plugin, qOverload<>(&ScatterplotPlugin::selectionChanged), [this, updateUI]() {
        updateUI();
    });

    updateUI();
}

void SubsetSettingsWidget::updateState()
{
    auto layout = new QHBoxLayout();

    setWidgetLayout(layout);

    layout->addWidget(_createSubsetPushButton);
    layout->addWidget(_fromSourceCheckBox);

    layout->invalidate();
    layout->activate();

    setCurrentIndex(_state == State::Popup ? 0 : 1);

    switch (_state)
    {
        case State::Popup:
            _createSubsetPushButton->setText("Create");
            _fromSourceCheckBox->setText("From source dataset");
            break;

        case State::Compact:
            _createSubsetPushButton->setText("Create subset");
            _fromSourceCheckBox->setText("Source");
            break;

        case State::Full:
            _createSubsetPushButton->setText("Create subset");
            _fromSourceCheckBox->setText("From source");
            break;

        default:
            break;
    }
    
    _fromSourceCheckBox->setVisible(_state != State::Compact);
}