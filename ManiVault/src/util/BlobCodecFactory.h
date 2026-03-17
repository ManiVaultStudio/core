// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include "BlobCodec.h"

#include <QString>

namespace mv::gui
{
	class CodecSettingsAction;
}

namespace mv::util {

class BlobCodecSettings;

class CORE_EXPORT BlobCodecFactory : public QObject
{
public:

    BlobCodecFactory(QObject* parent);
    ~BlobCodecFactory() override = default;

    virtual BlobCodec::Type type() const = 0;
    virtual QString key() const = 0;
    virtual QString displayName() const = 0;

    virtual gui::CodecSettingsAction* createSettingsFromVariantMap(const QVariantMap& map, QObject* parent = nullptr) const = 0;

    virtual std::unique_ptr<BlobCodec> createCodec(gui::CodecSettingsAction* codecSettingsAction = nullptr) const = 0;

    /**
     * Get default codec settings action for this codec (returns nullptr if no settings are needed)
     * @return Default codec settings action for this codec (returns nullptr if no settings are needed)
     */
    virtual const gui::CodecSettingsAction* getDefaultCodecSettingsAction() const = 0;
};

}
