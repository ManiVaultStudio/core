// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"

#include "actions/ToggleAction.h"
#include "actions/StringAction.h"
#include "actions/TriggerAction.h"

#include <QObject>

namespace mv {

class AbstractErrorLogger;

/**
 * Abstract error manager class
 *
 * Base abstract error manager class
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractErrorManager : public AbstractManager
{
    Q_OBJECT

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractErrorManager(QObject* parent) :
        AbstractManager(parent, "Errors"),
        _errorLogger(nullptr)
    {
    }

    /** Show the error logging consent dialog */
    virtual void showErrorLoggingConsentDialog() = 0;

protected:

    /**
     * Get the error logger
     * @return Pointer to the error logger (maybe nullptr)
     */
    AbstractErrorLogger* getErrorLogger() const
    {
	    return _errorLogger;
    };

    /**
     * Set the error logger to \p errorLogger
     * @param errorLogger Pointer to the error logger (maybe nullptr)
     */
    void setErrorLogger(AbstractErrorLogger* errorLogger)
    {
        _errorLogger = errorLogger;
    }

public: // Const action getters

    virtual const gui::TriggerAction& getLoggingAskConsentDialogAction() const { return const_cast<AbstractErrorManager*>(this)->getLoggingAskConsentDialogAction(); };             /** Get action for asking the user for consent to log errors */
    virtual const gui::ToggleAction& getLoggingUserHasOptedAction()  const { return const_cast<AbstractErrorManager*>(this)->getLoggingUserHasOptedAction(); };                     /** Get action for user has opted */
	virtual const gui::ToggleAction& getLoggingEnabledAction()  const { return const_cast<AbstractErrorManager*>(this)->getLoggingEnabledAction(); };                               /** Get action for logging enabled */
    virtual const gui::StringAction& getLoggingDsnAction()  const { return const_cast<AbstractErrorManager*>(this)->getLoggingDsnAction(); };                                       /** Get action for logging data source name (DSN) */
    virtual const gui::ToggleAction& getLoggingShowCrashReportDialogAction()  const { return const_cast<AbstractErrorManager*>(this)->getLoggingShowCrashReportDialogAction(); };   /** Get action for showing a crash report dialog when the application fails */

protected: // Non-const action getters

    virtual gui::TriggerAction& getLoggingAskConsentDialogAction() = 0;         /** Get action for asking the user for consent to log errors */
    virtual gui::ToggleAction& getLoggingUserHasOptedAction() = 0;              /** Get action for user has opted */
    virtual gui::ToggleAction& getLoggingEnabledAction() = 0;                   /** Get action for logging enabled */
    virtual gui::StringAction& getLoggingDsnAction() = 0;                       /** Get action for logging data source name (DSN) */
    virtual gui::ToggleAction& getLoggingShowCrashReportDialogAction() = 0;     /** Get action for showing a crash report dialog when the application fails */

private:
    AbstractErrorLogger*    _errorLogger;   /** Pointer to the error logger */

    friend class AbstractErrorLogger;
    friend class gui::ErrorLoggingSettingsAction;
};

}
