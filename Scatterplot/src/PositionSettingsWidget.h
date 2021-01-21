#pragma once

#include "widgets/WidgetState.h"
#include "widgets/PopupPushButton.h"

#include <QStackedWidget>

class ScatterplotPlugin;

class QLabel;
class QComboBox;

class PositionSettingsWidget : public QStackedWidget
{
public:
    PositionSettingsWidget(QWidget* parent = nullptr);

    void initializeUI();
    void setScatterPlotPlugin(const ScatterplotPlugin& plugin);

    QSize sizeHint() const override {
        return currentWidget()->sizeHint();
    }

    QSize minimumSizeHint() const override {
        return currentWidget()->minimumSizeHint();
    }

public:

    int getDimensionX();
    int getDimensionY();

    void setDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());

private:
    hdps::gui::WidgetState          _widgetState;
    hdps::gui::PopupPushButton*     _popupPushButton;
    QWidget*                        _widget;
    QLabel*                         _xDimensionLabel;
    QComboBox*                      _xDimensionComboBox;
    QLabel*                         _yDimensionLabel;
    QComboBox*                      _yDimensionComboBox;
};