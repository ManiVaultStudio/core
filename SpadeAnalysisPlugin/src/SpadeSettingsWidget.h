#ifndef SPADE_SETTINGS_WIDGET_H
#define SPADE_SETTINGS_WIDGET_H

#include <widgets/SettingsWidget.h>

#include <QObject>
#include <QComboBox>
#include <QPushButton>

using namespace hdps::gui;

class SpadeAnalysisPlugin;

class SpadeSettingsWidget : public SettingsWidget
{
    Q_OBJECT

public:
    SpadeSettingsWidget(const SpadeAnalysisPlugin* analysis);

public:
    QComboBox dataOptions;
    QPushButton startButton;
};

#endif // SPADE_SETTINGS_WIDGET_H
