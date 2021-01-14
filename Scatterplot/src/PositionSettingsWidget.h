#pragma once

#include "WidgetStateMixin.h"

#include <QWidget>

class ScatterplotPlugin;

class QLabel;
class QComboBox;

class PositionSettingsWidget : public QWidget, public WidgetStateMixin
{
public:
    PositionSettingsWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

    State getState(const QSize& sourceWidgetSize) const override;

protected:

    void updateState() override;

public:

    int getDimensionX();
    int getDimensionY();

    void setDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());
    void setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());

private:
    QLabel*     _xDimensionLabel;
    QComboBox*  _xDimensionComboBox;
    QLabel*     _yDimensionLabel;
    QComboBox*  _yDimensionComboBox;
};