#include "../Core/Engine.h"
#include <iostream>
#include <chrono>

void Engine::Update() {
    // 1. CORREÇÃO CRÍTICA: Ler o UWorld apenas UMA vez!
    auto tGWorld = Memory::read<uintptr_t>(Memory::getBaseAddress() + Offsets::UWorld);
    if (!tGWorld || !IsUsermodePtr(tGWorld)) {
        ClearFNameCache();
        return;
    }

    // 2. LER O NÍVEL (Não precisa do GameInstance para desenhar o mundo)
    uintptr_t tPersistentLevel = Memory::read<uintptr_t>(tGWorld + Offsets::PersistentLevel);

    // 3. LER GAME INSTANCE (Tenta direto, se falhar tenta a desencriptação antiga)
    uintptr_t tGameInstance = Memory::read<uintptr_t>(tGWorld + Offsets::GameInstance);
    if (!IsUsermodePtr(tGameInstance)) {
        tGameInstance = GetGameInstance(tGWorld);
    }

    // 4. LER JOGADOR LOCAL
    uintptr_t tLocalPlayer = 0;
    uintptr_t tPlayerController = 0;
    if (IsUsermodePtr(tGameInstance)) {
        uintptr_t local_players = Memory::read<uintptr_t>(tGameInstance + Offsets::LocalPlayers);
        if (IsUsermodePtr(local_players)) {
            tLocalPlayer = Memory::read<uintptr_t>(local_players); // Primeiro jogador do Array
            if (IsUsermodePtr(tLocalPlayer)) {
                tPlayerController = Memory::read<uintptr_t>(tLocalPlayer + Offsets::APlayerController);
            }
        }
    }

    // 5. LER PAWN (O nosso boneco) E ESTADO
    uintptr_t tAcknowledgedPawn = 0;
    uintptr_t tPlayerState = 0;
    if (IsUsermodePtr(tPlayerController)) {
        tAcknowledgedPawn = Memory::read<uintptr_t>(tPlayerController + Offsets::AcknowledgedPawn);
        tPlayerState = Memory::read<uintptr_t>(tPlayerController + Offsets::APlayerState);
    }

    uintptr_t tRootComponent = 0;
    if (IsUsermodePtr(tAcknowledgedPawn)) {
        tRootComponent = Memory::read<uintptr_t>(tAcknowledgedPawn + Offsets::RootComponent);
    }

    // 6. LER ARRAY DE ATORES (Os inimigos)
    uintptr_t tActors = 0;
    if (IsUsermodePtr(tPersistentLevel)) {
        tActors = Memory::read<uintptr_t>(tPersistentLevel + Offsets::AActors);
    }

    // =========================================================================
    // DIAGNÓSTICO EM TEMPO REAL (Imprime na consola a cada 3 segundos)
    // =========================================================================
    static auto lastPrint = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now - lastPrint).count() >= 3) {
        std::cout << "\n--- DIAGNOSTICO DE PONTEIROS ---" << std::endl;
        std::cout << "[+] UWorld:         0x" << std::hex << tGWorld << std::endl;
        std::cout << "[+] PersistentLvl:  0x" << tPersistentLevel << std::endl;
        std::cout << "[+] GameInstance:   0x" << tGameInstance << std::endl;
        std::cout << "[+] LocalPlayer:    0x" << tLocalPlayer << std::endl;
        std::cout << "[+] PlayerControl:  0x" << tPlayerController << std::endl;
        std::cout << "[+] MyPawn:         0x" << tAcknowledgedPawn << std::endl;
        std::cout << "[+] Actors Array:   0x" << tActors << std::dec << std::endl;

        if (!IsUsermodePtr(tGameInstance)) std::cout << "[-] AVISO: GameInstance quebrado/encriptado!" << std::endl;
        if (!IsUsermodePtr(tPersistentLevel)) std::cout << "[-] AVISO: PersistentLevel quebrado!" << std::endl;
        if (!IsUsermodePtr(tActors)) std::cout << "[-] AVISO: AActors offset quebrado!" << std::endl;

        lastPrint = now;
    }
    // =========================================================================

    // Publicar o estado lido de forma segura para as threads do ESP e Aimbot
    {
        std::unique_lock<std::shared_mutex> stateLock(m_stateMutex);
        GWorld = tGWorld;
        GameInstance = tGameInstance;
        PersistentLevel = tPersistentLevel;
        localplayer = tLocalPlayer;
        PlayerController = tPlayerController;
        AcknowledgedPawn = tAcknowledgedPawn;
        RootComponent = tRootComponent;
        PlayerState = tPlayerState;
        Actors = tActors;
    }

    // Atualizar o Camera Manager
    uintptr_t tPCM = tPlayerController ? GetCameraManagerFromActors() : 0;
    {
        std::unique_lock<std::shared_mutex> stateLock(m_stateMutex);
        PlayerCameraManager = tPCM;
    }
}