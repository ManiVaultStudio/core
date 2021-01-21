#pragma once

#include "widgets/WidgetState.h"
#include "widgets/PopupPushButton.h"

#include <QStackedWidget>

class ScatterplotPlugin;

class QPushButton;
class QCheckBox;

class SubsetSettingsWidget : public QStackedWidget
{
public:
    SubsetSettingsWidget(QWidget* parent = nullptr);

    void initializeUI();
    void setScatterPlotPlugin(const ScatterplotPlugin& plugin);

private:
    hdps::gui::WidgetState          _widgetState;
    hdps::gui::PopupPushButton*     _popupPushButton;
    QWidget*                        _widget;
    QPushButton*                    _createSubsetPushButton;
    QCheckBox*                      _fromSourceCheckBox;
};