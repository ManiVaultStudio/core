#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

// =============================================================================
// Loading input box
// =============================================================================

class InputDialog : public QDialog
{
    Q_OBJECT
public:
    InputDialog(QWidget* parent);

signals:
    void closeDialog(QString dataSetName, bool hasHeaders, QString dataType);

public slots:
    void closeDialogAction();

private:
    QLineEdit* _dataNameInput;
    QCheckBox* _headerCheckbox;
    QComboBox* const _dataTypeComboBox = new QComboBox{};

    QPushButton* loadButton;
};
