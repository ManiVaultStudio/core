// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ZstdCodecSettingsAction.h"


#include <util/BlobCodec.h>
#include <util/BlobCodecFactory.h>

class ZstdBlobCodecFactory final : public mv::util::BlobCodecFactory
{
public:
    ZstdBlobCodecFactory(QObject* parent = nullptr);

	mv::util::BlobCodec::Type type() const override;

	QString key() const override;

	QString displayName() const override;


	mv::gui::CodecSettingsAction* createSettingsFromVariantMap(const QVariantMap& map, QObject* parent = nullptr) const override;

	std::unique_ptr<mv::util::BlobCodec> createCodec(mv::gui::CodecSettingsAction* codecSettingsAction = nullptr) const override;

    /**
     * Get default codec settings action for this codec (returns nullptr if no settings are needed)
     * @return Default codec settings action for this codec (returns nullptr if no settings are needed)
     */
    const mv::gui::CodecSettingsAction* getDefaultCodecSettingsAction() const override;

private:
    ZstdCodecSettingsAction     _defaultSettingsAction;     /** Default codec settings action for this codec */
};
