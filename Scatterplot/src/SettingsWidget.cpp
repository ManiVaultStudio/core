#include "SettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QPainter>

#include "ui_SettingsWidget.h"

const hdps::Vector3f SettingsWidget::DEFAULT_BASE_COLOR = hdps::Vector3f(255.f / 255, 99.f / 255, 71.f / 255);
const hdps::Vector3f SettingsWidget::DEFAULT_SELECTION_COLOR = hdps::Vector3f(72.f / 255, 61.f / 255, 139.f / 255);

SettingsWidget::SettingsWidget(const ScatterplotPlugin& plugin) :
    QWidget(),
    _baseColor(DEFAULT_BASE_COLOR),
    _selectionColor(DEFAULT_SELECTION_COLOR),
    _ui{ std::make_unique<Ui::SettingsWidget>() }
{
    _ui->setupUi(this);

    _ui->renderModeWidget->initialize(plugin);
    _ui->plotSettingsStackedWidget->initialize(plugin);
    _ui->dimensionPickerWidget->initialize(plugin);
}

SettingsWidget::~SettingsWidget()
{
}

int SettingsWidget::getXDimension()
{
    return _ui->dimensionPickerWidget->getDimensionX();
}

int SettingsWidget::getYDimension()
{
    return _ui->dimensionPickerWidget->getDimensionY();
}

hdps::Vector3f SettingsWidget::getBaseColor()
{
    return _baseColor;
}

hdps::Vector3f SettingsWidget::getSelectionColor()
{
    return _selectionColor;
}

void SettingsWidget::initDimOptions(const unsigned int nDim)
{
    _ui->dimensionPickerWidget->setDimensions(nDim);
}

void SettingsWidget::initDimOptions(const std::vector<QString>& dimNames)
{
    _ui->dimensionPickerWidget->setDimensions(dimNames.size(), dimNames);
}

void SettingsWidget::initScalarDimOptions(const unsigned int nDim)
{
    _ui->dimensionPickerWidget->setScalarDimensions(nDim);
}

void SettingsWidget::initScalarDimOptions(const std::vector<QString>& dimNames)
{
    _ui->dimensionPickerWidget->setScalarDimensions(dimNames.size(), dimNames);
}

void SettingsWidget::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}