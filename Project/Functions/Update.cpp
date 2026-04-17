#include "../Core/Engine.h"
#include <iostream>
#include <chrono>
#include <vector>

void Engine::Update() {
    // 1. UWorld Proxy -> Real
    auto GworldProxy = Memory::read<uintptr_t>(Memory::getBaseAddress() + Offsets::UWorld);
    if (!IsUsermodePtr(GworldProxy)) {
        ClearFNameCache();
        return;
    }

    auto tGWorld = Memory::read<uintptr_t>(GworldProxy);
    if (!IsUsermodePtr(tGWorld)) return;

    // 2. PersistentLevel e Atores
    uintptr_t tPersistentLevel = Memory::read<uintptr_t>(tGWorld + Offsets::PersistentLevel);
    if (!IsUsermodePtr(tPersistentLevel)) return;

    uintptr_t tActors = Memory::read<uintptr_t>(tPersistentLevel + Offsets::AActors);
    int actor_count = Memory::read<int>(tPersistentLevel + Offsets::ActorCount);

    uintptr_t tPlayerController = 0;
    uintptr_t tPCM = 0;
    uintptr_t tAcknowledgedPawn = 0;
    uintptr_t tPlayerState = 0;
    uintptr_t tRootComponent = 0;

    // ========================================================================
    // BYPASS V4: O "CAVALO DE TROIA" (APawn -> PlayerController)
    // O Cérebro não está na lista, mas o Corpo está. Vamos pelo Corpo!
    // ========================================================================
    if (IsUsermodePtr(tActors) && actor_count > 0 && actor_count < 5000) {
        std::vector<uint64_t> actorList(actor_count);
        if (Memory::ReadRaw(tActors, actorList.data(), actor_count * sizeof(uint64_t))) {

            for (uint64_t actor : actorList) {
                if (!IsUsermodePtr(actor)) continue;

                // Hipótese 1: E se, por milagre, o Cérebro estiver na lista?
                uintptr_t pcm_direct = Memory::read<uintptr_t>(actor + Offsets::PCOwner);
                if (IsUsermodePtr(pcm_direct)) {
                    float fov = Memory::read<float>(pcm_direct + Offsets::DefaultFOV);
                    if (fov >= 50.0f && fov <= 130.0f) {
                        tPlayerController = actor;
                        tPCM = pcm_direct;
                        break;
                    }
                }

                // Hipótese 2 (A Garantida): Este ator é o nosso Corpo (APawn)!
                // O ponteiro APawn::Controller no UE5 fica sempre entre o 0x2A0 e 0x2D0
                for (uintptr_t ctrl_off = 0x2A0; ctrl_off <= 0x2D0; ctrl_off += 8) {
                    uintptr_t possible_controller = Memory::read<uintptr_t>(actor + ctrl_off);

                    if (IsUsermodePtr(possible_controller)) {
                        // Se isto for um Cérebro, ele tem uma Câmara agarrada ao PCOwner
                        uintptr_t pcm_indirect = Memory::read<uintptr_t>(possible_controller + Offsets::PCOwner);

                        if (IsUsermodePtr(pcm_indirect)) {
                            float fov = Memory::read<float>(pcm_indirect + Offsets::DefaultFOV);

                            // Se tem FOV de jogador, ganhámos a lotaria!
                            if (fov >= 50.0f && fov <= 130.0f) {
                                tPlayerController = possible_controller; // O Cérebro
                                tPCM = pcm_indirect;                     // A Câmara
                                tAcknowledgedPawn = actor;               // O Corpo (Cavalo de Troia)
                                break;
                            }
                        }
                    }
                }
                if (tPlayerController != 0) break; // Sai do scan assim que te encontrar
            }
        }
    }

    // ========================================================================
    // LER AS ESTATÍSTICAS DO JOGADOR
    // ========================================================================
    if (tPlayerController != 0) {
        // Garantir que temos o Corpo (Caso a Hipótese 1 tenha funcionado)
        if (tAcknowledgedPawn == 0) {
            tAcknowledgedPawn = Memory::read<uintptr_t>(tPlayerController + Offsets::AcknowledgedPawn);
            if (!IsUsermodePtr(tAcknowledgedPawn)) tAcknowledgedPawn = Memory::read<uintptr_t>(tPlayerController + 0x3D8);
            if (!IsUsermodePtr(tAcknowledgedPawn)) tAcknowledgedPawn = 0;
        }

        // Lê a tua "Ficha Médica" (PlayerState)
        tPlayerState = Memory::read<uintptr_t>(tPlayerController + Offsets::PlayerState);
        if (!IsUsermodePtr(tPlayerState)) tPlayerState = Memory::read<uintptr_t>(tPlayerController + 0x3C0);
        if (!IsUsermodePtr(tPlayerState)) tPlayerState = 0;
    }

    if (IsUsermodePtr(tAcknowledgedPawn)) {
        tRootComponent = Memory::read<uintptr_t>(tAcknowledgedPawn + Offsets::RootComponent);
    }

    // =========================================================================
    // DIAGNÓSTICO EM TEMPO REAL
    // =========================================================================
    static auto lastPrint = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now - lastPrint).count() >= 3) {
        std::cout << "\n--- DIAGNOSTICO DE PONTEIROS ---" << std::endl;
        std::cout << "[+] UWorld (Real):  0x" << std::hex << tGWorld << std::endl;
        std::cout << "[+] PersistentLvl:  0x" << tPersistentLevel << std::endl;
        std::cout << "[+] Actors Array:   0x" << tActors << " (Atores: " << std::dec << actor_count << ")" << std::hex << std::endl;

        if (tPlayerController == 0) {
            std::cout << "[-] PlayerControl:  Nao Encontrado (Procurando o teu corpo na memoria...)" << std::endl;
            std::cout << "[-] MyPawn:         0x0" << std::endl;
        }
        else {
            std::cout << "[+] PlayerControl:  0x" << tPlayerController << std::endl;
            std::cout << "[+] CameraManager:  0x" << tPCM << std::endl;
            std::cout << "[+] MyPawn:         0x" << tAcknowledgedPawn << std::endl;
            std::cout << "[+] FOV Atual:      " << std::dec << Memory::read<float>(tPCM + Offsets::DefaultFOV) << " graus" << std::hex << std::endl;
        }

        lastPrint = now;
    }
    // =========================================================================

    // Enviar dados para o Aimbot e ESP
    {
        std::unique_lock<std::shared_mutex> stateLock(m_stateMutex);
        GWorld = tGWorld;
        GameInstance = 0;
        PersistentLevel = tPersistentLevel;
        localplayer = 0;
        PlayerController = tPlayerController;
        AcknowledgedPawn = tAcknowledgedPawn;
        RootComponent = tRootComponent;
        PlayerState = tPlayerState;
        Actors = tActors;
        PlayerCameraManager = tPCM;
    }
}