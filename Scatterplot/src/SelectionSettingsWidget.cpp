#include "SelectionSettingsWidget.h"
#include "SelectionSettingsPopupWidget.h"
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
    //resize(QSize(10, 10));

    setFixedHeight(32);
    setFixedWidth(32);

    auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _ui->popupPushButton->setStyleSheet("text-align: center");
    _ui->popupPushButton->setFont(fontAwesome.getFont(8));
    _ui->popupPushButton->setText(fontAwesome.getIconCharacter("mouse-pointer"));

    const auto showSelectionSettingsPopupWidget = [this, &plugin](QWidget* parent) {
        auto windowLevelWidget = new SelectionSettingsPopupWidget(parent);

        windowLevelWidget->initialize(plugin);
        windowLevelWidget->show();
    };

    QObject::connect(_ui->popupPushButton, &QPushButton::clicked, [this, &plugin, showSelectionSettingsPopupWidget]() {
        showSelectionSettingsPopupWidget(_ui->popupPushButton);
    });
}