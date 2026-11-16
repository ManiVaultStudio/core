// SPDX-License-Identifier: LGPL-3.0-or-later

#include "StorageBackendMemory.h"

#include <QFile>
#include <QTextStream>

#ifdef Q_OS_WIN
    #define _WIN32_WINNT 0x0600
    #include <windows.h>
#elif defined(Q_OS_MACOS)
    #include <mach/mach.h>
    #include <mach/mach_host.h>
#endif

namespace mv::gui
{

std::optional<std::uint64_t> getAvailablePhysicalMemoryBytes()
{
#ifdef Q_OS_WIN
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(memoryStatus);
    if (GlobalMemoryStatusEx(&memoryStatus))
        return memoryStatus.ullAvailPhys;
    return std::nullopt;
#elif defined(Q_OS_LINUX)
    QFile file(QStringLiteral("/proc/meminfo"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return std::nullopt;

    QTextStream stream(&file);
    while (!stream.atEnd())
    {
        const auto line = stream.readLine();
        if (line.startsWith(QStringLiteral("MemAvailable:")))
        {
            bool ok = false;
            const auto availableKibibytes = line.section(QLatin1Char(':'), 1).simplified().section(QLatin1Char(' '), 0, 0).toULongLong(&ok);
            return ok ? std::optional<std::uint64_t>(availableKibibytes * 1024) : std::nullopt;
        }
    }

    return std::nullopt;
#elif defined(Q_OS_MACOS)
    vm_statistics64_data_t vmStatistics;
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    const auto host = mach_host_self();
    vm_size_t pageSize = 0;
    if (host_page_size(host, &pageSize) != KERN_SUCCESS || host_statistics64(host, HOST_VM_INFO64, reinterpret_cast<host_info64_t>(&vmStatistics), &count) != KERN_SUCCESS)
        return std::nullopt;

    return (static_cast<std::uint64_t>(vmStatistics.free_count) + vmStatistics.inactive_count + vmStatistics.speculative_count) * pageSize;
#else
    return std::nullopt;
#endif
}

}
