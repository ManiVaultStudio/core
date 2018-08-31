#pragma once

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

/**
 * Widget containing checkboxes for enabling/disabling certain dimensions of the data.
 */
struct DimensionPickerWidget : QWidget
{
    DimensionPickerWidget()
    {
        setMinimumHeight(100);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

        setLayout(&_layout);
    }

    /**
     * Returns a list of booleans that represent whether the dimension
     * at that index is enabled or not.
     *
     * @return List of enabled dimensions
     */
    std::vector<bool> getEnabledDimensions() const;

    /**
     * Adds check boxes to the widgets for every dimension.
     * Names for the dimensions can be provided. If no names are provided
     * the dimensions will be named according to their index.
     *
     * @param numDimensions Number of checkboxes to add
     * @param names         Names of the dimensions, can be an empty vector
     */
    void setDimensions(unsigned int numDimensions, std::vector<QString> names);

private:
    void clearWidget();

    std::vector<QCheckBox*> _checkBoxes;
    QGridLayout _layout;
};


/**
 * Main settings widget
 */
class TsneSettingsWidget : public SettingsWidget
{
    Q_OBJECT

public:
    TsneSettingsWidget();

    std::vector<bool> getEnabledDimensions();
    bool hasValidSettings();

    QString currentData();
    void onNumDimensionsChanged(TsneAnalysisPlugin* analysis, unsigned int numDimensions, std::vector<QString> names);
private:
    void checkInputStyle(QLineEdit& input);

signals:
    void startComputation();
    void stopComputation();
    void dataSetPicked(QString);

private slots:
    void onStartToggled(bool pressed);
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
