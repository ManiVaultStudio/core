#include "PositionSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include "widgets/ResponsiveToolBar.h"

#include <QLabel>
#include <QComboBox>

using namespace hdps::gui;

PositionSettingsWidget::PositionSettingsWidget(QWidget* parent /*= nullptr*/) :
    QStackedWidget(parent),
    _widgetState(this),
    _popupPushButton(new PopupPushButton()),
    _widget(new QWidget()),
    _xDimensionLabel(new QLabel()),
    _xDimensionComboBox(new QComboBox()),
    _yDimensionLabel(new QLabel()),
    _yDimensionComboBox(new QComboBox())
{
    initializeUI();

    connect(&_widgetState, &WidgetState::updateState, [this](const WidgetState::State& state) {
        const auto setWidgetLayout = [this](QLayout* layout) -> void {
            if (_widget->layout())
                delete _widget->layout();

            layout->setMargin(ResponsiveToolBar::LAYOUT_MARGIN);
            layout->setSpacing(ResponsiveToolBar::LAYOUT_SPACING);

            _widget->setLayout(layout);
        };

        switch (state)
        {
            case WidgetState::State::Popup:
            {
                auto layout = new QGridLayout();

                setWidgetLayout(layout);

                layout->addWidget(_xDimensionLabel, 0, 0);
                layout->addWidget(_xDimensionComboBox, 0, 1);
                layout->addWidget(_yDimensionLabel, 1, 0);
                layout->addWidget(_yDimensionComboBox, 1, 1);

                setCurrentWidget(_popupPushButton);
                removeWidget(_widget);
                break;
            }

            case WidgetState::State::Compact:
            case WidgetState::State::Full:
            {
                auto layout = new QHBoxLayout();

                setWidgetLayout(layout);

                layout->addWidget(_xDimensionLabel);
                layout->addWidget(_xDimensionComboBox);
                layout->addWidget(_yDimensionLabel);
                layout->addWidget(_yDimensionComboBox);

                if (count() == 1)
                    addWidget(_widget);

                setCurrentWidget(_widget);
                break;
            }

            default:
                break;
        }

        switch (state)
        {
            case WidgetState::State::Compact:
                _xDimensionLabel->setText("X:");
                _yDimensionLabel->setText("Y:");
                _xDimensionComboBox->setFixedWidth(80);
                _yDimensionComboBox->setFixedWidth(80);
                break;

            case WidgetState::State::Popup:
            case WidgetState::State::Full:
                _xDimensionLabel->setText("X dimension:");
                _yDimensionLabel->setText("Y dimension:");
                _xDimensionComboBox->setFixedWidth(120);
                _yDimensionComboBox->setFixedWidth(120);
                break;

            default:
                break;
        }
    });

    _widgetState.initialize();
}

void PositionSettingsWidget::initializeUI()
{
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    setWindowTitle("Position settings");
    setToolTip("Position settings");

    _popupPushButton->setWidget(_widget);
    _popupPushButton->setIcon(Application::getIconFont("FontAwesome").getIcon("ruler-combined"));
    
    _widget->setWindowTitle("Position");

    const auto xDimensionToolTipText = "X dimension";

    _xDimensionLabel->setToolTip(xDimensionToolTipText);
    _xDimensionComboBox->setToolTip(xDimensionToolTipText);

    const auto yDimensionToolTipText = "Y dimension";

    _yDimensionLabel->setToolTip(yDimensionToolTipText);
    _yDimensionComboBox->setToolTip(yDimensionToolTipText);

    addWidget(_popupPushButton);
    addWidget(_widget);
}

void PositionSettingsWidget::setScatterPlotPlugin(const ScatterplotPlugin& plugin)
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