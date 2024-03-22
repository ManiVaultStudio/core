// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QtCore/QtGlobal>

#ifndef MV_STATIC
#ifdef MV_SHARED_EXPORT
#define CORE_EXPORT Q_DECL_EXPORT
#else
#define CORE_EXPORT Q_DECL_IMPORT
#endif
#else
#define CORE_EXPORT
#endif
