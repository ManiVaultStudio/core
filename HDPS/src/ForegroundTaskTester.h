// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskTester.h"

namespace hdps
{

/**
 * Foreground task tester class
 *
 * Tests various foreground task scenarios.
 *
 * @author Thomas Kroes
 */
class ForegroundTaskTester final : public AbstractTaskTester
{
    Q_OBJECT

public:

    /**
     * Initialize tester with pointer to \parent object and tester name
     * @param parent Pointer to \parent object
     * @param name Name of the tester
     */
    Q_INVOKABLE ForegroundTaskTester(QObject* parent, const QString& name);

private:
    void testRunningIndeterminate();
    void testPerformance();
};

}

Q_DECLARE_METATYPE(hdps::ForegroundTaskTester)

inline const auto foregroundTaskTesterMetaTypeId = qRegisterMetaType<hdps::ForegroundTaskTester*>("hdps::ForegroundTaskTester");
