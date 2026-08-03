// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#ifdef signals
    #pragma push_macro("signals")
    #undef signals
    #define MV_RESTORE_QT_SIGNALS
#endif

#ifdef slots
    #pragma push_macro("slots")
    #undef slots
    #define MV_RESTORE_QT_SLOTS
#endif

#ifdef emit
    #pragma push_macro("emit")
    #undef emit
    #define MV_RESTORE_QT_EMIT
#endif

#include <taskflow/taskflow.hpp>

#ifdef MV_RESTORE_QT_EMIT
    #pragma pop_macro("emit")
    #undef MV_RESTORE_QT_EMIT
#endif

#ifdef MV_RESTORE_QT_SLOTS
    #pragma pop_macro("slots")
    #undef MV_RESTORE_QT_SLOTS
#endif

#ifdef MV_RESTORE_QT_SIGNALS
    #pragma pop_macro("signals")
    #undef MV_RESTORE_QT_SIGNALS
#endif