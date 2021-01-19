#include "ColorSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QLabel>
#include <QComboBox>

using namespace hdps::gui;

ColorSettingsWidget::ColorSettingsWidget(QWidget* parent /*= nullptr*/) :
    ResponsiveToolBar::Widget("Color"),
    _colorByLabel(new QLabel()),
    _colorByComboBox(new QComboBox()),
    _colorDimensionComboBox(new QComboBox())
{
    _popupPushButton->setIcon(Application::getIconFont("FontAwesome").getIcon("palette"));

    _colorByLabel->setToolTip("Color by");
    _colorByLabel->setText("Color by:");

    _colorByComboBox->setToolTip("Color by");
    _colorByComboBox->setFixedWidth(75);
    _colorByComboBox->addItem("Dimension");
    _colorByComboBox->addItem("Data");

    computeStateSizes();
}

void ColorSettingsWidget::initialize(const ScatterplotPlugin& plugin)
{
    connect(_colorByComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
        _colorDimensionComboBox->setVisible(index == 0);
    });

    connect(_colorDimensionComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this, &plugin](int index) {
        const_cast<ScatterplotPlugin&>(plugin).cDimPicked(index);
    });
    
    auto scatterPlotWidget = const_cast<ScatterplotPlugin&>(plugin).getScatterplotWidget();

    const auto renderModeChanged = [this, scatterPlotWidget]() {
        setEnabled(scatterPlotWidget->getRenderMode() == ScatterplotWidget::RenderMode::SCATTERPLOT);
    };

    QObject::connect(scatterPlotWidget, &ScatterplotWidget::renderModeChanged, [this, renderModeChanged](const ScatterplotWidget::RenderMode& renderMode) {
        renderModeChanged();
    });

    renderModeChanged();
}

void ColorSettingsWidget::updateState()
{
    auto layout = new QHBoxLayout();

    setWidgetLayout(layout);

    layout->addWidget(_colorByLabel);
    layout->addWidget(_colorByComboBox);
    layout->addWidget(_colorDimensionComboBox);

    layout->invalidate();
    layout->activate();

    switch (_state)
    {
        case State::Popup:
            setCurrentIndex(0);
            break;

        case State::Compact:
        case State::Full:
            setCurrentIndex(1);
            break;

        default:
            break;
    }

    _colorDimensionComboBox->setFixedWidth(_state == State::Compact ? 80 : 120);
}

void ColorSettingsWidget::setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names)
{
    auto& stringListModel = createStringListModel(numDimensions, names, *this);

    QSignalBlocker signalBlocker(_colorDimensionComboBox);

    _colorDimensionComboBox->setModel(&stringListModel);
}