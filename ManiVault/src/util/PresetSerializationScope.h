// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::util {

/**
 * @brief Tracks preset serialization.
 *
 * PresetSerializationScope exposes a thread-local flag for detecting preset
 * serialization.
 *
 * Create a Scope object on the stack before serializing a preset. The scope
 * enables preset serialization for the current thread and restores the previous
 * state when it is destroyed. Nested scopes preserve the state that was active
 * before they were entered.
 *
 * The class itself is intentionally not exported because it owns private
 * thread-local storage. Only the functions that consumers call across the DLL
 * boundary are exported.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class PresetSerializationScope
{
public:

    /**
     * @brief Activates preset serialization.
     *
     * Scope enables the preset serialization context for the current thread
     * while the object exists. Destroying the scope restores the state that was
     * active before construction.
     *
     * Scope is exported so plugins and other consumers can safely create stack
     * instances while the thread-local state remains private to the public DLL.
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
