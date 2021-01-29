#include "PositionSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include "widgets/ResponsiveToolBar.h"

#include <QLabel>
#include <QComboBox>

using namespace hdps::gui;

PositionSettingsWidget::PositionSettingsWidget(const hdps::gui::ResponsiveToolBar::WidgetState& state, QWidget* parent /*= nullptr*/) :
    ScatterplotSettingsWidget(state, parent),
    _xDimensionLabel(new QLabel()),
    _xDimensionComboBox(new QComboBox()),
    _yDimensionLabel(new QLabel()),
    _yDimensionComboBox(new QComboBox())
{
    initializeUI();
}

void PositionSettingsWidget::initializeUI()
{
    const auto xDimensionToolTipText = "X dimension";

    _xDimensionLabel->setToolTip(xDimensionToolTipText);
    _xDimensionComboBox->setToolTip(xDimensionToolTipText);

    const auto yDimensionToolTipText = "Y dimension";

    _yDimensionLabel->setToolTip(yDimensionToolTipText);
    _yDimensionComboBox->setToolTip(yDimensionToolTipText);

    QLayout* stateLayout = nullptr;

    switch (_state)
    {
        case ResponsiveToolBar::WidgetState::Popup:
        {
            auto layout = new QGridLayout();

            layout->addWidget(_xDimensionLabel, 0, 0);
            layout->addWidget(_xDimensionComboBox, 0, 1);
            layout->addWidget(_yDimensionLabel, 1, 0);
            layout->addWidget(_yDimensionComboBox, 1, 1);

            stateLayout = layout;
            break;
        }

        case ResponsiveToolBar::WidgetState::Compact:
        case ResponsiveToolBar::WidgetState::Full:
        {
            auto layout = new QHBoxLayout();

            layout->addWidget(_xDimensionLabel);
            layout->addWidget(_xDimensionComboBox);
            layout->addWidget(_yDimensionLabel);
            layout->addWidget(_yDimensionComboBox);

            stateLayout = layout;
            break;
        }

        default:
            break;
    }

    switch (_state)
    {
        case ResponsiveToolBar::WidgetState::Compact:
            _xDimensionLabel->setText("X:");
            _yDimensionLabel->setText("Y:");
            _xDimensionComboBox->setFixedWidth(80);
            _yDimensionComboBox->setFixedWidth(80);
            break;

        case ResponsiveToolBar::WidgetState::Popup:
        case ResponsiveToolBar::WidgetState::Full:
            _xDimensionLabel->setText("X dimension:");
            _yDimensionLabel->setText("Y dimension:");
            _xDimensionComboBox->setFixedWidth(120);
            _yDimensionComboBox->setFixedWidth(120);
            break;

        default:
            break;
    }

    stateLayout->setMargin(0);
    stateLayout->setSpacing(4);

    setLayout(stateLayout);
}

void PositionSettingsWidget::setScatterPlotPlugin(const ScatterplotPlugin& plugin)
{
    _scatterplotPlugin = &const_cast<ScatterplotPlugin&>(plugin);

    QObject::connect(_xDimensionComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [&plugin](int index) {
        const_cast<ScatterplotPlugin&>(plugin).xDimPicked(index);
    });

    QObject::connect(_yDimensionComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [&plugin](int index) {
        const_cast<ScatterplotPlugin&>(plugin).yDimPicked(index);
    });

    /*
    QObject::connect(_ui->colorOptionsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
        switch (index)
        {
            case 0:
                _ui->colorDimensionStackedWidget->setCurrentIndex(0);
                break;

            case 1:
                _ui->colorDimensionStackedWidget->setCurrentIndex(1);
                break;
        }
    });
    
    _ui->colorDimensionStackedWidget->initialize(plugin);
    */
}

int PositionSettingsWidget::getDimensionX()
{
    return _xDimensionComboBox->currentIndex();
}

int PositionSettingsWidget::getDimensionY()
{
    return _yDimensionComboBox->currentIndex();
}

void PositionSettingsWidget::setDimensions(unsigned int numDimensions, const std::vector<QString>& names /*= std::vector<QString>()*/)
{
    auto& stringListModel = createStringListModel(numDimensions, names, *this);

    QSignalBlocker xDimensionComboBoxSignalBlocker(_xDimensionComboBox), yDimensionComboBoxSignalBlocker(_yDimensionComboBox);

    _xDimensionComboBox->setModel(&stringListModel);
    _yDimensionComboBox->setModel(&stringListModel);

    if (numDimensions >= 2) {
        _xDimensionComboBox->setCurrentIndex(0);
        _yDimensionComboBox->setCurrentIndex(1);
    }
}