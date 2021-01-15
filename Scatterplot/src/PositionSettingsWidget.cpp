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
    const auto xDimensionToolTipText = "X dimension";

    _xDimensionLabel->setToolTip(xDimensionToolTipText);
    _xDimensionComboBox->setToolTip(xDimensionToolTipText);
    _xDimensionComboBox->setFixedHeight(20);

    const auto yDimensionToolTipText = "Y dimension";

    _yDimensionLabel->setToolTip(yDimensionToolTipText);
    _yDimensionComboBox->setToolTip(yDimensionToolTipText);
    _yDimensionComboBox->setFixedHeight(20);
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

    if (width < 500)
        state = WidgetStateMixin::State::Popup;

    if (width >= 500 && width < 2000)
        state = WidgetStateMixin::State::Compact;

    if (width >= 2000)
        state = WidgetStateMixin::State::Full;

    return state;
}

void PositionSettingsWidget::updateState()
{
    if (layout())
        delete layout();

    const auto applyLayout = [this](QLayout* stateLayout) {
        Q_ASSERT(stateLayout != nullptr);

        stateLayout->setMargin(0);

        setLayout(stateLayout);
    };
    
    QString xDimensionLabelText, yDimensionLabelText;

    switch (_state)
    {
        case State::Popup:
        {
            auto stateLayout = new QGridLayout();

            applyLayout(stateLayout);

            xDimensionLabelText = "X dimension:";
            yDimensionLabelText = "Y dimension:";

            stateLayout->addWidget(_xDimensionLabel, 0, 0);
            stateLayout->addWidget(_xDimensionComboBox, 0, 1);
            stateLayout->addWidget(_yDimensionLabel, 1, 0);
            stateLayout->addWidget(_yDimensionComboBox, 1, 1);

            break;
        }

        case State::Compact:
        {
            auto stateLayout = new QHBoxLayout();

            applyLayout(stateLayout);

            stateLayout->addWidget(_xDimensionLabel);
            stateLayout->addWidget(_xDimensionComboBox);
            stateLayout->addWidget(_yDimensionLabel);
            stateLayout->addWidget(_yDimensionComboBox);

            break;
        }

        case State::Full:
        {
            xDimensionLabelText = "X:";
            yDimensionLabelText = "Y:";

            auto stateLayout = new QHBoxLayout();

            applyLayout(stateLayout);

            stateLayout->addWidget(_xDimensionLabel);
            stateLayout->addWidget(_xDimensionComboBox);
            stateLayout->addWidget(_yDimensionLabel);
            stateLayout->addWidget(_yDimensionComboBox);

            break;
        }

        default:
            break;
    }

    const auto labelsVisible = _state != WidgetStateMixin::State::Compact;

    _xDimensionLabel->setVisible(labelsVisible);
    _yDimensionLabel->setVisible(labelsVisible);

    _xDimensionLabel->setText(xDimensionLabelText);
    _yDimensionLabel->setText(yDimensionLabelText);

    const auto fixedComboBoxWidth = _state == WidgetStateMixin::State::Compact ? 75 : 150;

    _xDimensionComboBox->setFixedWidth(fixedComboBoxWidth);
    _yDimensionComboBox->setFixedWidth(fixedComboBoxWidth);
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