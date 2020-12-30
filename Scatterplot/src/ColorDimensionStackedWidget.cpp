#include "ColorDimensionStackedWidget.h"
#include "ScatterplotPlugin.h"

#include "ui_ColorDimensionStackedWidget.h"

#include <QStringListModel>

ColorDimensionStackedWidget::ColorDimensionStackedWidget(QWidget* parent /*= nullptr*/) :
    QStackedWidget(parent),
    _ui{ std::make_unique<Ui::ColorDimensionStackedWidget>() }
{
    _ui->setupUi(this);
}

void ColorDimensionStackedWidget::initialize(const ScatterplotPlugin& plugin)
{
    QObject::connect(_ui->colorByDimensionComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [&plugin](int index) {
        const_cast<ScatterplotPlugin&>(plugin).cDimPicked(index);
    });

    /*
    QObject::connect(plugin._scatterPlotWidget, &ScatterplotWidget::renderModeChanged, [this](const ScatterplotWidget::RenderMode& renderMode) {
        switch (renderMode)
        {
            case ScatterplotWidget::RenderMode::SCATTERPLOT:
                setCurrentIndex(0);
                break;

            case ScatterplotWidget::RenderMode::DENSITY:
            case ScatterplotWidget::RenderMode::LANDSCAPE:
                setCurrentIndex(1);
                break;
        }
    });

    _ui->pointSettingsWidget->initialize(plugin);
    _ui->densitySettingsWidget->initialize(plugin);
    */
}

void ColorDimensionStackedWidget::setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names)
{
    auto& stringListModel = createStringListModel(numDimensions, names, *this);

    QSignalBlocker signalBlocker(_ui->colorByDimensionComboBox);

    _ui->colorByDimensionComboBox->setModel(&stringListModel);
}

QComboBox* ColorDimensionStackedWidget::getColorByDimensionComboBox()
{
    return _ui->colorByDimensionComboBox;
}