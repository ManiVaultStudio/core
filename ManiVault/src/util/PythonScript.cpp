// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PythonScript.h"

#ifdef _DEBUG
	#define PYTHON_SCRIPT_VERBOSE
#endif

namespace mv::util
{

PythonScript::PythonScript(const QString& title, const Type& type, const QUrl& location, const Datasets& datasets, const Version& languageVersion /*= Version(3, 12, 0)*/, QObject* parent /*= nullptr*/) :
    Script(title, type, Language::Python, languageVersion, location, datasets, parent)
{
}

void PythonScript::run()
{
    Script::run();



}

}
