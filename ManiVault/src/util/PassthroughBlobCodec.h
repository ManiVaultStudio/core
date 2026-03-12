// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "BlobCodec.h"

namespace mv::util {

class CORE_EXPORT PassthroughBlobCodec final : public BlobCodec
{
public:
    [[nodiscard]] Type getType() const override;
    [[nodiscard]] QString getName() const override;

    [[nodiscard]] Result encode(const QByteArray& input) const override;
    [[nodiscard]] Result decode(const QByteArray& input, qsizetype expectedSize = -1) const override;
};

}