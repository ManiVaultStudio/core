#include "ScatterplotSettingsWidget.h"
#include "ScatterplotPlugin.h"

ScatterplotSettingsWidget::ScatterplotSettingsWidget(const hdps::gui::ResponsiveSectionWidget::State& state, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _state(state)
{
}

ScatterplotWidget* ScatterplotSettingsWidget::getScatterplotWidget()
{
    Q_ASSERT(_scatterplotPlugin != nullptr);

    return _scatterplotPlugin->getScatterplotWidget();
}