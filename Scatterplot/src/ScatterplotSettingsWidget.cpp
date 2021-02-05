#include "ScatterplotSettingsWidget.h"
#include "ScatterplotPlugin.h"

ScatterplotSettingsWidget::ScatterplotSettingsWidget(const hdps::gui::ResponsiveSectionWidget::WidgetState& widgetState, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _widgetState(widgetState)
{
}

ScatterplotWidget* ScatterplotSettingsWidget::getScatterplotWidget()
{
    Q_ASSERT(_scatterplotPlugin != nullptr);

    return _scatterplotPlugin->getScatterplotWidget();
}

void ScatterplotSettingsWidget::applyLayout(QLayout* layout)
{
    layout->setMargin(0);
    layout->setSpacing(4);

    setLayout(layout);
}

bool ScatterplotSettingsWidget::isForm() const
{
    return _widgetState.testFlag(ResponsiveSectionWidget::WidgetStateFlag::Form);
}

bool ScatterplotSettingsWidget::isSequential() const
{
    return _widgetState.testFlag(ResponsiveSectionWidget::WidgetStateFlag::Sequential);
}

bool ScatterplotSettingsWidget::isCompact() const
{
    return _widgetState.testFlag(static_cast<ResponsiveSectionWidget::WidgetStateFlag>(ResponsiveSectionWidget::State::Compact));
}

bool ScatterplotSettingsWidget::isFull() const
{
    return _widgetState.testFlag(static_cast<ResponsiveSectionWidget::WidgetStateFlag>(ResponsiveSectionWidget::State::Full));
}