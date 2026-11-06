// SPDX-License-Identifier: LGPL-3.0-or-later

#include "StorageBackendConfigurationDialog.h"

#include <util/StyledIcon.h>

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSizePolicy>
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

        const auto settingsKey = QStringLiteral("PointData/StorageBackend/%1/Presets").arg(static_cast<int>(backendType));
        _presetsAction = std::make_unique<mv::gui::PresetsAction>(nullptr, _conversionAction.get(), settingsKey, tr("Point data storage backend"), mv::util::StyledIcon("prescription"));

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

    resize(650, 650);
    setFixedWidth(650);

    auto* layout = new QVBoxLayout(this);

    if (_conversionAction)
    {
        auto* conversionWidget = _conversionAction->createWidget(this);
        conversionWidget->setMinimumWidth(0);
        conversionWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
        layout->addWidget(conversionWidget);
    }

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    if (purpose == Purpose::Configure)
    {
        auto* updateButton = buttons->button(QDialogButtonBox::Ok);
        updateButton->setText(tr("Update"));
        updateButton->setEnabled(_conversionAction && _conversionAction->canApply() && _conversionAction->hasChanges());

        connect(_conversionAction.get(), &mv::gui::StorageBackendConversionAction::optionsChanged, updateButton, [this, updateButton] {
            updateButton->setEnabled(_conversionAction->canApply() && _conversionAction->hasChanges());
        });
    }
    else if (_conversionAction)
    {
        auto* applyButton = buttons->button(QDialogButtonBox::Ok);
        if (purpose == Purpose::Convert)
            applyButton->setText(tr("Convert"));
        applyButton->setEnabled(_conversionAction->canApply());

        connect(_conversionAction.get(), &mv::gui::StorageBackendConversionAction::optionsChanged, applyButton, [this, applyButton] {
            applyButton->setEnabled(_conversionAction->canApply());
        });
    }

    auto* buttonLayout = new QHBoxLayout();
    if (_presetsAction)
    {
        auto* presetsButton = new QPushButton(tr("Presets"), this);
        presetsButton->setMenu(_presetsAction->getMenu(presetsButton));
        buttonLayout->addWidget(presetsButton);
    }
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(buttons);
    layout->addLayout(buttonLayout);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

mv::pointdata::storage::StorageBackendConfigurationOptionsPtr StorageBackendConfigurationDialog::getConversionOptions() const
{
    return _conversionAction ? _conversionAction->getConversionOptions() : nullptr;
}
