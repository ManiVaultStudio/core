#include "ColorSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

using namespace hdps::gui;

ColorSettingsWidget::ColorDimensionWidget::ColorDimensionWidget() :
    QWidget(),
    _colorDimensionComboBox(new QComboBox()),
    _layout(new QHBoxLayout()),
    _colorDataLineEdit(new QLineEdit()),
    _removeColorDataPushButton(new QPushButton())
{
    //setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    
    setLayout(_layout);

    _layout->setMargin(ResponsiveToolBar::LAYOUT_MARGIN);
    _layout->setSpacing(ResponsiveToolBar::LAYOUT_SPACING);

    _layout->addWidget(_colorDimensionComboBox);
    _layout->addWidget(_colorDataLineEdit);
    _layout->addWidget(_removeColorDataPushButton);

    _colorDataLineEdit->setEnabled(false);
    _colorDataLineEdit->setReadOnly(true);
    _colorDataLineEdit->setText("No color data...");
    _colorDataLineEdit->setTextMargins(5, 0, 0, 0);

    _removeColorDataPushButton->setIcon(Application::getIconFont("FontAwesome").getIcon("trash"));
}

QComboBox* ColorSettingsWidget::ColorDimensionWidget::getColorDimensionComboBox()
{
    return _colorDimensionComboBox;
}

ColorSettingsWidget::ColorSettingsWidget(QWidget* parent /*= nullptr*/) :
    ResponsiveToolBar::StatefulWidget(parent, "Color"),
    _colorByLabel(new QLabel()),
    _colorByComboBox(new QComboBox()),
    _colorDimensionWidget(new ColorDimensionWidget())
{
    initializeUI();
}

void ColorSettingsWidget::initializeUI()
{
    _colorByLabel->setToolTip("Color by");
    _colorByLabel->setText("Color by:");

    _colorByComboBox->setToolTip("Color by");
    _colorByComboBox->setFixedWidth(75);
    _colorByComboBox->addItem("Dimension");
    _colorByComboBox->addItem("Data");
}

void ColorSettingsWidget::setScatterPlotPlugin(const ScatterplotPlugin& plugin)
{
    connect(_colorByComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
        //_colorDimensionWidget->setCurrentIndex(index);
        _colorDimensionWidget->adjustSize();
    });

    connect(_colorDimensionWidget->getColorDimensionComboBox(), qOverload<int>(&QComboBox::currentIndexChanged), [this, &plugin](int index) {
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

void ColorSettingsWidget::setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names)
{
    auto& stringListModel = createStringListModel(numDimensions, names, *this);

    QSignalBlocker signalBlocker(_colorDimensionWidget->getColorDimensionComboBox());

    _colorDimensionWidget->getColorDimensionComboBox()->setModel(&stringListModel);
}

QLayout* ColorSettingsWidget::getLayout(const ResponsiveToolBar::WidgetState& state)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(_colorByLabel);
    layout->addWidget(_colorByComboBox);
    layout->addWidget(_colorDimensionWidget);

    return layout;
}