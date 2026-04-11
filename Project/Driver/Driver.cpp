/*
#include "Driver.hpp"

#include <winioctl.h>
#include <ntstatus.h>
#include <winternl.h>
#include <fstream>
#include <cstring>

// ================= GLOBALS =================

HANDLE Kernel::drvhandle = nullptr;
INT32  Kernel::pid = 0;
uintptr_t Kernel::g_process_base = 0;
uintptr_t Kernel::g_client_base = 0;

bool Kernel::checkload() {
    drvhandle = CreateFileW(
        L"\\\\.\\\\wolfdrvszap",
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        3,
        0x00000080,
        0
    );

    return drvhandle && drvhandle != INVALID_HANDLE_VALUE;
}

void Kernel::autth(int addr)
{
    RW_REQUEST req{};
    req.sec = csec;
    req.addr = static_cast<ULONGLONG>(addr);

    DWORD bytesReturned = 0;

    DeviceIoControl(
        drvhandle,
        IOCTL_AUTTH,
        &req,
        sizeof(req),
        nullptr,
        0,
        &bytesReturned,
        nullptr
    );
}

uintptr_t Kernel::getbase()
{
    uintptr_t img_addr = 0;

    BA_REQUEST req{};
    req.sec = csec;
    req.pid = pid;
    req.addr = reinterpret_cast<ULONGLONG*>(&img_addr);

    DWORD bytesReturned = 0;

    DeviceIoControl(
        drvhandle,
        IOCTL_BA,
        &req,
        sizeof(req),
        nullptr,
        0,
        &bytesReturned,
        nullptr
    );

    g_process_base = img_addr;
    return img_addr;
}

uintptr_t Kernel::getmodulebase(const wchar_t* module_name)
{
    uintptr_t mod_base = 0;

    BM_REQUEST req{};
    req.sec = csec;
    req.pid = pid;
    req.addr = reinterpret_cast<ULONGLONG*>(&mod_base);
    wcsncpy_s(req.name, module_name, _TRUNCATE);

    DWORD bytesReturned = 0;

    DeviceIoControl(
        drvhandle,
        IOCTL_BM,
        &req,
        sizeof(req),
        nullptr,
        0,
        &bytesReturned,
        nullptr
    );

    return mod_base;
}

void Kernel::rmem(PVOID tgt_addr, PVOID buf, DWORD sz)
{
    RW_REQUEST req{};
    req.sec = csec;
    req.pid = pid;
    req.addr = reinterpret_cast<ULONGLONG>(tgt_addr);
    req.buf = reinterpret_cast<ULONGLONG>(buf);
    req.sz = sz;
    req.wr = FALSE;

    DWORD bytesReturned = 0;

    DeviceIoControl(
        drvhandle,
        IOCTL_RW,
        &req,
        sizeof(req),
        nullptr,
        0,
        &bytesReturned,
        nullptr
    );
}

void Kernel::wmem(PVOID tgt_addr, PVOID buf, DWORD sz)
{
    RW_REQUEST req{};
    req.sec = csec;
    req.pid = pid;
    req.addr = reinterpret_cast<ULONGLONG>(tgt_addr);
    req.buf = reinterpret_cast<ULONGLONG>(buf);
    req.sz = sz;
    req.wr = TRUE;

    DWORD bytesReturned = 0;

    DeviceIoControl(
        drvhandle,
        IOCTL_RW,
        &req,
        sizeof(req),
        nullptr,
        0,
        &bytesReturned,
        nullptr
    );
}

void Kernel::simmouseld()
{
    MOUSE_INPUT_DATA req{};
    req.ButtonFlags = 0x0001;

    DWORD bytesReturned = 0;

    DeviceIoControl(
        drvhandle,
        code_Mousesim,
        &req,
        sizeof(req),
        nullptr,
        0,
        &bytesReturned,
        nullptr
    );
}

void Kernel::simmouselu()
{
    MOUSE_INPUT_DATA req{};
    req.ButtonFlags = 0x0002;

    DWORD bytesReturned = 0;

    DeviceIoControl(
        drvhandle,
        code_Mousesim,
        &req,
        sizeof(req),
        nullptr,
        0,
        &bytesReturned,
        nullptr
    );
}

void Kernel::simmousemov1(LONG64 dx, LONG64 dy)
{
    MOUSE_INPUT_DATA mid{};
    mid.Flags = MOUSE_MOVE_RELATIVE;
    mid.LastX = static_cast<LONG>(dx);
    mid.LastY = static_cast<LONG>(dy);

    DWORD bytesReturned = 0;

    DeviceIoControl(
        drvhandle,
        code_Mousesim,
        &mid,
        sizeof(mid),
        nullptr,
        0,
        &bytesReturned,
        nullptr
    );
}

INT32 Kernel::getpid(LPCTSTR proc_name) {
    PROCESSENTRY32 pe{};
    pe.dwSize = sizeof(pe);

    HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hsnap == INVALID_HANDLE_VALUE)
        return 0;

    if (Process32First(hsnap, &pe)) {
        do {
            if (!lstrcmpi(pe.szExeFile, proc_name)) {
                pid = pe.th32ProcessID;
                CloseHandle(hsnap);
                return pid;
            }
        } while (Process32Next(hsnap, &pe));
    }

    CloseHandle(hsnap);
    return 0;
}

// ================= HELPERS =================

std::string ReadString(UINT_PTR addr) {
    constexpr size_t buf_size = 256;
    char buffer[buf_size]{};

    Kernel::rmem(reinterpret_cast<PVOID>(addr), buffer, buf_size);
    return std::string(buffer);
}
*/
#include "Driver.hpp"

