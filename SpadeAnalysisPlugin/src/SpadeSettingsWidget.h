#ifndef CLUSTERING_SETTINGS_WIDGET_H
#define CLUSTERING_SETTINGS_WIDGET_H

#include <widgets/SettingsWidget.h>

#include <QObject>
#include <QComboBox>
#include <QPushButton>

using namespace hdps::gui;

class ClusteringPlugin;

class ClusteringSettingsWidget : public SettingsWidget
{
    Q_OBJECT

public:
    ClusteringSettingsWidget(const ClusteringPlugin* analysis);

public:
    QComboBox dataOptions;
    QPushButton startButton;
};

#endif // CLUSTERING_SETTINGS_WIDGET_H
