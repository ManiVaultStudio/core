// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "RamComponentSpec.h"

#ifdef Q_OS_WIN
	#define _WIN32_WINNT 0x0600
	#include <windows.h>
#endif

#ifdef Q_OS_LINUX
	#include <sys/sysinfo.h>
#endif

#ifdef Q_OS_MAC
    #include <sys/sysctl.h>
#endif

#include <QSysInfo>

namespace mv::util
{

RamComponentSpec::RamComponentSpec() :
    HardwareComponentSpec("RAM Component Spec"),
    _numberOfBytes(0)
{
}

void RamComponentSpec::fromSystem()
{
    _numberOfBytes = getTotalSystemRAMBytes();

    setInitialized();
}

void RamComponentSpec::fromVariantMap(const QVariantMap& variantMap)
{
    try {
        if (!variantMap.contains("numberOfBytes"))
            throw std::runtime_error("Variant map does not contain 'numberOfBytes' key.");

        _numberOfBytes = variantMap.value("numberOfBytes", 0).toULongLong();

        setInitialized();
    }
    catch (std::exception& e)
    {
        qCritical() << "Unable read RAM hardware component properties from variant map:" << e.what();
    }
    catch (...)
    {
        qCritical() << "Unable read RAM hardware component properties from variant map";
    }
}

std::uint64_t RamComponentSpec::getTotalSystemRAMBytes()
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
