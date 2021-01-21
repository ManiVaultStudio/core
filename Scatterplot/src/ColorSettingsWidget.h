#pragma once

#include "widgets/WidgetState.h"
#include "widgets/PopupPushButton.h"

#include <QStackedWidget>

class ScatterplotPlugin;

class QLabel;
class QComboBox;
class QHBoxLayout;
class QLineEdit;
class QPushButton;

class ColorSettingsWidget : public QStackedWidget
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

    QSize sizeHint() const override {
        return currentWidget()->sizeHint();
    }

    QSize minimumSizeHint() const override {
        return currentWidget()->minimumSizeHint();
    }

public:
    void setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());

private:
    hdps::gui::WidgetState          _widgetState;
    hdps::gui::PopupPushButton*     _popupPushButton;
    QWidget*                        _widget;
    QLabel*                         _colorByLabel;
    QComboBox*                      _colorByComboBox;
    ColorDimensionWidget*           _colorDimensionWidget;
};