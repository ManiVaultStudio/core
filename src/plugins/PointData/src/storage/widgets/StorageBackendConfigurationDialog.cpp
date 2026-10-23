// SPDX-License-Identifier: LGPL-3.0-or-later

#include "StorageBackendConfigurationDialog.h"

#include <util/StyledIcon.h>

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

#include <utility>

StorageBackendConfigurationDialog::StorageBackendConfigurationDialog(std::unique_ptr<mv::gui::StorageBackendConversionAction> conversionAction, const bool updateMode, QWidget* parent) :
    QDialog(parent),
    _conversionAction(std::move(conversionAction))
{
    setWindowIcon(mv::util::StyledIcon("shuffle"));
    setWindowTitle(tr("Configure point-data conversion"));
    resize(400, 500);

    auto* layout = new QVBoxLayout(this);

    if (_conversionAction)
        layout->addWidget(_conversionAction->createWidget(this));

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    if (updateMode)
    {
        auto* updateButton = buttons->button(QDialogButtonBox::Ok);
        updateButton->setText(tr("Update"));
        updateButton->setEnabled(false);

        connect(_conversionAction.get(), &mv::gui::StorageBackendConversionAction::optionsChanged, updateButton, [updateButton] {
            updateButton->setEnabled(true);
        });
    }

    layout->addStretch(1);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

mv::pointdata::storage::StorageBackendConversionOptionsPtr StorageBackendConfigurationDialog::getConversionOptions() const
{
    return _conversionAction ? _conversionAction->getConversionOptions() : nullptr;
}
