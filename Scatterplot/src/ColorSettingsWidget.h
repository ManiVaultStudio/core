#pragma once

#include "widgets/ResponsiveToolBar.h"

class ScatterplotPlugin;

class QLabel;
class QComboBox;

class ColorSettingsWidget : public hdps::gui::ResponsiveToolBar::Widget
{
public:
    class ColorDimensionWidget : public QWidget {
    public:
        ColorDimensionWidget() :
            QWidget(),
            _layout(new QHBoxLayout())
        {
            setLayout(_layout);

        }

    protected:
        QHBoxLayout*    _layout;
        QComboBox*      _comboBox;

        friend class ColorSettingsWidget;
    };

public:
    ColorSettingsWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

    void updateState() override;

public:

    void setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());

private:
    QLabel*         _colorByLabel;
    QComboBox*      _colorByComboBox;
    QComboBox*      _colorDimensionComboBox;
};