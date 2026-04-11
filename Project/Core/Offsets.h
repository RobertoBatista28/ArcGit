#pragma once
#include <cstddef>

namespace Offsets {
	// --- Globais e Base ---
	constexpr std::ptrdiff_t UWorld = 0xDE77378;
	constexpr std::ptrdiff_t GNames = 0xDA26E00;
	constexpr std::ptrdiff_t FNameTable = 0xD96B86C;
	
	// NOTA: O GameInstance agora usa desencriptação (assinatura de referência: E8 ? ? ? ? 0F 57 C0 48 8B CB 4C 8B F0)
	constexpr std::ptrdiff_t GameInstance = 0x300; 
	
	// --- Nível e Atores ---
	constexpr std::ptrdiff_t PersistentLevel = 0xE8;
	constexpr std::ptrdiff_t AActors = 0x100;
	constexpr std::ptrdiff_t LocalPlayers = 0xF0;
	
	// --- Classes / Atores Principais ---
	constexpr std::ptrdiff_t APlayerController = 0x98;
	constexpr std::ptrdiff_t APlayerCameraManager = 0x3A8;
	constexpr std::ptrdiff_t ViewTargetTarget = 0x3F0;
	constexpr std::ptrdiff_t AcknowledgedPawn = 0x3E8; // Atualizado (Topo do Post)
	constexpr std::ptrdiff_t PlayerNamePrivate = 0x440;
	constexpr std::ptrdiff_t APlayerState = 0x3C8;
	
	// --- Componentes UE5 ---
	constexpr std::ptrdiff_t RootComponent = 0x230;
	constexpr std::ptrdiff_t RelativeLocation = 0x238;
	constexpr std::ptrdiff_t USkeletalMeshComponent = 0x430; // Mesh
	constexpr std::ptrdiff_t ComponentToWorld = 0x350;
	constexpr std::ptrdiff_t ActorID = 0x18;
	constexpr std::ptrdiff_t Super = 0xB0;
	
	// --- ARC Raiders: Informações de Jogo ---
	constexpr std::ptrdiff_t HealthInfo = 0x530;
	constexpr std::ptrdiff_t HealthComponent = 0xD20;
	constexpr std::ptrdiff_t Health = 0x688; // CachedHealth
	constexpr std::ptrdiff_t MaxHealth = 0x328;
	constexpr std::ptrdiff_t Shield = 0x170; // Armor
	constexpr std::ptrdiff_t TeamID = 0x822;
	constexpr std::ptrdiff_t InventoryComponent = 0xC58;
	constexpr std::ptrdiff_t WeaponQuality = 0x540;
	
	// InventoryComponent
	constexpr std::ptrdiff_t EquippedPrimaryItem = 0x540; 
	constexpr std::ptrdiff_t ItemDataAsset = 0xD0;

	// --- Câmara FMinimalViewInfo (Nova abordagem para W2S) ---
	constexpr std::ptrdiff_t CameraLocation = 0x410;
	constexpr std::ptrdiff_t CameraRotation = 0x430;
	constexpr std::ptrdiff_t CameraFOV = 0x450;
	
	// --- FOV Changer / Rendering ---
	constexpr std::ptrdiff_t DefaultFOV = 0x3C0; 
	constexpr std::ptrdiff_t LockedFOV = 0x3C4;
	
	// --- Vários ---
	constexpr std::ptrdiff_t BoundsScale = 0x4E8;
	constexpr std::ptrdiff_t LastSubmitTime = 0x4EC;
	constexpr std::ptrdiff_t LastRenderTimeOnScreen = 0x4F0;
	constexpr std::ptrdiff_t ComponentVelocity = 0x280;
	constexpr std::ptrdiff_t bIsBreaked = 0x12A0; // PlayerStatusVar
	constexpr std::ptrdiff_t ControlRotation = 0x340;
}