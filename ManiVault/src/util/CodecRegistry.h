// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include "BlobCodecFactory.h"

namespace mv::gui
{
    class CodecSettingsAction;
}

namespace mv::util {

class CodecRegistry
{
public:
    static CodecRegistry& instance();

    void registerFactory(std::unique_ptr<BlobCodecFactory> factory);

    bool isRegistered(BlobCodec::Type type) const;

    bool isRegistered(const QString& key) const;

    const BlobCodecFactory& factory(BlobCodec::Type type) const;

    gui::CodecSettingsAction* createDefaultSettings(BlobCodec::Type type, QObject* parent = nullptr) const;
    gui::CodecSettingsAction* createSettingsFromVariantMap(BlobCodec::Type type, const QVariantMap& map, QObject* parent = nullptr) const;

    std::unique_ptr<BlobCodec> createCodec(const gui::CodecSettingsAction& codecSettingsAction) const;

private:
    std::unordered_map<BlobCodec::Type, std::unique_ptr<BlobCodecFactory>> _factoriesByType;
};

}