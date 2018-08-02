#ifndef TSNE_SETTINGS_WIDGET_H
#define TSNE_SETTINGS_WIDGET_H

#include <widgets/SettingsWidget.h>

#include <QObject>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>

#include <vector>

using namespace hdps::gui;

class TsneAnalysisPlugin;

struct DimensionPickerWidget : QWidget
{
    DimensionPickerWidget()
    {
        setLayout(&_layout);
    }

    std::vector<bool> getEnabledDimensions()
    {
        std::vector<bool> enabledDimensions;

        for (QCheckBox* checkBox : _checkBoxes)
        {
            enabledDimensions.push_back(checkBox->isChecked());
        }

        return enabledDimensions;
    }

    void setDimensions(unsigned int numDimensions, std::vector<QString> names)
    {
        bool hasNames = names.size() == numDimensions;

        for (QCheckBox* widget : _checkBoxes)
        {
            _layout.removeWidget(widget);
            delete widget;
        }

        _checkBoxes.clear();
        for (int i = 0; i < numDimensions; i++)
        {
            QString name = hasNames ? names[i] : QString("Dim ") + QString::number(i);
            QCheckBox* widget = new QCheckBox(name);
            widget->setChecked(true);

            _checkBoxes.push_back(widget);
            int row = i % (numDimensions / 2);
            int column = i / (numDimensions / 2);
            _layout.addWidget(widget, row, column);
        }
    }
private:
    std::vector<QCheckBox*> _checkBoxes;

    QGridLayout _layout;
};

class TsneSettingsWidget : public SettingsWidget
{
    Q_OBJECT

public:
    TsneSettingsWidget(const TsneAnalysisPlugin* analysis);

    std::vector<bool> getEnabledDimensions();
    bool hasValidSettings();

    void onNumDimensionsChanged(TsneAnalysisPlugin* analysis, unsigned int numDimensions, std::vector<QString> names);
private:
    void checkInputStyle(QLineEdit& input);

private slots:
    void numIterationsChanged(const QString &value);
    void perplexityChanged(const QString &value);
    void exaggerationChanged(const QString &value);
    void expDecayChanged(const QString &value);
    void numTreesChanged(const QString &value);
    void numChecksChanged(const QString &value);
    void thetaChanged(const QString& value);

public:
    DimensionPickerWidget _dimensionPickerWidget;

    QComboBox dataOptions;
    QLineEdit numIterations;
    QLineEdit perplexity;
    QLineEdit exaggeration;
    QLineEdit expDecay;
    QLineEdit numTrees;
    QLineEdit numChecks;
    QLineEdit theta;
    QPushButton startButton;
};

#endif // TSNE_SETTINGS_WIDGET_H
