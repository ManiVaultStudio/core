#pragma once

#include "ManiVaultGlobals.h"

#include <QString>

#include <exception>

class QWidget;

namespace mv {

class ManiVaultException;

namespace util {

CORE_EXPORT void exceptionMessageBox(const QString& title, const QString& reason, QWidget* parent = nullptr);
CORE_EXPORT void exceptionMessageBox(const QString& title, QWidget* parent = nullptr);
CORE_EXPORT void exceptionMessageBox(const QString& title, const std::exception& exception, QWidget* parent = nullptr);
CORE_EXPORT void exceptionMessageBox(const QString& title, const ManiVaultException& exception, QWidget* parent = nullptr);

} // namespace util

} // namespace mv