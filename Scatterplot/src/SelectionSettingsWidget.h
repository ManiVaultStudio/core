#pragma once

#include "ScatterplotSettingsWidget.h"

class ScatterplotPlugin;
class PixelSelectionTool;

class QHBoxLayout;
class QLabel;
class QComboBox;
class QPushButton;
class QDoubleSpinBox;
class QSlider;
class QCheckBox;

class SelectionSettingsWidget : public ScatterplotSettingsWidget
{
public:
    SelectionSettingsWidget(const hdps::gui::ResponsiveToolBar::WidgetState& state, QWidget* parent = nullptr);

    void initializeUI();
    void connectToPlugin();

private:
    QLabel*                         _typeLabel;
    QWidget*                        _typeWidget;
    QHBoxLayout*                    _typeLayout;
    QComboBox*                      _typeComboBox;
    QPushButton*                    _modifierAddPushButton;
    QPushButton*                    _modifierRemovePushButton;
    QLabel*                         _radiusLabel;
    QWidget*                        _radiusWidget;
    QHBoxLayout*                    _radiusLayout;
    QDoubleSpinBox*                 _radiusDoubleSpinBox;
    QSlider*                        _radiusSlider;
    QLabel*                         _selectLabel;
    QWidget*                        _selectWidget;
    QHBoxLayout*                    _selectLayout;
    QPushButton*                    _clearSelectionPushButton;
    QPushButton*                    _selectAllPushButton;
    QPushButton*                    _invertSelectionPushButton;
    QCheckBox*                      _notifyDuringSelectionCheckBox;
    hdps::gui::PopupPushButton*     _additionalSettingsPopupPushButton;
};