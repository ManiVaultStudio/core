// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "PassthroughBlobCodecFactory.h"
#include "PassthroughBlobCodec.h"
#include "PassthroughCodecSettingsAction.h"

PassthroughBlobCodecFactory::PassthroughBlobCodecFactory() :
    _defaultSettingsAction(nullptr, "Settings")
{
}

mv::util::BlobCodec::Type PassthroughBlobCodecFactory::type() const
{
    return mv::util::BlobCodec::Type::Zstd;
}

QString PassthroughBlobCodecFactory::key() const
{
    return "none";
}

QString PassthroughBlobCodecFactory::displayName() const
{
    return "No compression";
}

mv::gui::CodecSettingsAction* PassthroughBlobCodecFactory::createSettingsFromVariantMap(const QVariantMap& map, QObject* parent) const
{
    auto* settings = new PassthroughCodecSettingsAction(nullptr, "Settings");
    settings->fromVariantMap(map);
    return settings;
}

std::unique_ptr<mv::util::BlobCodec> PassthroughBlobCodecFactory::createCodec(const mv::gui::CodecSettingsAction* codecSettingsAction /*= nullptr*/) const
{
    return std::make_unique<PassthroughBlobCodec>(codecSettingsAction);
}

const mv::gui::CodecSettingsAction* PassthroughBlobCodecFactory::getDefaultCodecSettingsAction() const
{
    return &_defaultSettingsAction;
}
