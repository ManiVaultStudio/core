#pragma once

#include "widgets/WidgetState.h"
#include "widgets/PopupPushButton.h"

#include <QStackedWidget>

class ScatterplotPlugin;
class PixelSelectionTool;

class QHBoxLayout;
class QLabel;
class QComboBox;
class QPushButton;
class QDoubleSpinBox;
class QSlider;
class QCheckBox;

class SelectionSettingsWidget : public QStackedWidget
{
public:
    SelectionSettingsWidget(QWidget* parent = nullptr);

    void initializeUI();
    void setScatterPlotPlugin(const ScatterplotPlugin& plugin);

    QSize sizeHint() const override {
        return currentWidget()->sizeHint();
    }

    QSize minimumSizeHint() const override {
        return currentWidget()->minimumSizeHint();
    }

private:
    hdps::gui::WidgetState          _widgetState;
    hdps::gui::PopupPushButton*     _popupPushButton;
    QWidget*                        _widget;
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
    QPushButton*                    _advancedSettingsPushButton;
};