#include "SettingsWidget.h"
#include "ScatterplotPlugin.h"

#include "widgets/ResponsiveToolBar.h"

#include <QPushButton>
#include <QDialog>
#include <QPropertyAnimation>

using namespace hdps::gui;

const hdps::Vector3f SettingsWidget::DEFAULT_BASE_COLOR = hdps::Vector3f(255.f / 255, 99.f / 255, 71.f / 255);
const hdps::Vector3f SettingsWidget::DEFAULT_SELECTION_COLOR = hdps::Vector3f(72.f / 255, 61.f / 255, 139.f / 255);

SettingsWidget::SettingsWidget(const ScatterplotPlugin& plugin) :
    QWidget(static_cast<QWidget*>(&const_cast<ScatterplotPlugin&>(plugin))),
    _responsiveToolBar(new ResponsiveToolBar(this)),
    _baseColor(DEFAULT_BASE_COLOR),
    _selectionColor(DEFAULT_SELECTION_COLOR)
{
    setAutoFillBackground(true);

    auto horizontalLayout = new QHBoxLayout();

    horizontalLayout->setMargin(5);
    horizontalLayout->setSpacing(0);
    horizontalLayout->addWidget(_responsiveToolBar);

    setLayout(horizontalLayout);

    //setEnabled(false);

    connect(&plugin, &ScatterplotPlugin::currentDatasetChanged, [this](const QString& currentDataset) {
        setEnabled(!currentDataset.isEmpty());
    });
}

hdps::Vector3f SettingsWidget::getBaseColor()
{
    return _baseColor;
}

hdps::Vector3f SettingsWidget::getSelectionColor()
{
    return _selectionColor;
}

void SettingsWidget::initScalarDimOptions(const unsigned int nDim)
{
    //_colorSettingsWidget->setScalarDimensions(nDim);
}

void SettingsWidget::initScalarDimOptions(const std::vector<QString>& dimNames)
{
    //_colorSettingsWidget->setScalarDimensions(dimNames.size(), dimNames);
}