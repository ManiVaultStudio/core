#include "PositionSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QLabel>
#include <QComboBox>

using namespace hdps::gui;

PositionSettingsWidget::PositionSettingsWidget(QWidget* parent /*= nullptr*/) :
    ResponsiveToolBar::Widget("Position", 1000),
    _xDimensionLabel(new QLabel()),
    _xDimensionComboBox(new QComboBox()),
    _yDimensionLabel(new QLabel()),
    _yDimensionComboBox(new QComboBox())
{
    _popupPushButton->setIcon(Application::getIconFont("FontAwesome").getIcon("ruler-combined"));

    const auto xDimensionToolTipText = "X dimension";

    _xDimensionLabel->setToolTip(xDimensionToolTipText);
    _xDimensionComboBox->setToolTip(xDimensionToolTipText);

    const auto yDimensionToolTipText = "Y dimension";

    _yDimensionLabel->setToolTip(yDimensionToolTipText);
    _yDimensionComboBox->setToolTip(yDimensionToolTipText);

    computeStateSizes();
}

void PositionSettingsWidget::initialize(const ScatterplotPlugin& plugin)
{
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

void PositionSettingsWidget::updateState()
{
    switch (_state)
    {
        case State::Popup:
        {
            setCurrentIndex(0);
            break;
        }

        case State::Compact:
        case State::Full:
        {
            auto layout = new QHBoxLayout();

            setWidgetLayout(layout);

            layout->addWidget(_xDimensionLabel);
            layout->addWidget(_xDimensionComboBox);
            layout->addWidget(_yDimensionLabel);
            layout->addWidget(_yDimensionComboBox);

            layout->invalidate();
            layout->activate();

            setCurrentIndex(1);
            break;
        }

        default:
            break;
    }

    switch (_state)
    {
        case State::Compact:
            _xDimensionLabel->setText("X:");
            _yDimensionLabel->setText("Y:");
            _xDimensionComboBox->setFixedWidth(80);
            _yDimensionComboBox->setFixedWidth(80);
            break;

        case State::Popup:
        case State::Full:
            _xDimensionLabel->setText("X dimension:");
            _yDimensionLabel->setText("Y dimension:");
            _xDimensionComboBox->setFixedWidth(120);
            _yDimensionComboBox->setFixedWidth(120);
            break;

        default:
            break;
    }
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

void PositionSettingsWidget::setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names /*= std::vector<QString>()*/)
{
    /*
    auto& stringListModel = createStringListModel(numDimensions, names, *this);

    auto colorByDimensionComboBox = _ui->colorDimensionStackedWidget->getColorByDimensionComboBox();

    QSignalBlocker signalBlocker(colorByDimensionComboBox);

    colorByDimensionComboBox->setModel(&stringListModel);
    */
}