#pragma once

#include "WidgetStateMixin.h"

#include <QWidget>

class ScatterplotPlugin;

class QPushButton;
class QCheckBox;

class SubsetSettingsWidget : public QWidget, public WidgetStateMixin
{
public:
    SubsetSettingsWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

    State getState(const QSize& sourceWidgetSize) const override;

protected:

    void updateState() override;

private:
    QPushButton*    _createSubsetPushButton;
    QCheckBox*      _fromSourceCheckBox;
};