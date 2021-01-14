#include "PositionSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QLabel>
#include <QComboBox>

PositionSettingsWidget::PositionSettingsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(),
    WidgetStateMixin("Position"),
    _xDimensionLabel(new QLabel()),
    _xDimensionComboBox(new QComboBox()),
    _yDimensionLabel(new QLabel()),
    _yDimensionComboBox(new QComboBox())
{
    _xDimensionLabel->setText("X:");
    _yDimensionLabel->setText("Y:");

    _xDimensionComboBox->setFixedWidth(100);
    _yDimensionComboBox->setFixedWidth(100);

    const auto xDimensionToolTipText = "X dimension";

    _xDimensionLabel->setToolTip(xDimensionToolTipText);
    _xDimensionComboBox->setToolTip(xDimensionToolTipText);

    const auto yDimensionToolTipText = "Y dimension";

    _yDimensionLabel->setToolTip(yDimensionToolTipText);
    _yDimensionComboBox->setToolTip(yDimensionToolTipText);
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

WidgetStateMixin::State PositionSettingsWidget::getState(const QSize& sourceWidgetSize) const
{
    const auto width = sourceWidgetSize.width();

    auto state = WidgetStateMixin::State::Popup;

    if (width >= 1000 && width < 1500)
        state = WidgetStateMixin::State::Compact;

    if (width >= 1500)
        state = WidgetStateMixin::State::Full;

    return state;
}

void PositionSettingsWidget::updateState()
{
    const auto applyLayout = [this](QLayout* stateLayout) {
        Q_ASSERT(stateLayout != nullptr);

        if (layout())
            delete layout();

        stateLayout->setMargin(0);

        setLayout(stateLayout);
    };
    
    QString xDimensionLabelText, yDimensionLabelText;

    switch (_state)
    {
        case State::Popup:
        {
            auto stateLayout = new QGridLayout();

            xDimensionLabelText = "X dimension:";
            yDimensionLabelText = "Y dimension:";

            stateLayout->addWidget(_xDimensionLabel, 0, 0);
            stateLayout->addWidget(_xDimensionComboBox, 0, 1);
            stateLayout->addWidget(_yDimensionLabel, 1, 0);
            stateLayout->addWidget(_yDimensionComboBox, 1, 1);

            applyLayout(stateLayout);

            break;
        }

        case State::Compact:
        {
            xDimensionLabelText = "X dim.:";
            yDimensionLabelText = "Y dim.:";

            auto stateLayout = new QHBoxLayout();

            stateLayout->addWidget(_xDimensionLabel);
            stateLayout->addWidget(_xDimensionComboBox);
            stateLayout->addWidget(_yDimensionLabel);
            stateLayout->addWidget(_yDimensionComboBox);

            applyLayout(stateLayout);

            break;
        }

        case State::Full:
        {
            xDimensionLabelText = "X dimension:";
            yDimensionLabelText = "Y dimension:";

            auto stateLayout = new QHBoxLayout();

            stateLayout->addWidget(_xDimensionLabel);
            stateLayout->addWidget(_xDimensionComboBox);
            stateLayout->addWidget(_yDimensionLabel);
            stateLayout->addWidget(_yDimensionComboBox);

            applyLayout(stateLayout);

            break;
        }

        default:
            break;
    }

    _xDimensionLabel->setText(xDimensionLabelText);
    _yDimensionLabel->setText(yDimensionLabelText);

    /*
    _doubleSpinBox->setVisible(_state != WidgetStateMixin::State::Compact);
    _slider->setFixedWidth(_state == WidgetStateMixin::State::Popup ? 100 : 40);
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

void PositionSettingsWidget::setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names /*= std::vector<QString>()*/)
{
    /*
    auto& stringListModel = createStringListModel(numDimensions, names, *this);

    auto colorByDimensionComboBox = _ui->colorDimensionStackedWidget->getColorByDimensionComboBox();

    QSignalBlocker signalBlocker(colorByDimensionComboBox);

    colorByDimensionComboBox->setModel(&stringListModel);
    */
}