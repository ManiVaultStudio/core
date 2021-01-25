#include "PlotSettingsWidget.h"
#include "ScatterplotPlugin.h"
#include "PointSettingsWidget.h"
#include "DensitySettingsWidget.h"

using namespace hdps::gui;

PlotSettingsWidget::PlotSettingsWidget(QWidget* parent /*= nullptr*/) :
    ResponsiveToolBar::StatefulWidget(parent, "Plot"),
    _pointSettingsWidget(new PointSettingsWidget(this)),
    _densitySettingsWidget(new DensitySettingsWidget(this))
{
    initializeUI();
}

void PlotSettingsWidget::initializeUI()
{
    /*
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    setWindowTitle("Point settings");
    setToolTip("Point settings");

    _popupPushButton->setWidget(_widget);
    _popupPushButton->setIcon(Application::getIconFont("FontAwesome").getIcon("paint-brush"));
    
    _widget->setWindowTitle("Point rendering");

    _sizeLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    
    _sizeDoubleSpinBox->setMinimum(1.0);
    _sizeDoubleSpinBox->setMaximum(20.0);
    _sizeDoubleSpinBox->setDecimals(1);
    _sizeDoubleSpinBox->setSuffix("px");

    _sizeSlider->setOrientation(Qt::Horizontal);
    _sizeSlider->setMinimum(1000);
    _sizeSlider->setMaximum(20000);

    _opacityLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    _opacityDoubleSpinBox->setMinimum(1.0);
    _opacityDoubleSpinBox->setMaximum(100.0);
    _opacityDoubleSpinBox->setDecimals(1);
    _opacityDoubleSpinBox->setSuffix("%");

    _opacitySlider->setOrientation(Qt::Horizontal);
    _opacitySlider->setMinimum(1);
    _opacitySlider->setMaximum(100);

    const auto sizeToolTipText = "Point size";

    _sizeLabel->setToolTip(sizeToolTipText);
    _sizeDoubleSpinBox->setToolTip(sizeToolTipText);
    _sizeSlider->setToolTip(sizeToolTipText);

    const auto opacityToolTipText = "Point opacity";

    _opacityLabel->setToolTip(opacityToolTipText);
    _opacityDoubleSpinBox->setToolTip(opacityToolTipText);
    _opacitySlider->setToolTip(opacityToolTipText);

    addWidget(_popupPushButton);
    addWidget(_widget);
    */
}

void PlotSettingsWidget::setScatterPlotPlugin(const ScatterplotPlugin& plugin)
{
}

QIcon PlotSettingsWidget::getIcon() const
{
    return Application::getIconFont("FontAwesome").getIcon("paint-brush");
}

QLayout* PlotSettingsWidget::getLayout(const hdps::gui::ResponsiveToolBar::WidgetState& state)
{
    return new QHBoxLayout();
}