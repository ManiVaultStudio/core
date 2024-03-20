// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Variant.h"

#ifdef _DEBUG
    #define VARIANT_VERBOSE
#endif

namespace mv {

Variant::Variant(const QVariant& other) :
    QVariant(other)
{

}

}
