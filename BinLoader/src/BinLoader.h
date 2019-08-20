#pragma once

#include <LoaderPlugin.h>

#include <QDialog>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QLineEdit>

using namespace hdps::plugin;

// =============================================================================
// Loading input box
// =============================================================================

enum BinaryDataType
{
    FLOAT, UBYTE
};

class BinLoadingInputDialog : public QDialog
{
    Q_OBJECT
public:
    BinLoadingInputDialog(QWidget* parent) :
        QDialog(parent)
    {
        setWindowTitle(tr("Binary Loader"));

        dataNameInput = new QLineEdit();
        dataNameInput->setText("Dataset");

        dataTypeInput = new QComboBox();
        dataTypeInput->addItem("Float");
        dataTypeInput->addItem("Unsigned Byte");

        numDimsInput = new QSpinBox();
        numDimsInput->setMinimum(1);
        numDimsInput->setMaximum(INT_MAX);
        numDimsInput->setValue(1);
        numDimsLabel = new QLabel(tr("Number of dimensions:"));
        numDimsLabel->setBuddy(numDimsInput);

        loadButton = new QPushButton(tr("Load file"));
        loadButton->setDefault(true);

        connect(loadButton, &QPushButton::pressed, this, &BinLoadingInputDialog::closeDialogAction);
        connect(this, &BinLoadingInputDialog::closeDialog, this, &QDialog::accept);

        QHBoxLayout *layout = new QHBoxLayout();
        layout->addWidget(dataNameInput);
        layout->addWidget(numDimsLabel);
        layout->addWidget(numDimsInput);
        layout->addWidget(dataTypeInput);
        layout->addWidget(loadButton);
        setLayout(layout);
    }

signals:
    void closeDialog(unsigned int numDimensions, BinaryDataType dataType, QString dataSetName);

public slots:
    void closeDialogAction()
    {
        BinaryDataType dataType;
        if (dataTypeInput->currentText() == "Float")
            dataType = BinaryDataType::FLOAT;
        else if (dataTypeInput->currentText() == "Unsigned Byte")
            dataType = BinaryDataType::UBYTE;

        emit closeDialog(numDimsInput->value(), dataType, dataNameInput->text());
    }

private:
    QLineEdit* dataNameInput;
    QComboBox* dataTypeInput;
    QLabel* numDimsLabel;
    QSpinBox* numDimsInput;

    QPushButton* loadButton;
};

// =============================================================================
// View
// =============================================================================

class BinLoader : public QObject, public LoaderPlugin
{
    Q_OBJECT
public:
    BinLoader() : LoaderPlugin("BIN Loader") { }
    ~BinLoader(void) override;
    
    void init() override;

    void loadData() Q_DECL_OVERRIDE;

public slots:
    void dialogClosed(unsigned int numDimensions, BinaryDataType dataType, QString dataSetName);

private:
    unsigned int _numDimensions;
    BinaryDataType _dataType;
    QString _dataSetName;
};


// =============================================================================
// Factory
// =============================================================================

class BinLoaderFactory : public LoaderPluginFactory
{
    Q_INTERFACES(hdps::plugin::LoaderPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.BinLoader"
                      FILE  "BinLoader.json")
    
public:
    BinLoaderFactory(void) {}
    ~BinLoaderFactory(void) override {}
    
    LoaderPlugin* produce() override;
};
