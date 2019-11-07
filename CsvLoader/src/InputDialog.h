#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
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
    void closeDialog(QString dataSetName, bool hasHeaders);

public slots:
    void closeDialogAction();

private:
    QLineEdit* _dataNameInput;
    QCheckBox* _headerCheckbox;

    QPushButton* loadButton;
};
