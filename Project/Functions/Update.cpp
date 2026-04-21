#include "../Core/Engine.h"

void Engine::Update() {
	auto GworldPtr = Memory::read<uintptr_t>(Memory::getBaseAddress() + Offsets::UWorld);
	if (!GworldPtr) {
		ClearFNameCache();
		return;
	}

	auto tGWorld = Memory::read<uintptr_t>(GworldPtr);
	if (!tGWorld) {
		ClearFNameCache();
		return;
	}

	CheckWorldChange(tGWorld);

	uintptr_t tPersistentLevel = 0;
	uintptr_t tActors = 0;

	uintptr_t tPlayerController = this->PlayerController;
	uintptr_t tAcknowledgedPawn = this->AcknowledgedPawn;
	uintptr_t tPlayerCameraManager = this->PlayerCameraManager;
	uintptr_t tRootComponent = 0;
	uintptr_t tPlayerState = 0;

	tPersistentLevel = Memory::read<uintptr_t>(tGWorld + Offsets::PersistentLevel);

	if (tPersistentLevel) {
		tActors = Memory::read<uintptr_t>(tPersistentLevel + Offsets::AActors);
		int actor_count = Memory::read<int>(tPersistentLevel + Offsets::ActorCount);

		// 1. Validar a Cache (Verifica se o nosso boneco atual é válido)
		if (tPlayerController && IsUsermodePtr(tPlayerController)) {
			uintptr_t current_pawn = Memory::read<uintptr_t>(tPlayerController + Offsets::AcknowledgedPawn);
			if (!current_pawn || !IsUsermodePtr(current_pawn)) {
				tPlayerController = 0;
				tAcknowledgedPawn = 0;
				tPlayerCameraManager = 0;
			}
		}
		else {
			tPlayerController = 0;
			tAcknowledgedPawn = 0;
			tPlayerCameraManager = 0;
		}

		// 2. Se a Cache falhou, fazemos um mini-loop de "Salvamento" apenas em atores válidos
		if (!tPlayerController && tActors && actor_count > 0 && actor_count < 10000) {
			std::vector<uint64_t> all_actors(actor_count);
			if (Memory::ReadRaw(tActors, all_actors.data(), actor_count * sizeof(uint64_t))) {

				for (int i = 0; i < actor_count; i++) {
					uintptr_t actor = all_actors[i];
					if (!actor || !IsUsermodePtr(actor)) continue;

					// Procuramos diretamente por Câmaras. 
					// Os CameraManagers têm PCOwner (0x430), e o offset ViewTarget (0xCA0) a funcionar.
					uintptr_t pc_owner = Memory::read<uintptr_t>(actor + Offsets::PCOwner);

					if (pc_owner && IsUsermodePtr(pc_owner)) {
						// Se este ator tem um PCOwner, validamos se esse Controller é o LOCAL
						// (só os locais têm um AcknowledgedPawn não-nulo do lado do cliente)
						uintptr_t ack_pawn = Memory::read<uintptr_t>(pc_owner + Offsets::AcknowledgedPawn);

						if (ack_pawn && IsUsermodePtr(ack_pawn)) {
							// Lemos a coordenada para garantir que é um Pawn físico
							uintptr_t root = Memory::read<uintptr_t>(ack_pawn + Offsets::RootComponent);
							if (root && IsUsermodePtr(root)) {
								// SUCESSO ABSOLUTO
								tPlayerCameraManager = actor; // O ator iterado ERA a CameraManager!
								tPlayerController = pc_owner;
								tAcknowledgedPawn = ack_pawn;
								break; // Paramos de iterar.
							}
						}
					}
				}
			}
		}

		// 3. Atualizar componentes estáticos se tivermos um player vivo
		if (tPlayerController && tAcknowledgedPawn) {
			tRootComponent = Memory::read<uintptr_t>(tAcknowledgedPawn + Offsets::RootComponent);
			tPlayerState = Memory::read<uintptr_t>(tPlayerController + Offsets::PlayerState);
		}
	}

	// 4. PUBLICAR OS DADOS NO LOCK (Atomicamente)
	{
		std::unique_lock<std::shared_mutex> stateLock(m_stateMutex);
		GWorld = tGWorld;
		PersistentLevel = tPersistentLevel;
		PlayerController = tPlayerController;
		AcknowledgedPawn = tAcknowledgedPawn;
		PlayerCameraManager = tPlayerCameraManager;
		RootComponent = tRootComponent;
		PlayerState = tPlayerState;
		Actors = tActors;
	}

	// Debug console
	if (debugLog) {
		std::cout << "GWorld()-> " << std::hex << tGWorld << std::dec << std::endl;
		std::cout << "PersistentLevel()-> " << std::hex << tPersistentLevel << std::dec << std::endl;
		std::cout << "PlayerCameraManager()-> " << std::hex << tPlayerCameraManager << std::dec << std::endl;
		std::cout << "PlayerController()-> " << std::hex << tPlayerController << std::dec << std::endl;
		std::cout << "AcknowledgedPawn()-> " << std::hex << tAcknowledgedPawn << std::dec << std::endl;
		if (tRootComponent && IsUsermodePtr(tRootComponent)) {
			auto myPosition = Memory::read<Vector3>(tRootComponent + Offsets::RelativeLocation);
			printf("myPosition: [%.2f] [%.2f] [%.2f]\n", myPosition.x, myPosition.y, myPosition.z);
		}
	}
}