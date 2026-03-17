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

class CORE_EXPORT CodecRegistry
{
public:

    void registerFactory(std::unique_ptr<BlobCodecFactory> factory);

    bool isRegistered(BlobCodec::Type type) const;

    bool isRegistered(const QString& key) const;

    const BlobCodecFactory& factory(BlobCodec::Type type) const;

    const BlobCodecFactory& factory(const QString& key) const;

    gui::CodecSettingsAction* createSettingsFromVariantMap(BlobCodec::Type type, const QVariantMap& map, QObject* parent = nullptr) const;

    std::unique_ptr<BlobCodec> createCodec(const mv::gui::CodecSettingsAction* codecSettingsAction = nullptr) const;
    std::unique_ptr<BlobCodec> createCodec(BlobCodec::Type type) const;
    std::unique_ptr<BlobCodec> createCodec(const QString& typeName) const;

    std::vector<BlobCodec::Type> availableTypes() const;
    QStringList availableTypeNames() const;

    CodecRegistry(const CodecRegistry&) = delete;
    CodecRegistry& operator=(const CodecRegistry&) = delete;

    CodecRegistry(CodecRegistry&&) = delete;
    CodecRegistry& operator=(CodecRegistry&&) = delete;

protected:

    CodecRegistry();

    std::vector<std::unique_ptr<BlobCodecFactory>>          _factoriesOwned;
    std::unordered_map<BlobCodec::Type, BlobCodecFactory*>  _factoriesByType;
    QHash<QString, BlobCodecFactory*>                       _factoriesByKey;

    friend class Application;
};

CORE_EXPORT CodecRegistry& codecRegistry();

}