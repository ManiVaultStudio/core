// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "util/BlobCodec.h"

class ZstdBlobCodec final : public mv::util::BlobCodec
{
public:

    explicit ZstdBlobCodec(int compressionLevel = 3);

    [[nodiscard]] Type getType() const override;
    [[nodiscard]] QString getName() const override;

    [[nodiscard]] Result encode(const QByteArray& input) const override;
    [[nodiscard]] Result decode(const QByteArray& input, qsizetype expectedSize = -1) const override;
    [[nodiscard]] QString getFileExtension() const override;

    void setCompressionLevel(int compressionLevel);
    [[nodiscard]] int getCompressionLevel() const;

private:
    int _compressionLevel;
};
