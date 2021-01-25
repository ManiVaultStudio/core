#pragma once

#include "widgets/ResponsiveToolBar.h"

class ScatterplotPlugin;

class QLabel;
class QComboBox;
class QHBoxLayout;
class QLineEdit;
class QPushButton;

class ColorSettingsWidget : public hdps::gui::ResponsiveToolBar::StatefulWidget
{
public:
    class ColorDimensionWidget : public QWidget {
    public:
        ColorDimensionWidget();

        QComboBox* getColorDimensionComboBox();

    protected:
        QComboBox*      _colorDimensionComboBox;
        QHBoxLayout*    _layout;
        QLineEdit*      _colorDataLineEdit;
        QPushButton*    _removeColorDataPushButton;

        friend class ColorSettingsWidget;
    };

public:
    ColorSettingsWidget(QWidget* parent = nullptr);

    void initializeUI();
    void setScatterPlotPlugin(const ScatterplotPlugin& plugin);

public:
    void setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());

protected:
    QLayout* getLayout(const hdps::gui::ResponsiveToolBar::WidgetState& state) override;

private:
    QLabel*                 _colorByLabel;
    QComboBox*              _colorByComboBox;
    ColorDimensionWidget*   _colorDimensionWidget;
};