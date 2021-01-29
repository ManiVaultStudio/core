#include "ScatterplotSettingsWidget.h"

ScatterplotPlugin* ScatterplotSettingsWidget::scatterplotPlugin = nullptr;

ScatterplotSettingsWidget::ScatterplotSettingsWidget(const hdps::gui::ResponsiveToolBar::WidgetState& state, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _state(state)
{
    initializeUI();
    connect();
}