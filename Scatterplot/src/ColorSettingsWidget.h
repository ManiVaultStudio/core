#pragma once

#include "widgets/ResponsiveToolBar.h"

#include <QStackedWidget>

class ScatterplotPlugin;

class QLabel;
class QComboBox;
class QLineEdit;
class QPushButton;

class ColorSettingsWidget : public hdps::gui::ResponsiveToolBar::Widget
{
public:
    class ColorDimensionWidget : public QStackedWidget {
    public:
        ColorDimensionWidget();

        QComboBox* getColorDimensionComboBox();

        QSize sizeHint() const override {
            return currentWidget()->sizeHint();
        }

        QSize minimumSizeHint() const override {
            return currentWidget()->minimumSizeHint();
        }

    protected:
        QComboBox*      _colorDimensionComboBox;
        QWidget*        _widget;
        QHBoxLayout*    _layout;
        QLineEdit*      _colorDataLineEdit;
        QPushButton*    _removeColorDataPushButton;

        friend class ColorSettingsWidget;
    };

public:
    ColorSettingsWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

    void updateState() override;

public:

    void setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());

private:
    QLabel*                 _colorByLabel;
    QComboBox*              _colorByComboBox;
    ColorDimensionWidget*   _colorDimensionWidget;
};