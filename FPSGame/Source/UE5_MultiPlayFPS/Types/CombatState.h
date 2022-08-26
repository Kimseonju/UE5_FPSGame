#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName="Unoccupied"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_ThrowGrenadeing UMETA(DisplayName = "ThrowGrenadeing"),
	ECS_Stunned UMETA(DisplayName = "Stunned"),
	ECS_END UMETA(DisplayName = "END")
};