uintptr_t virtualaddy = 0;
uintptr_t base_address = 0;
HANDLE Kernel::drvhandle = nullptr;
INT32  Kernel::pid = 0;
uintptr_t Kernel::g_process_base = 0;

bool Kernel::checkload() {
    // Abrimos a ligação ao driver OFICIAL do Windows (Nul)
    drvhandle = CreateFileW(L"\\\\.\\Nul", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

    // Verificamos se a nossa função modificada responde com um "Ping"
    if (drvhandle && drvhandle != INVALID_HANDLE_VALUE) {
        RW_REQUEST req{};
        req.sec = csec;
        req.wr = 99; // 99 = Comando de Ping (Apenas verificar ligação)
        
        DWORD bytes = 0;
        if (DeviceIoControl(drvhandle, IOCTL_CMD, &req, sizeof(req), &req, sizeof(req), &bytes, 0)) {
            return true; // Sucesso! O driver respondeu.
        }
    }
    return false;
}

void Kernel::autth(int addr) {
    UNREFERENCED_PARAMETER(addr);
}

uintptr_t Kernel::getbase() {
    RW_REQUEST req{};
    req.sec = csec;
    req.pid = pid;
    req.wr = 2;
    DWORD bytes = 0;
    DeviceIoControl(drvhandle, IOCTL_CMD, &req, sizeof(req), &req, sizeof(req), &bytes, 0);
    g_process_base = (uintptr_t)req.addr;
    return g_process_base;
}

void Kernel::rmem(PVOID tgt_addr, PVOID buf, DWORD sz) {
    RW_REQUEST req{};
    req.sec = csec; req.pid = pid; req.wr = 0;
    req.addr = (ULONGLONG)tgt_addr; req.buf = (ULONGLONG)buf; req.sz = sz;
    DWORD bytes = 0;
    DeviceIoControl(drvhandle, IOCTL_CMD, &req, sizeof(req), &req, sizeof(req), &bytes, 0);
}

void Kernel::wmem(PVOID tgt_addr, PVOID buf, DWORD sz) {
    RW_REQUEST req{};
    req.sec = csec; req.pid = pid; req.wr = 1;
    req.addr = (ULONGLONG)tgt_addr; req.buf = (ULONGLONG)buf; req.sz = sz;
    DWORD bytes = 0;
    DeviceIoControl(drvhandle, IOCTL_CMD, &req, sizeof(req), &req, sizeof(req), &bytes, 0);
}

void Kernel::simmouseld() {
    RW_REQUEST req{}; req.sec = csec; req.wr = 4;
    DWORD bytes = 0; DeviceIoControl(drvhandle, IOCTL_CMD, &req, sizeof(req), &req, sizeof(req), &bytes, 0);
}

void Kernel::simmouselu() {
    RW_REQUEST req{}; req.sec = csec; req.wr = 5;
    DWORD bytes = 0; DeviceIoControl(drvhandle, IOCTL_CMD, &req, sizeof(req), &req, sizeof(req), &bytes, 0);
}

void Kernel::simmousemov1(LONG64 dx, LONG64 dy) {
    RW_REQUEST req{}; req.sec = csec; req.wr = 6;
    req.addr = (ULONGLONG)dx; req.buf = (ULONGLONG)dy;
    DWORD bytes = 0; DeviceIoControl(drvhandle, IOCTL_CMD, &req, sizeof(req), &req, sizeof(req), &bytes, 0);
}

INT32 Kernel::getpid(LPCTSTR proc_name) {
    PROCESSENTRY32 pe{};
    pe.dwSize = sizeof(pe);
    HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hsnap == INVALID_HANDLE_VALUE) return 0;
    if (Process32First(hsnap, &pe)) {
        do {
            if (!lstrcmpi(pe.szExeFile, proc_name)) {
                pid = pe.th32ProcessID;
                CloseHandle(hsnap);
                return pid;
            }
        } while (Process32Next(hsnap, &pe));
    }
    CloseHandle(hsnap);
    return 0;
}

std::string ReadString(UINT_PTR addr) {
    char buffer[256]{};
    Kernel::rmem((PVOID)addr, buffer, sizeof(buffer));
    return std::string(buffer);
}