#pragma once
#include <cstddef>

namespace Offsets {
	// --- Globais e Base ---
	constexpr std::ptrdiff_t UWorld = 0xE011D18;
	constexpr std::ptrdiff_t GNames = 0xDB48E80;
	constexpr std::ptrdiff_t FNameTable = 0xDA8D7F4;
	
	// --- UWorld ---
	constexpr std::ptrdiff_t PersistentLevel = 0xF0;
	constexpr std::ptrdiff_t LevelCollections = 0x398;
	constexpr std::ptrdiff_t OwningGameInstance = 0x458;
	
	// --- Nível e Atores ---
	constexpr std::ptrdiff_t AActors = 0x108;
	constexpr std::ptrdiff_t ActorCount = 0x110;
	constexpr std::ptrdiff_t LocalPlayers = 0xF0;
	
	// --- Classes / Atores Principais ---
	constexpr std::ptrdiff_t PlayerController = 0x3E8;
	constexpr std::ptrdiff_t DefaultFOV = 0x448;
	constexpr std::ptrdiff_t LockedFOV = 0x44C;
	constexpr std::ptrdiff_t ViewTarget = 0xCA0;
	constexpr std::ptrdiff_t CameraCachePOV = ViewTarget + 0x10;
	constexpr std::ptrdiff_t PCOwner = 0x430;
	constexpr std::ptrdiff_t ControlRotation = 0x428;
	constexpr std::ptrdiff_t AcknowledgedPawn = 0x3F0;
	constexpr std::ptrdiff_t PlayerState = 0x3D0;
	constexpr std::ptrdiff_t PawnPrivate = 0x428;  // APlayerState (mesmo offset, classe diferente)
	constexpr std::ptrdiff_t PlayerNamePrivate = 0x448;
	constexpr std::ptrdiff_t PlayerArray = 0x490;
	constexpr std::ptrdiff_t PlayerCount = 0x498;
	
	// --- Componentes UE5 ---
	constexpr std::ptrdiff_t RootComponent = 0x230;
	constexpr std::ptrdiff_t RelativeLocation = 0x218;
	constexpr std::ptrdiff_t RelativeRotation = 0x230;
	constexpr std::ptrdiff_t RelativeScale3D = 0x248;
	constexpr std::ptrdiff_t ComponentToWorld = 0x330;
	constexpr std::ptrdiff_t Mesh = 0x438;
	constexpr std::ptrdiff_t ComponentVelocity = 0x260;
	
	// --- Câmara FMinimalViewInfo ---
	constexpr std::ptrdiff_t CameraLocation = CameraCachePOV + 0x10;
	constexpr std::ptrdiff_t CameraRotation = CameraCachePOV + 0x30;
	constexpr std::ptrdiff_t CameraFOV = CameraCachePOV + 0x50;
	// --- ARC Raiders: Informações de Saúde e Status ---
	constexpr std::ptrdiff_t HealthComponent = 0xD78;
	constexpr std::ptrdiff_t HealthInfo = 0x540;
	constexpr std::ptrdiff_t CachedHealth = 0x668;
	constexpr std::ptrdiff_t MaxHealth = 0x308;
	constexpr std::ptrdiff_t Armor = 0x150;
	constexpr std::ptrdiff_t PlayerStatusVar = 0x12A0;
	constexpr std::ptrdiff_t TeamID = 0x822;
	
	// --- Inventário ---
	constexpr std::ptrdiff_t InventoryComponent = 0xCB0;
	constexpr std::ptrdiff_t EquippedPrimaryItem = 0x540;
	constexpr std::ptrdiff_t CurrentItemActors = 0x4D0;
	constexpr std::ptrdiff_t ItemDataAsset = 0xD0;
	constexpr std::ptrdiff_t WeaponQuality = 0x482;
	
	// --- Vários ---
	constexpr std::ptrdiff_t BoundsScale = 0x478;
	constexpr std::ptrdiff_t LastSubmitTime = BoundsScale + 0x4;
	constexpr std::ptrdiff_t LastRenderTime = BoundsScale + 0x8;
	constexpr std::ptrdiff_t LastRenderTimeOnScreen = BoundsScale + 0xC;
}