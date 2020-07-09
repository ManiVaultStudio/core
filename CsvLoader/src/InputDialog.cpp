#include "InputDialog.h"
#include "PointData.h"

InputDialog::InputDialog(QWidget* parent) :
    QDialog(parent)
{
    setWindowTitle(tr("CSV Loader"));

    _dataNameInput = new QLineEdit();
    _dataNameInput->setText("Dataset");

    _headerCheckbox = new QCheckBox("Has Headers");


    for (const char* const typeName : PointData::getElementTypeNames())
    {
        _dataTypeComboBox->addItem(QString::fromLatin1(typeName));
    }
    loadButton = new QPushButton(tr("Load file"));
    loadButton->setDefault(true);

    connect(loadButton, &QPushButton::pressed, this, &InputDialog::closeDialogAction);
    connect(this, &InputDialog::closeDialog, this, &QDialog::accept);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(_dataNameInput);
    layout->addWidget(_headerCheckbox);
    auto label = std::make_unique<QLabel>(tr("&Store as: "));
    label->setBuddy(_dataTypeComboBox);
    layout->addWidget(label.release());
    layout->addWidget(_dataTypeComboBox);
    layout->addWidget(loadButton);
    setLayout(layout);
}

void InputDialog::closeDialogAction()
{
    bool hasHeaders = _headerCheckbox->isChecked();
    emit closeDialog(_dataNameInput->text(), hasHeaders, _dataTypeComboBox->currentText());
}
