// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HardwareSpec.h"

#ifdef Q_OS_WIN
	#define _WIN32_WINNT 0x0600
	#include <windows.h>
#endif

#ifdef Q_OS_UNIX
	#include <sys/sysinfo.h>
#endif

#include <QSysInfo>

namespace mv::util
{
DisplayResolution::DisplayResolution() :
    HardwareSpec(),
    _horizontal(0),
    _vertical(0)
{
}

void DisplayResolution::fromSystem()
{
    if (auto screen = QGuiApplication::primaryScreen()) {
        _horizontal = screen->size().width();
        _vertical   = screen->size().height();
    } else {
        _horizontal = 0;
        _vertical   = 0;
    }
}

void DisplayResolution::fromVariantMap(const QVariantMap& variantMap)
{
    _horizontal = variantMap.value("horizontal", 0).toInt();
    _vertical   = variantMap.value("vertical", 0).toInt();
}

AvailableRAM::AvailableRAM() :
    HardwareSpec(),
    _numberOfBytes(0)
{
}

void AvailableRAM::fromSystem()
{
    _numberOfBytes = getTotalSystemRAMBytes();
}

void AvailableRAM::fromVariantMap(const QVariantMap& variantMap)
{
    _numberOfBytes = variantMap.value("numberOfBytes", 0).toULongLong();
}

std::uint64_t AvailableRAM::getTotalSystemRAMBytes()
{
#if defined(Q_OS_WIN)
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return status.ullTotalPhys;
#elif defined(Q_OS_MAC)
    int64_t mem;
    size_t len = sizeof(mem);
    sysctlbyname("hw.memsize", &mem, &len, nullptr, 0);
    return mem;
#elif defined(Q_OS_UNIX)
    struct sysinfo info;
    sysinfo(&info);
    return quint64(info.totalram) * info.mem_unit;
#else
    return 0;
#endif
}

}
