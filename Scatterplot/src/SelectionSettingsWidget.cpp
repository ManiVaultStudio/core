#include "SelectionSettingsWidget.h"
#include "ScatterplotPlugin.h"
#include "Application.h"

#include "ui_SelectionSettingsWidget.h"

#include <QDebug>
#include <QDoubleSpinBox>
#include <QCheckBox>

SelectionSettingsWidget::SelectionSettingsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
	_ui{ std::make_unique<Ui::SelectionSettingsWidget>() }
{
    _ui->setupUi(this);
}

void SelectionSettingsWidget::initialize(const ScatterplotPlugin& plugin)
{
    auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _ui->typeComboBox->addItems(QStringList(PixelSelectionTool::types.keys()));

    auto& scatterplotPlugin = const_cast<ScatterplotPlugin&>(plugin);
    auto& pixelSelectionTool = scatterplotPlugin.getSelectionTool();

    QObject::connect(_ui->typeComboBox, &QComboBox::currentTextChanged, [this, &pixelSelectionTool](QString currentText) {
        pixelSelectionTool.setType(PixelSelectionTool::getTypeEnum(currentText));
    });

    const auto updateTypeUI = [this, &scatterplotPlugin]() {
        const auto canSelect = scatterplotPlugin.canSelect();

        _ui->typeLabel->setEnabled(canSelect);
        _ui->typeComboBox->setEnabled(canSelect);
    };
    
    pixelSelectionTool.setChanged();
}