// SPDX-License-Identifier: LGPL-3.0-or-later

#include "StorageBackendConfigurationDialog.h"

#include <util/StyledIcon.h>

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

#include <utility>

StorageBackendConfigurationDialog::StorageBackendConfigurationDialog(std::unique_ptr<mv::gui::StorageBackendConversionAction> conversionAction, const bool updateMode, QWidget* parent) :
    StorageBackendConfigurationDialog(std::move(conversionAction), updateMode ? Purpose::Configure : Purpose::Convert, parent)
{
}

StorageBackendConfigurationDialog::StorageBackendConfigurationDialog(std::unique_ptr<mv::gui::StorageBackendConversionAction> conversionAction, const Purpose purpose, QWidget* parent) :
    QDialog(parent),
    _conversionAction(std::move(conversionAction))
{
    setWindowIcon(mv::util::StyledIcon("gear"));

    auto backendName = tr("Storage");

    if (_conversionAction)
    {
        const auto backendType = _conversionAction->getTargetBackendType();
        backendName = tr("%1 Storage").arg(mv::pointdata::storage::getStorageBackendTypeName(backendType));

        if (backendType == mv::pointdata::storage::StorageBackendType::Eager)
            backendName = tr("In-Memory Storage");
    }

    switch (purpose)
    {
        case Purpose::Configure:
            setWindowTitle(tr("Configure %1").arg(backendName));
            break;

        case Purpose::Convert:
            setWindowTitle(tr("Convert to %1").arg(backendName));
            break;

        case Purpose::Import:
            setWindowTitle(tr("Configure %1 for Import").arg(backendName));
            break;
    }

    resize(400, 500);

    auto* layout = new QVBoxLayout(this);

    if (_conversionAction)
        layout->addWidget(_conversionAction->createWidget(this));

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    if (purpose == Purpose::Configure)
    {
        auto* updateButton = buttons->button(QDialogButtonBox::Ok);
        updateButton->setText(tr("Update"));
        updateButton->setEnabled(false);

        connect(_conversionAction.get(), &mv::gui::StorageBackendConversionAction::optionsChanged, updateButton, [updateButton] {
            updateButton->setEnabled(true);
        });
    }
    else if (_conversionAction)
    {
        auto* convertButton = buttons->button(QDialogButtonBox::Ok);
        convertButton->setText(tr("Convert"));
        convertButton->setEnabled(_conversionAction->canApply());

        connect(_conversionAction.get(), &mv::gui::StorageBackendConversionAction::optionsChanged, convertButton, [this, convertButton] {
            convertButton->setEnabled(_conversionAction->canApply());
        });
    }

    layout->addStretch(1);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

mv::pointdata::storage::StorageBackendConfigurationOptionsPtr StorageBackendConfigurationDialog::getConversionOptions() const
{
    return _conversionAction ? _conversionAction->getConversionOptions() : nullptr;
}
