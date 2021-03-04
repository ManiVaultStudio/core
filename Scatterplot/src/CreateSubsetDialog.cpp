#include "SanitizeDataDialog.h"
#include "ImageCollection.h"

#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QPushButton>

SanitizeDataDialog::SanitizeDataDialog(ImageCollection* imageCollection, std::vector<float>& data, QWidget* parent /*= nullptr*/) :
    QDialog(parent)
{
    setWindowTitle(QString("Found NaN values in: %1").arg(imageCollection->getDatasetName(Qt::EditRole).toString()));
    setWindowIcon(hdps::Application::getIconFont("FontAwesome").getIcon("exclamation-circle"));
    
    auto layout = new QGridLayout();

    setLayout(layout);

    auto label = new QLabel("Replace NaN with:");

    label->setToolTip("Replace NaN values with real value");
    label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    layout->addWidget(label, 0, 0);

    auto replacementValueLayout = new QHBoxLayout();

    auto doubleSpinBox = new QDoubleSpinBox();

    doubleSpinBox->setToolTip("Replace NaN values with real value");
    doubleSpinBox->setRange(std::numeric_limits<float>::min(), std::numeric_limits<float>::max());
    doubleSpinBox->setValue(0.0);

    layout->addWidget(doubleSpinBox, 0, 1);

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    dialogButtonBox->button(QDialogButtonBox::Ok)->setText("&Replace");
    dialogButtonBox->button(QDialogButtonBox::Cancel)->setText("&Don't replace");

    layout->addWidget(dialogButtonBox, 1, 0, 1, 2);

    connect(dialogButtonBox, &QDialogButtonBox::accepted, [this, doubleSpinBox, &data]() {
        replaceNans(data, static_cast<float>(doubleSpinBox->value()));
        accept();
    });

    connect(dialogButtonBox, &QDialogButtonBox::rejected, [this, doubleSpinBox, &data]() {
        reject();
    });
}

void SanitizeDataDialog::replaceNans(std::vector<float>& data, const float& replacementValue /*= 0.0f*/)
{
    qDebug() << QString("Replacing NaN values with: %1").arg(QString::number(replacementValue, 'f', 2));

    for (auto& element : data)
        if (isnan(element))
            element = static_cast<float>(replacementValue);
}