// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::util {

/**
 * @brief Tracks preset serialization.
 *
 * PresetSerializationContext exposes a thread-local flag that allows
 * serialization code to detect whether it is currently running as part of
 * preset serialization.
 *
 * Use Scope on the stack to enable the context for the current thread and
 * automatically restore the previous state when the scope exits. Nested scopes
 * preserve the state that was active before they were entered.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class PresetSerializationContext
{
public:

    /**
     * @brief Activates preset serialization.
     *
     * Scope enables the preset serialization context for the current thread
     * while the object exists. Destroying the scope restores the state that was
     * active before construction.
     */
    class CORE_EXPORT Scope
    {
    public:
        /**
         * @brief Construct a scope.
         */
        Scope();

        /**
         * @brief Destruct the scope.
         */
        ~Scope();

        /** Disable copy construction. */
        Scope(const Scope&) = delete;

        /** Disable copy assignment. */
        Scope& operator=(const Scope&) = delete;

    private:
        bool _previousActive;   /**< Preset serialization state before this scope was entered */
    };

    /**
     * @brief Get whether preset serialization is active.
     * @return Whether the current thread is inside a preset serialization scope.
     */
    [[nodiscard]] static CORE_EXPORT bool isActive();

private:
    static thread_local bool _active;   /**< Whether preset serialization is active for the current thread */
};

}
