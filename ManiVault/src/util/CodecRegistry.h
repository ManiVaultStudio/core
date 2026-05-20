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

    SharedCodec createCodec(QObject* parent, BlobCodec::Type type) const;
    SharedCodec createCodec(QObject* parent, mv::gui::CodecSettingsAction* codecSettingsAction = nullptr) const;
    SharedCodec createCodec(QObject* parent, const QString& typeName) const;

    std::vector<BlobCodec::Type> availableTypes() const;
    QStringList availableTypeNames() const;
    QStringList availableTypeDisplayNames() const;
    BlobCodec::Type typeFromDisplayName(const QString& displayName) const;

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