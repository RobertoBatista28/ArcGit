/*
#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <string>

// ================= IOCTL =================

#define IOCTL_RW        CTL_CODE(FILE_DEVICE_UNKNOWN, 0x753, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_BA        CTL_CODE(FILE_DEVICE_UNKNOWN, 0x845, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_AUTTH     CTL_CODE(FILE_DEVICE_UNKNOWN, 0x902, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_Mousesim   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x563, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_dumpdrv    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x905, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_BM    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x535, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define csec            0x253e5fe

// ================= STRUCTS =================

typedef struct _SYSTEM_MODULE {
    PVOID  Reserved1;
    PVOID  Reserved2;
    PVOID  ImageBase;
    ULONG  ImageSize;
    ULONG  Flags;
    USHORT Id;
    USHORT Rank;
    USHORT LoadCount;
    USHORT NameOffset;
    CHAR   Name[256];
} SYSTEM_MODULE, * PSYSTEM_MODULE;

typedef struct _SYSTEM_MODULE_INFORMATION {
    ULONG ModulesCount;
    SYSTEM_MODULE Modules[1];
} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;

typedef struct _DUMP_REQUEST {
    WCHAR module_name[260];
} DUMP_REQUEST, * PDUMP_REQUEST;

typedef struct _MOUSE_INPUT_DATA {
    USHORT UnitId;
    USHORT Flags;
    union {
        ULONG Buttons;
        struct {
            USHORT ButtonFlags;
            USHORT ButtonData;
        };
    };
    ULONG RawButtons;
    LONG  LastX;
    LONG  LastY;
    ULONG ExtraInformation;
} MOUSE_INPUT_DATA, * PMOUSE_INPUT_DATA;

typedef struct _RW_REQUEST {
    INT32 sec;
    INT32 pid;
    ULONGLONG addr;
    ULONGLONG buf;
    ULONGLONG sz;
    BOOLEAN wr;
} RW_REQUEST, * PRW_REQUEST;

typedef struct _BA_REQUEST {
    INT32 sec;
    INT32 pid;
    ULONGLONG* addr;
} BA_REQUEST, * PBA_REQUEST;

typedef struct _BM_REQUEST {
    ULONG sec;
    ULONG pid;
    wchar_t name[260];
    ULONGLONG* addr;
} BM_REQUEST;
// ================= KERNEL API =================

namespace Kernel {

    extern HANDLE    drvhandle;
    extern INT32     pid;
    extern uintptr_t g_process_base;
    extern uintptr_t g_client_base;

    bool        checkload();
    void        autth(int addr);
    INT32       getpid(LPCTSTR proc_name);
    uintptr_t   getbase();
    uintptr_t getmodulebase(const wchar_t* module_name);

    void rmem(PVOID tgt_addr, PVOID buf, DWORD sz);
    void wmem(PVOID tgt_addr, PVOID buf, DWORD sz);

    void simmouseld();
    void simmouselu();
    void simmousemov1(LONG64 dx, LONG64 dy);
}

// ================= HELPERS (INLINE) =================
inline uintptr_t virtualaddy = 0;
inline uintptr_t base_address = 0;

template <typename T>
inline T read(uint64_t addr) {
    T buf{};
    Kernel::rmem((PVOID)addr, &buf, sizeof(T));
    return buf;
}

template <typename T>
inline T write(uint64_t addr, T buf) {
    Kernel::wmem((PVOID)addr, &buf, sizeof(T));
    return buf;
}

std::string ReadString(UINT_PTR addr);
*/
#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <string>

#define csec 0x253e5fe
#define IOCTL_CMD CTL_CODE(FILE_DEVICE_UNKNOWN, 0x777, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

typedef struct _RW_REQUEST {
    INT32 sec;
    INT32 pid;
    ULONGLONG addr;
    ULONGLONG buf;
    ULONGLONG sz;
    INT32 wr; // 0=Read, 1=Write, 2=GetBase, 4=MouseD, 5=MouseU, 6=MouseM
    wchar_t module_name[260];
} RW_REQUEST, * PRW_REQUEST;

extern uintptr_t virtualaddy;
extern uintptr_t base_address;

namespace Kernel {
    extern HANDLE    drvhandle;
    extern INT32     pid;
    extern uintptr_t g_process_base;

    bool        checkload();
    void        autth(int addr);
    INT32       getpid(LPCTSTR proc_name);
    uintptr_t   getbase();
    uintptr_t   getmodulebase(const wchar_t* module_name);

    void rmem(PVOID tgt_addr, PVOID buf, DWORD sz);
    void wmem(PVOID tgt_addr, PVOID buf, DWORD sz);

    void simmouseld();
    void simmouselu();
    void simmousemov1(LONG64 dx, LONG64 dy);
}

template <typename T>
inline T read(uint64_t addr) {
    T buf{};
    Kernel::rmem((PVOID)addr, &buf, sizeof(T));
    return buf;
}

template <typename T>
inline void write(uint64_t addr, T buf) {
    Kernel::wmem((PVOID)addr, &buf, sizeof(T));
}

std::string ReadString(UINT_PTR addr);