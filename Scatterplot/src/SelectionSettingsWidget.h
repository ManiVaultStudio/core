#pragma once

#include "WidgetStateMixin.h"

#include <QWidget>

class ScatterplotPlugin;
class PixelSelectionTool;

class QHBoxLayout;
class QLabel;
class QComboBox;
class QPushButton;
class QDoubleSpinBox;
class QSlider;
class QCheckBox;

class SelectionSettingsWidget : public QWidget, public WidgetStateMixin
{
public:

    SelectionSettingsWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

    State getState(const QSize& sourceWidgetSize) const override;

protected:

    void updateState() override;

private:
    QLabel*             _typeLabel;
    QWidget*            _typeWidget;
    QHBoxLayout*        _typeLayout;
    QComboBox*          _typeComboBox;
    QPushButton*        _modifierAddPushButton;
    QPushButton*        _modifierRemovePushButton;
    QLabel*             _radiusLabel;
    QWidget*            _radiusWidget;
    QHBoxLayout*        _radiusLayout;
    QDoubleSpinBox*     _radiusDoubleSpinBox;
    QSlider*            _radiusSlider;
    QLabel*             _selectLabel;
    QWidget*            _selectWidget;
    QHBoxLayout*        _selectLayout;
    QPushButton*        _clearSelectionPushButton;
    QPushButton*        _selectAllPushButton;
    QPushButton*        _invertSelectionPushButton;
    QCheckBox*          _notifyDuringSelectionCheckBox;
};