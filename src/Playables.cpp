#include "Playables.h"
#include <iostream>
#include <algorithm> // Required for std::clamp
#include <cmath>     // For other math functions like std::sqrt, std::sin
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/wrapped.hpp>
//#include "../build/BindMacros.h"

using namespace godot;

void Playables::_bind_methods()
{
	//BIND_FUNC(Playables, OnGroundDash);
	//ClassDB::add_virtual_method(get_class_static(), MethodInfo(Variant::INT, "foo"));

	BIND_VIRTUAL(Playables, OnGroundDash);
	BIND_VIRTUAL(Playables, OnWallDash);
	BIND_VIRTUAL(Playables, OnGroundJump);
	BIND_VIRTUAL(Playables, OnWallJump);
	BIND_VIRTUAL(Playables, OnJumpDone);
	BIND_VIRTUAL(Playables, OnJumpFailed);
	BIND_VIRTUAL(Playables, OnDashDone);
	BIND_VIRTUAL(Playables, OnDashFailed);
	/*BIND_VIRTUAL_2(Playables, StartNewPhysics, Variant::FLOAT, deltaTime, Variant::FLOAT, Iteration);
	BIND_VIRTUAL_1(Playables, OnMovementModeChanged, Variant::INT, PreviousMovementModeINT);
	BIND_VIRTUAL_1(Playables, UpdateCharacterStateBeforeMovement, Variant::FLOAT, deltaSeconds);
	BIND_VIRTUAL_1(Playables, UpdateCharacterStateAfterMovement, Variant::FLOAT, deltaSeconds);*/
	BIND_FUNC(Playables, CamUpdate);
	BIND_VIRTUAL(Playables, CheckToJump); 
	BIND_FUNC(Playables, StartNewPhysics);
	BIND_FUNC(Playables, OnMovementModeChanged);
	BIND_FUNC(Playables, UpdateCharacterStateBeforeMovement);
	BIND_FUNC(Playables, UpdateCharacterStateAfterMovement);
	BIND_SIG(Playables, OBJECT, OnGroundDash);
	BIND_PROP(Playables, Variant::FLOAT, MaxDashClamp); 

	BIND_PROP(Playables, Variant::NODE_PATH, WallCheckRayPath);
	BIND_PROP(Playables, Variant::NODE_PATH, GroundCheckRayPath);

	ClassDB::bind_method(D_METHOD("SetMaxWalkSpeed", "speed"), &Playables::SetMaxWalkSpeed);
	ClassDB::bind_method(D_METHOD("GetMaxWalkSpeed"), &Playables::GetMaxWalkSpeed);
	ClassDB::bind_method(D_METHOD("SetMaxRunSpeed", "speed"), &Playables::SetMaxRunSpeed);
	ClassDB::bind_method(D_METHOD("GetMaxRunSpeed"), &Playables::GetMaxRunSpeed);

	ClassDB::bind_method(D_METHOD("GetCamPath"), &Playables::GetCamPath);
	ClassDB::bind_method(D_METHOD("SetCamPath", "p_path"), &Playables::SetCamPath);

	ClassDB::bind_method(D_METHOD("GetCapPath"), &Playables::GetCapPath);
	ClassDB::bind_method(D_METHOD("SetCapPath", "p_path"), &Playables::SetCapPath);

	ClassDB::bind_method(D_METHOD("SetInputDirection", "inDirection"), &Playables::SetInputDirection);
	ClassDB::bind_method(D_METHOD("GetInputDirection"), &Playables::GetInputDirection); 

	ClassDB::bind_method(D_METHOD("Setinput", "new_input"), &Playables::SetInput);
	ClassDB::bind_method(D_METHOD("Getinput"), &Playables::Getinput);

	ClassDB::bind_method(D_METHOD("SetCam", "new_Cam"), &Playables::SetCam);
	ClassDB::bind_method(D_METHOD("GetCam"), &Playables::GetCam);

	ClassDB::bind_method(D_METHOD("SetCapBody", "new_Cam"), &Playables::SetCapBody);
	ClassDB::bind_method(D_METHOD("GetCapBody"), &Playables::GetCapBody);

	ClassDB::bind_method(D_METHOD("SetCapsuleBody", "newCap"), &Playables::SetCapsuleBody);
	ClassDB::bind_method(D_METHOD("GetCapsuleBody"), &Playables::GetCapsuleBody);

	ClassDB::bind_method(D_METHOD("PhysTick", "delta"), &Playables::PhysTick);

	ClassDB::bind_method(D_METHOD("SetGravity", "newVal"), &Playables::SetGravity);
	ClassDB::bind_method(D_METHOD("GetGravity"), &Playables::GetGravity);

	//ClassDB::bind_method(D_METHOD("SetCoyoteTimeActive", "newVal"), &Playables::SetCoyoteTimeActive);
	//ClassDB::bind_method(D_METHOD("GetCoyoteTimeActive"), &Playables::GetCoyoteTimeActive);

//#####################################################################################################

	ClassDB::bind_method(D_METHOD("SetAcceleration", "newVal"), &Playables::SetAcceleration);
	ClassDB::bind_method(D_METHOD("GetAcceleration"), &Playables::GetAcceleration);

	ClassDB::bind_method(D_METHOD("SetDeceleration", "newVal"), &Playables::SetDeceleration);
	ClassDB::bind_method(D_METHOD("GetDeceleration"), &Playables::GetDeceleration);

//#####################################################################################################

	ClassDB::bind_method(D_METHOD("SetCrouchFlag", "newVal"), &Playables::SetCrouchFlag);
	ClassDB::bind_method(D_METHOD("IsCrouching"), &Playables::IsCrouching);

	ClassDB::bind_method(D_METHOD("SetJumpFlag", "newVal"), &Playables::SetJumpFlag);
	ClassDB::bind_method(D_METHOD("IsJumping"), &Playables::IsJumping);

	ClassDB::bind_method(D_METHOD("SetSprintFlag", "newVal"), &Playables::SetSprintFlag);
	ClassDB::bind_method(D_METHOD("IsSprinting"), &Playables::IsSprinting);

	ClassDB::bind_method(D_METHOD("init"), &Playables::init);

	ClassDB::bind_method(D_METHOD("SetCrouchHeight", "newVal"), &Playables::SetCrouchHeight);
	ClassDB::bind_method(D_METHOD("GetCrouchHeight"), &Playables::GetCrouchHeight);

	ClassDB::bind_method(D_METHOD("SetSlideGravity", "newVal"), &Playables::SetSlideGravity);
	ClassDB::bind_method(D_METHOD("GetSlideGravity"), &Playables::GetSlideGravity);

	ClassDB::bind_method(D_METHOD("SetWallGravity", "newVal"), &Playables::SetWallGravity);
	ClassDB::bind_method(D_METHOD("GetWallGravity"), &Playables::GetWallGravity);

	ClassDB::bind_method(D_METHOD("SetSlideFloorGravityInfluence", "newVal"), &Playables::SetSlideFloorGravityInfluence);
	ClassDB::bind_method(D_METHOD("GetSlideFloorGravityInfluence"), &Playables::GetSlideFloorGravityInfluence);

	ClassDB::bind_method(D_METHOD("SetSlideFriction", "newVal"), &Playables::SetSlideFriction);
	ClassDB::bind_method(D_METHOD("GetSlideFriction"), &Playables::GetSlideFriction);

	ClassDB::bind_method(D_METHOD("SetWalkFriction", "newVal"), &Playables::SetWalkFriction);
	ClassDB::bind_method(D_METHOD("GetWalkFriction"), &Playables::GetWalkFriction);

	ClassDB::bind_method(D_METHOD("SetDirectionDrift", "newVal"), &Playables::SetDirectionDrift);
	ClassDB::bind_method(D_METHOD("GetDirectionDrift"), &Playables::GetDirectionDrift);

	ClassDB::bind_method(D_METHOD("SetMinSlideVel", "newVal"), &Playables::SetMinSlideVel);
	ClassDB::bind_method(D_METHOD("GetMinSlideVel"), &Playables::GetMinSlideVel);

	ClassDB::bind_method(D_METHOD("SetJumpPower", "newVal"), &Playables::SetJumpPower);
	ClassDB::bind_method(D_METHOD("GetJumpPower"), &Playables::GetJumpPower);

	ClassDB::bind_method(D_METHOD("SetJumpClamps", "newVal"), &Playables::SetJumpClamps);
	ClassDB::bind_method(D_METHOD("GetJumpClamps"), &Playables::GetJumpClamps);

	ClassDB::bind_method(D_METHOD("SetAirControlFactor", "newVal"), &Playables::SetAirControlFactor);
	ClassDB::bind_method(D_METHOD("GetAirControlFactor"), &Playables::GetAirControlFactor);

	ClassDB::bind_method(D_METHOD("SetMaxDashTime", "newVal"), &Playables::SetMaxDashTime);
	ClassDB::bind_method(D_METHOD("GetMaxDashTime"), &Playables::GetMaxDashTime);

	ClassDB::bind_method(D_METHOD("SetLowerAllowedWall", "newVal"), &Playables::SetLowerAllowedWall);
	ClassDB::bind_method(D_METHOD("GetLowerAllowedWall"), &Playables::GetLowerAllowedWall);

	ClassDB::bind_method(D_METHOD("SetLandShakeTime", "newVal"), &Playables::SetLandShakeTime);
	ClassDB::bind_method(D_METHOD("GetLandShakeTime"), &Playables::GetLandShakeTime);

	ClassDB::bind_method(D_METHOD("SetLandShakeIntensity", "newVal"), &Playables::SetLandShakeIntensity);
	ClassDB::bind_method(D_METHOD("GetLandShakeIntensity"), &Playables::GetLandShakeIntensity);

	ClassDB::bind_method(D_METHOD("SetChargeActionShakeTime", "newVal"), &Playables::SetChargeActionShakeTime);
	ClassDB::bind_method(D_METHOD("GetChargeActionShakeTime"), &Playables::GetChargeActionShakeTime);

	ClassDB::bind_method(D_METHOD("SetChargeActionShakeIntensity", "newVal"), &Playables::SetChargeActionShakeIntensity);
	ClassDB::bind_method(D_METHOD("GetChargeActionShakeIntensity"), &Playables::GetChargeActionShakeIntensity);

	ClassDB::bind_method(D_METHOD("SetShakeSpeed", "newVal"), &Playables::SetShakeSpeed);
	ClassDB::bind_method(D_METHOD("GetShakeSpeed"), &Playables::GetShakeSpeed);

	ClassDB::bind_method(D_METHOD("SetShakeDecay", "newVal"), &Playables::SetShakeDecay);
	ClassDB::bind_method(D_METHOD("GetShakeDecay"), &Playables::GetShakeDecay);

	ClassDB::bind_method(D_METHOD("SetFOVVelCap", "newVal"), &Playables::SetFOVVelCap);
	ClassDB::bind_method(D_METHOD("GetFOVVelCap"), &Playables::GetFOVVelCap);

	ClassDB::bind_method(D_METHOD("SetWallTilt", "newVal"), &Playables::SetWallTilt);
	ClassDB::bind_method(D_METHOD("GetWallTilt"), &Playables::GetWallTilt);

	ClassDB::bind_method(D_METHOD("SetWalkTilt", "newVal"), &Playables::SetWalkTilt);
	ClassDB::bind_method(D_METHOD("GetWalkTilt"), &Playables::GetWalkTilt);

	ClassDB::bind_method(D_METHOD("SetCoyoteTime", "newVal"), &Playables::SetCoyoteTime);
	ClassDB::bind_method(D_METHOD("GetCoyoteTime"), &Playables::GetCoyoteTime);

	ClassDB::bind_method(D_METHOD("SetChargeIncrements", "newVal"), &Playables::SetChargeIncrements);
	ClassDB::bind_method(D_METHOD("GetChargeIncrements"), &Playables::GetChargeIncrements);

	ClassDB::bind_method(D_METHOD("SetTiltTimeFactor", "newVal"), &Playables::SetTiltTimeFactor);
	ClassDB::bind_method(D_METHOD("GetTiltTimeFactor"), &Playables::GetTiltTimeFactor);

	ClassDB::bind_method(D_METHOD("SetBobbingFactor", "newVal"), &Playables::SetBobbingFactor);
	ClassDB::bind_method(D_METHOD("GetBobbingFactor"), &Playables::GetBobbingFactor);

	ClassDB::bind_method(D_METHOD("SetMaxFOVIncrease", "newVal"), &Playables::SetMaxFOVIncrease);
	ClassDB::bind_method(D_METHOD("GetMaxFOVIncrease"), &Playables::GetMaxFOVIncrease);

	ClassDB::bind_method(D_METHOD("SetBufferTime", "newVal"), &Playables::SetBufferTime);
	ClassDB::bind_method(D_METHOD("GetBufferTime"), &Playables::GetBufferTime);


	//ClassDB::bind_method(D_METHOD("chargeDash"), &Playables::chargeDash);
	//ClassDB::bind_method(D_METHOD("chargeJump"), &Playables::chargeJump);

	ClassDB::bind_method(D_METHOD("chargeAction", "IsDash"), &Playables::ChargeAction);
	ClassDB::bind_method(D_METHOD("GetCharge", "IsDash"), &Playables::GetCharge);

	ClassDB::bind_method(D_METHOD("InputBufferCall"), &Playables::InputBufferCall);
	ClassDB::bind_method(D_METHOD("CoyoteBufferCall"), &Playables::CoyoteBufferCall);
	ClassDB::bind_method(D_METHOD("MaxDashTimerCall"), &Playables::MaxDashTimerCall);
	ClassDB::bind_method(D_METHOD("ActiveShakeTimerCall"), &Playables::ActiveShakeTimerCall);


	ADD_GROUP("MovementConstants", "movement_");

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "Deceleration"), "SetDeceleration", "GetDeceleration");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "Acceleration"), "SetAcceleration", "GetAcceleration");


	ADD_SUBGROUP("Walk", "walk_");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "MaxWalkSpeed"), "SetMaxWalkSpeed", "GetMaxWalkSpeed");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "MaxRunSpeed"), "SetMaxRunSpeed", "GetMaxRunSpeed");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "CrouchHeight"), "SetCrouchHeight", "GetCrouchHeight");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "WalkFriction"), "SetWalkFriction", "GetWalkFriction");
	ADD_SUBGROUP("JumpDash", "jumpdash_");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "JumpClamps"), "SetJumpClamps", "GetJumpClamps");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "JumpPower"), "SetJumpPower", "GetJumpPower");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "MaxDashTime"), "SetMaxDashTime", "GetMaxDashTime");


	ADD_SUBGROUP("Slide", "slide_");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "SlideGravity"), "SetSlideGravity", "GetSlideGravity");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "SlideFloorGravityInfluence"), "SetSlideFloorGravityInfluence", "GetSlideFloorGravityInfluence");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "SlideFriction"), "SetSlideFriction", "GetSlideFriction");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "DirectionDrift"), "SetDirectionDrift", "GetDirectionDrift");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "minSlideVel"), "SetMinSlideVel", "GetMinSlideVel");

	ADD_SUBGROUP("WallRun", "wallrun_");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "WallGravity"), "SetWallGravity", "GetWallGravity");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "LowerAllowedWall"), "SetLowerAllowedWall", "GetLowerAllowedWall");

	ADD_SUBGROUP("Falling", "falling_");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "Gravity"), "SetGravity", "GetGravity");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "AirControlFactor"), "SetAirControlFactor", "GetAirControlFactor");

	ADD_GROUP("UserSettings", "user_");
	ADD_SUBGROUP("Grace", "grace_");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "BufferTime"), "SetBufferTime", "GetBufferTime");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "CoyoteTime"), "SetCoyoteTime", "GetCoyoteTime");

	ADD_SUBGROUP("Camera", "camera_");



 	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "LandShakeIntensity"), "SetLandShakeIntensity", "GetLandShakeIntensity");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ChargeActionShakeIntensity"), "SetChargeActionShakeIntensity", "GetChargeActionShakeIntensity");
 	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ShakeSpeed"), "SetShakeSpeed", "GetShakeSpeed");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ShakeDecay"), "SetShakeDecay", "GetShakeDecay");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "FOVVelCap"), "SetFOVVelCap", "GetFOVVelCap");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "WallTilt"), "SetWallTilt", "GetWallTilt");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "WalkTilt"), "SetWalkTilt", "GetWalkTilt");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ChargeIncrements"), "SetChargeIncrements", "GetChargeIncrements");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "TiltTimeFactor"), "SetTiltTimeFactor", "GetTiltTimeFactor");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "BobbingFactor"), "SetBobbingFactor", "GetBobbingFactor");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "maxFOVIncrease"), "SetMaxFOVIncrease", "GetMaxFOVIncrease");

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "InputDirection"), "SetInputDirection", "GetInputDirection");
	//ADD_PROPERTY(PropertyInfo(Variant::BOOL, "CoyoteTimeActive"), "SetCoyoteTimeActive", "GetCoyoteTimeActive");


 	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "LandShakeTime"), "SetLandShakeTime", "GetLandShakeTime");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ChargeActionShakeTime"), "SetChargeActionShakeTime", "GetChargeActionShakeTime");
 

	//ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "input"), "Setinput", "Getinput");
	//ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "Cam"), "SetCam", "GetCam");

	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "CamPath"), "SetCamPath", "GetCamPath");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "CapPath"), "SetCapPath", "GetCapPath");
	//ADD_PROPERTY(PropertyInfo(Variant::, "Cam"), "SetCam", "GetCam");

	//BIND_PROP(Playables, Variant::FLOAT, MaxWalkSpeed);
}

Playables::Playables()
{
	MaxWalkSpeed = 6;
	MaxRunSpeed = 16; 
	in = Input::get_singleton();
	LastWallNormal = Vector3();
	//for (int i = 0; i < get_child_count(); i++) //looks for the camera
	//{
	//	if (Camera3D* cam = Object::cast_to<Camera3D>(get_child(i)) )
	//	{
	//		Cam= cam;
	//		break; 
	//	}
	//}
}

Playables::~Playables()
{
	if(in != nullptr)
	in->set_mouse_mode(Input::MOUSE_MODE_VISIBLE);

	/*if (noise)
		memdelete(noise);*/
	/*if (JumpTimer)memdelete(JumpTimer);
	if (DashTimer)memdelete(DashTimer);
	if (InputBuffer)memdelete(InputBuffer);
	if (CoyoteBuffer)memdelete(CoyoteBuffer);
	if (MaxDashTimer)memdelete(MaxDashTimer);
	if (ActiveShakeTimer)memdelete(ActiveShakeTimer);
	if (noise)memdelete(noise);
	if (groundCheckRay)memdelete(groundCheckRay);*/
}

void Playables::_ready() {
}

void Playables::_process(double delta)
{
	if (Engine::get_singleton()->is_editor_hint()) {
		return;
	}
	//PhysTick(delta);  
}

void Playables::_input(const Ref<InputEvent>& event)
{
	if (Engine::get_singleton()->is_editor_hint()) {
		return;
	}
	/*func _input(event) :
		# Mouse in Camport coordinates.
		if event is InputEventMouseMotion :
	lookInput = event.screen_relative
		Capsule.rotate_y(-lookInput.x / 1000)
		Cam.rotation.x = (clamp(Cam.rotation.x - lookInput.y / 1000, deg_to_rad(-90), deg_to_rad(90)))*/
	//https://forum.godotengine.org/t/any-methods-to-identify-the-input-event-is-keyboard-mouse-or-joypad-in-c/47918/5
	Ref<InputEventMouseMotion> mouse_event = event;
	if (mouse_event.is_valid()) 
	{
		Vector2 lookInput = mouse_event->get_screen_relative();
		rotate_y(-lookInput.x / 1000);
		if (Cam != nullptr)
		{
			Vector3 current_rotation = Cam->get_rotation();
			float new_x = std::clamp(
				current_rotation.x - lookInput.y / 1000,
				(float)Math::deg_to_rad(-90.0),
				(float)Math::deg_to_rad(90.0)
			);
			Cam->set_rotation(Vector3(new_x, current_rotation.y, current_rotation.z));
		}
		//UtilityFunctions::print(lookInput);
	}
	Ref<InputEventKey> keyboard_event = event;
	//if (keyboard_event.is_valid()) 
	//{
	//	//Vector2 inputDir = Vector2::ZERO;
	//	//if (in->is_action_pressed("move_forward")) {
	//	
	//}
}

#pragma region Setters Getters

Vector3 Playables::GetSlideDirection()
{
	//FHitResult hit = CurrentFloor.HitResult;
	//if (hit.bBlockingHit) {
	//	FVector FloorNormal = hit.Normal;
	//	/*FVector CrossProduct;
	//	FVector up = FVector::UpVector;*/
	//	float DotProduct;

	//	//CrossProduct = FVector::CrossProduct(FloorNormal, up);
	//	//CrossProduct = FVector::CrossProduct(FloorNormal, CrossProduct);
	//	DotProduct = 1.0f - FVector::DotProduct(FloorNormal, FVector::UpVector);
	//	//FString VariableString = FString::Printf(TEXT("Value of Dot: %d"), DotProduct);
	//	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, VariableString);
	//	return (/*CrossProduct*/ FVector::VectorPlaneProject(FloorNormal, FVector::UpVector) * (FMath::Clamp(DotProduct, 0.0f, 1.0f)));//should be the direction the slide is going
	//}
	//return FVector(0, 0, 0);

	if (is_on_floor())
	{
		float DotProduct = 1.0 - (get_floor_normal().dot(UPWARDS));
		return get_floor_normal().slide(UPWARDS) * std::clamp(DotProduct, 0.0f, 1.0f);
	}

	return Vector3();
}

float Playables::GetSimulationTimeStep(float remainder, int iteration)
{
	//	float UCharacterMovementComponent::GetSimulationTimeStep(float RemainingTime, int32 Iterations) const
	//	{
	//		static uint32 s_WarningCount = 0;
	//		if (RemainingTime > MaxSimulationTimeStep)
	//		{
	//			if (Iterations < MaxSimulationIterations)
	//			{
	//				// Subdivide moves to be no longer than MaxSimulationTimeStep seconds
	//				RemainingTime = FMath::Min(MaxSimulationTimeStep, RemainingTime * 0.5f);
	//			}
	//			else
	//			{
	//				// If this is the last iteration, just use all the remaining time. This is usually better than cutting things short, as the simulation won't move far enough otherwise.
	//				// Print a throttled warning.
	//#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	//				if ((s_WarningCount++ < 100) || (GFrameCounter & 15) == 0)
	//				{
	//					UE_LOG(LogCharacterMovement, Warning, TEXT("GetSimulationTimeStep() - Max iterations %d hit while remaining time %.6f > MaxSimulationTimeStep (%.3f) for '%s', movement '%s'"), MaxSimulationIterations, RemainingTime, MaxSimulationTimeStep, *GetNameSafe(CharacterOwner), *GetMovementName());
	//				}
	//#endif
	//			}
	// 	   // no less than MIN_TICK_TIME (to avoid potential divide-by-zero during simulation).
		//return FMath::Max(MIN_TICK_TIME, RemainingTime);
	//		}
	if (remainder > MAXTIMESTEP && iteration < MAXITER)
		remainder = std::min(MAXTIMESTEP, remainder * 0.5f);

	return std::max(MIN_TICK_TIME, remainder);
}

float Playables::GetDashPower()
{
	float HoldTime = 0.1 * (ChargeFlags >> 4);//(CustomMovementFlags >> 4) * 0.1;
	if (MovementMode != EMovementMode::WallRunning)//(!IsCustomMovementMode(ECustomMovementMode::CMOVE_WallRun))
	{
		float calcLatDash = (std::clamp(HoldTime, 0.0f, 1.5f) + 0.4) * JumpPower * 1.1;//float calcLatDash = (FMath::Clamp(HoldTime, 0.0f, 1.5f) + 0.4f) * JumpPower * 1.1;
		float calcFreeDash = JumpPower * (std::clamp(HoldTime, 0.0f, 1.5f) + 0.2); //float calcFreeDash = JumpPower * (FMath::Clamp(HoldTime, 0.0f, 1.5f) + 0.2);
		float LatDashStrength = (MaxDashStrength > calcLatDash) && MaxDashActive ? MaxDashStrength : (VELMAG() > calcLatDash ? VELMAG() : calcLatDash);
		float FreeDashStrength = (MaxDashStrength > calcFreeDash) && MaxDashActive ? MaxDashStrength : (VELMAG() > calcFreeDash ? VELMAG() : calcFreeDash);
		float Value = IsPlayerFreeDashing() ? (FreeDashStrength) : LatDashStrength;
		//UtilityFunctions::print(calcFreeDash," ", calcLatDash);
		return /*CanDashFreely && CanDashLaterally ?*/ Value /*: CanDashFreely ? FreeDashStrength : JumpStrength*/;
	}
	else
	{
		float calcFreeDash = JumpPower * (std::clamp(HoldTime, 0.0f, 1.5f) + 0.2);
		float FreeDashStrength = (MaxDashStrength > calcFreeDash) ?/* Safe_MaxDashTimer > 0*/ MaxDashActive ? MaxDashStrength : VELMAG() : calcFreeDash;
		return /*CanDashFreely ?*/ FreeDashStrength /*: 0*/;
	}

	return 0;
}

bool Playables::IsPlayerFreeDashing()
{
	Vector3 forward = -get_global_basis().get_column(2) * -InputDirection.y;
	//UtilityFunctions::print((RelativeInputDirection.slide(UPWARDS)).dot(forward));
	return (std::clamp((double)get_floor_normal().dot(getForwardDir()), 0.0, 1.0) > 0.1 || !is_on_floor()) &&		//(FMath::Clamp((CurrentFloor.HitResult.Normal | PlayableOwner->GetControlRotation().Vector()), 0.0f, 1.0f) > 0.1 || !CurrentFloor.bBlockingHit) && //Checks if looking lower than floor or looking around parrallel to floor
		(InputDirection.is_zero_approx() || ((RelativeInputDirection.slide(UPWARDS)).dot(forward) >= 0.7)); 					//(Acceleration.IsNearlyZero() || (Acceleration.GetSafeNormal() | UpdatedComponent->GetForwardVector()) > 0.7);
}

Vector3 Playables::getForwardDir() const
{

	return Cam ? -(Cam->get_global_basis().get_column(2)) : Vector3();
}
#pragma endregion

#pragma region Ticks
void Playables::PhysTick(float delta, int iteration)
{
	if (Engine::get_singleton()->is_editor_hint()) {
		return;
	}

	UpdateCharacterStateBeforeMovementBRIDGE(delta);

	switch (MovementMode)
	{
	case Walking:
		WalkTick(delta, iteration);
		break;
	case Sliding:
		SlidingTick(delta, iteration);
		break;
	case WallRunning:
		WallRunTick(delta, iteration);
		break;
	case Falling:
		FallingTick(delta, iteration);
		break;
	case None:
		break;
	default:
		break;
	}

	UpdateCharacterStateAfterMovementBRIDGE(delta);
	//Vector3 vel = VEL(); 
	//Basis b = get_global_transform().get_basis();
	//Vector3 forward = -b.get_column(2) * -InputDirection.y;
	//Vector3 right = b.get_column(0) * InputDirection.x;
	//Vector3 Direction = (forward + right).normalized(); // Assuming InputDirection is a Vector2 where x is left/right and y is forward/backward

	//if (!InputDirection.is_zero_approx()) 
	//{
	//	//tilityFunctions::print("GettingInputs");
	//	if (!(vel.length() >= MaxRunSpeed))
	//	{
	//		vel += Direction * delta * Acceleration;
	//		vel = vel.normalized() * std::clamp((double)vel.length(), 0.0, MaxRunSpeed);
	//	}
	//	else
	//		vel = vel.normalized() * std::clamp((double)vel.length() - (Deceleration * delta), 0.0, (double)vel.length());
	//}
	//else
	//	vel = vel.normalized() * std::clamp((double)vel.length() - (Deceleration * delta), 0.0, (double)vel.length());
	//
	////UtilityFunctions::print(vel);
	//vel.y = !is_on_floor() ? (VEL().y - (Gravity * delta)) : 0;
	//set_velocity(vel); 
	//move_and_slide();
}

void Playables::WalkTick(float delta, int iteration)
{
	float remainingTime = delta;

	while ((remainingTime >= MIN_TICK_TIME) && (iteration < MAXITER))
	{
		iteration++;
		float timeTick = GetSimulationTimeStep(remainingTime, iteration);
		remainingTime -= timeTick;
		Vector3 vel = VEL();
		Vector3 oldNorm = get_floor_normal(); 
		if (!InputDirection.is_zero_approx() /*&& !(vel.length() >= (IsSprinting() || VELMAG() > MaxRunSpeed ? MaxRunSpeed : MaxWalkSpeed))*/)
		{
			vel += RelativeInputDirection.normalized() * timeTick * Acceleration;
			vel = vel.normalized() * std::clamp((float)vel.length(), 0.0f, VELMAG() > MaxRunSpeed
				? VELMAG()
				: IsSprinting() || VELMAG() > MaxWalkSpeed + 0.5
				? MaxRunSpeed
				: (IsCrouching() ? MaxCrouchSpeed : MaxWalkSpeed));

			//use these for fluid braking and acceleration friction braking 
			vel = vel - (vel - RelativeInputDirection * vel.length()) * std::min(delta * currFriction, 1.f);
		}
		else
		{
			vel = (vel.normalized() * std::clamp((double)vel.length() - (Deceleration * timeTick), 0.0, (double)vel.length()));
		}
		currFriction = std::clamp(currFriction + timeTick * WalkFriction * 2, 0.0f, WalkFriction);
		/*if (vel != vel.slide(get_floor_normal()))
			UtilityFunctions::print("changed Based On normal");
		vel = vel.slide(get_floor_normal()).normalized() * vel.length();*/

		set_velocity(vel + DOWNWARDS);
		move_and_slide();

		if (ShouldCatchAir(oldNorm, get_floor_normal()) || !is_on_floor())
		{
			SetMovementMode(EMovementMode::Falling);
			StartNewPhysicsBRIDGE(remainingTime, iteration);
			return;
		}
	}
}

void Playables::FallingTick(float delta, int iteration)
{
	float remainingTime = delta;
	while ((remainingTime >= MIN_TICK_TIME) && (iteration < MAXITER))
	{
		iteration++;
		float timeTick = GetSimulationTimeStep(remainingTime, iteration);
		remainingTime -= timeTick;
		Vector3 vel = VEL();
		if (!InputDirection.is_zero_approx() /*&& !(vel.length() >= (IsSprinting() || VELMAG() > MaxRunSpeed ? MaxRunSpeed : MaxWalkSpeed))*/)
		{
			vel += RelativeInputDirection.normalized() * timeTick * AirControlFactor * (IsCrouching() ? MaxCrouchSpeed : MaxWalkSpeed);
		}

		//fluid braking 
		vel = vel * (1. - std::min(SlideFriction * delta, 1.f)); //Velocity = Velocity * (1.f - FMath::Min(Friction * DeltaTime, 1.f));

		//Basis b = get_global_transform().get_basis();
		//Vector3 forward = -b.get_column(2) * -InputDirection.y;
		//Vector3 right = b.get_column(0) * InputDirection.x;
		//Vector3 Direction = (forward + right).normalized(); // Assuming InputDirection is a Vector2 where x is left/right and y is forward/backward
		vel.y = (VEL().y - (Gravity * timeTick));

		set_velocity(vel);
		move_and_slide();

		if (is_on_floor())
		{
			ScreenShake(abs(vel.y) * .1 + LandShakeIntensity, LandShakeTime + abs(vel.y) * 0.001);
			//UtilityFunctions::print(abs(vel.y) * 0.1);
			SetMovementMode(EMovementMode::Walking);
			StartNewPhysicsBRIDGE(remainingTime, iteration);
			return;
		}
		//UtilityFunctions::print("falling"); 
	}

}

void Playables::SlidingTick(float delta, int iteration)
{
	float remainingTime = delta;
	while ((remainingTime >= MIN_TICK_TIME) && (iteration < MAXITER))
	{
		iteration++;
		float timeTick = GetSimulationTimeStep(remainingTime, iteration);
		remainingTime -= timeTick;
		Vector3 vel = VEL();
		vel = vel * (is_on_floor() ? (1.f - std::min(currFriction * timeTick, 1.f)) : 1);
		currFriction = std::clamp(currFriction + timeTick, 0.0f, SlideFriction);

		if (is_on_floor() && !PrevFloor)
		{
			ScreenShake(abs(vel.y) * .1 + LandShakeIntensity, LandShakeTime + abs(vel.y) * 0.001);
			Vector3 floorprojection = get_floor_normal().slide(UPWARDS).slide(get_floor_normal()).normalized() * -(get_velocity().dot(-get_floor_normal()));
			currFriction = 0;
			set_velocity(get_velocity() + floorprojection);
		}

		/*FVector DirectionLerp = FVector::VectorPlaneProject(Acceleration.GetSafeNormal2D(), CurrentFloor.HitResult.Normal).GetSafeNormal2D() * timetick;
		Velocity = (FVector(DirectionLerp.X + Velocity.GetSafeNormal().X, DirectionLerp.Y + Velocity.GetSafeNormal().Y, 0)).GetSafeNormal2D() * Velocity.Size2D() + FVector::UpVector * Velocity.Z;*/

		if (!InputDirection.is_zero_approx())
		{
			Vector3 inDir = (RelativeInputDirection.normalized() * timeTick * DirectionDrift * (MaxRunSpeed)); 
			vel += Size2D(vel) < MaxRunSpeed || (vel + inDir).length() < vel.length() ? inDir : Vector3(0, 0, 0);
			Vector3 DirectionLerp = GetSafeNormal2D(RelativeInputDirection.slide(is_on_floor() ? get_floor_normal() : UPWARDS)) * timeTick * DirectionDrift;
			vel = (Vector3(DirectionLerp.x + vel.normalized().x, vel.normalized().y, DirectionLerp.z + vel.normalized().z)).normalized() * vel.length();
		}

		vel.y = VEL().y - (!is_on_floor() ? (SlideGravity * timeTick) : 0);


		Vector3 dir = GetSlideDirection();
		vel += SlideFloorGravityInfluence * dir.normalized().slide(UPWARDS) * timeTick * dir.length();

		if (!PrevFloor && is_on_floor())
		{
			Vector3 floorprojection = SlideGravity * get_floor_normal().slide(UPWARDS).slide(get_floor_normal()).normalized();
			currFriction = 0;
			set_velocity(get_velocity() + floorprojection);
		}

		//setting up the final movement
		//note: downwards needs to be here reguardless of if there's gravity or not 
		//that is how move and slide is calculating the snapping assuming that there's going to be a floor collision. 
		set_velocity(vel + (is_on_floor() ? DOWNWARDS : Vector3(0, 0, 0)));
		move_and_slide();

		if (is_on_wall() && vel.normalized().dot(get_wall_normal().normalized()) < -0.4)
		{
			set_velocity(GetMirroredVector(vel, get_wall_normal()));
			CanCoyoteTimeJump = true;
			//UtilityFunctions::print("called"); 
		}

		PrevFloor = is_on_floor();
	}
}

void Playables::WallRunTick(float delta, int iteration)
{
	float remainingTime = delta;
	while ((remainingTime >= MIN_TICK_TIME) && (iteration < MAXITER))
	{
		iteration++;
		float timeTick = GetSimulationTimeStep(remainingTime, iteration);
		remainingTime -= timeTick;
		Vector3 vel = VEL() + (LastWallNormal.dot(get_wall_normal()) > 0.9 ? LastWallNormal : Vector3()) * 9;
		
		LastWallNormal = get_wall_normal();
		LastYTouchedWall = get_global_position().y;
		 
		if (!InputDirection.is_zero_approx())
		{
			Vector3 tempVel = vel + RelativeInputDirection.slide(get_wall_normal()).slide(UPWARDS) * Acceleration * timeTick;
			if (!(vel.length() > MaxRunSpeed * 2 && tempVel.length() > vel.length())) vel = tempVel;
		}
		vel = vel.slide(get_wall_normal()) + DOWNWARDS * WallGravity * timeTick;
		vel = IsCrouching() ? vel * (1 - timeTick * 3) : vel;
		//setting up the final movement
		set_velocity(vel + (4 * -LastWallNormal * Vector3(1, 0, 1)));
		move_and_slide();

		//WallCheckRay->set_target_position(-LastWallNormal); 
		//WallCheckRay->force_raycast_update(); 
		//UtilityFunctions::print(WallCheckRay->is_colliding() ? "Colliding" : "Not Colliding");

		if (!is_on_wall() || is_on_floor() || LastWallNormal.dot(get_wall_normal()) < .4)
		{
			//UtilityFunctions::print(WallCheckRay->is_colliding() ? "Colliding" : "Not Colliding");
			SetMovementMode(Falling);
			StartNewPhysicsBRIDGE(remainingTime, iteration);
			return;
		}
	}
}
#pragma endregion

void Playables::StartNewPhysics(float deltaTime, int Iterations)
{
	if (Engine::get_singleton()->is_editor_hint()) {
		return;
	}

	switch (MovementMode)
	{
	case Walking:
		WalkTick(deltaTime, Iterations);
		break;
	case Sliding:
		SlidingTick(deltaTime, Iterations);
		break;
	case WallRunning:
		WallRunTick(deltaTime, Iterations);
		break;
	case Falling:
		FallingTick(deltaTime, Iterations);
		break;
	case None:
		break;
	default:
		break;
	}
}

void Playables::OnMovementModeChanged(int PreviousMovementModeINT)
{
	EMovementMode PreviousMovementMode = (EMovementMode)PreviousMovementModeINT;
	//UtilityFunctions::print(PreviousMovementMode, "->", MovementMode); 
	if (CanCoyoteTimeJump && MovementMode == Falling) 
	{
		//UtilityFunctions::print("CoyoteTimeStarted");
		CanCoyoteTimeJump = false; 
		CoyoteTimeActive = true; 
		CoyoteBuffer->start(); 
	}
	else 
	{
		MaxDashActive = true; 
		MaxDashTimer->start(); 
		CoyoteTimeActive = false; 
		CoyoteBuffer->stop(); 
	}

	switch (MovementMode)
	{
	case Walking:
		//UtilityFunctions::print("Walking");
		break;
	case Sliding:
		//UtilityFunctions::print("Sliding");
		break;
	case WallRunning:
		break;
	case Falling:
		break;
	case None:
		break;
	default:
		break;
	}
}

void Playables::UpdateCharacterStateBeforeMovement(float deltaSeconds)
{
	MaxDashStrength = VELMAG() > MaxDashStrength ? VELMAG() : MaxDashStrength;
	CanCoyoteTimeJump = is_on_floor() || MovementMode == WallRunning ? true : CanCoyoteTimeJump;
	CoyoteSlideRefreshCnt = is_on_floor() ? 0 : CoyoteSlideRefreshCnt;
	if (CheckToJump()) move_and_slide(); 
	LastWallNormal = is_on_floor() ? Vector3() : LastWallNormal;

	PrevFloor = is_on_floor();
	Basis b = get_global_transform().get_basis();
	PlayerForwardInput = -b.get_column(2) * -InputDirection.y;
	PlayerRightInput = b.get_column(0) * InputDirection.x;
	RelativeInputDirection = (PlayerForwardInput + PlayerRightInput).normalized(); // Assuming InputDirection is a Vector2 where x is left/right and y is forward/backward
	if (MovementMode != EMovementMode::Walking && is_on_floor() && !(IsCrouching() && VELMAG() > minSlideVel))
	{
		//UtilityFunctions::print("Walking");
		SetMovementMode(EMovementMode::Walking);
	}
	GroundCheckRay->force_raycast_update(); 
	if (MovementMode != EMovementMode::WallRunning && (is_on_wall_only() && !GroundCheckRay->is_colliding()
		&& (!LastWallNormal.is_equal_approx(get_wall_normal()) || (LastYTouchedWall - LowerAllowedWall) > get_global_position().y)) && !(IsCrouching() && VELMAG() > minSlideVel))
	{
		SetMovementMode(EMovementMode::WallRunning);
	}

	if (MovementMode != Falling && !is_on_floor() && !(IsCrouching() && VELMAG() > minSlideVel) && !(!GroundCheckRay->is_colliding() && is_on_wall_only()))
	{
		//UtilityFunctions::print("Falling");
		SetMovementMode(EMovementMode::Falling);
	}

	if (MovementMode != EMovementMode::Sliding && MovementMode != WallRunning && IsCrouching() && VELMAG() > minSlideVel)
	{
		//UtilityFunctions::print("Sliding");
		SetMovementMode(EMovementMode::Sliding);
	}	

	CamUpdateBRIDGE(deltaSeconds); 
}

void Playables::UpdateCharacterStateAfterMovement(float deltaSeconds)
{
	PrevInputFlags = InputFlags; //so we know when players release buttons and what not
	PrevFloor = is_on_floor(); 
}

void Playables::ExitWallRun()
{
}

void Playables::ChargeAction(bool isDash)
{
	if (isDash && (ChargeFlags >> 4 != 15)) 	ChargeFlags = (((ChargeFlags >> 4) + 1) << 4) + (ChargeFlags & 15);
	else if (!isDash && ((ChargeFlags & 15) != 15)) ChargeFlags = ((ChargeFlags & 15) + 1) + (ChargeFlags & 240);

	//UtilityFunctions::print("dashCharge: ", getCharge(true), "jumpCharge: ", getCharge(false));
}

void Playables::init()
{
	if (in == nullptr)
		in = Input::get_singleton();

	// Use .is_empty() to check if the path was actually set in the Godot Inspector

	if (!CamPath.is_empty()) Cam = get_node<Camera3D>(CamPath);
	if (!CapPath.is_empty()) 
	{
		CapBody = get_node<CollisionShape3D>(CapPath);
		CollisionShape3D* collision_shape = get_node<CollisionShape3D>(CapPath);
		if (collision_shape) {
			CapsuleBody = Object::cast_to<CapsuleShape3D>(collision_shape->get_shape().ptr());
			defaultHeight = CapsuleBody->get_height();
		}
	}
	//GroundCheckRayPath
	//WallCheckRayPath

	if (!GroundCheckRayPath.is_empty()) 
	{
		GroundCheckRay = get_node<RayCast3D>(GroundCheckRayPath);
		GroundCheckRay->set_target_position(Vector3(0, -CapsuleBody->get_height(), 0)); 
	}
	if (!WallCheckRayPath.is_empty()) 
	{
		WallCheckRay = get_node<RayCast3D>(WallCheckRayPath);
	}


	
	/*if (Cam == nullptr) {
		UtilityFunctions::printerr("ERROR: Cam is null. Assign CamPath in the Inspector!");
	}*/
	JumpTimer = memnew(Timer);
	DashTimer = memnew(Timer);
	InputBuffer = memnew(Timer);
	CoyoteBuffer = memnew(Timer);
	MaxDashTimer = memnew(Timer);
	ActiveShakeTimer = memnew(Timer);

	JumpTimer->set_one_shot(false);
	DashTimer->set_one_shot(false);
	InputBuffer->set_one_shot(true);
	CoyoteBuffer->set_one_shot(true);
	MaxDashTimer->set_one_shot(true);
	ActiveShakeTimer->set_one_shot(true);

	JumpTimer->set_wait_time(ChargeIncrements);
	DashTimer->set_wait_time(ChargeIncrements);
	InputBuffer->set_wait_time(BufferTime);
	CoyoteBuffer->set_wait_time(CoyoteTime);
	MaxDashTimer->set_wait_time(MaxDashTime);

	Callable ChargeCall = Callable(this, "chargeAction");

	DashTimer->connect("timeout", ChargeCall.bind(true));
	JumpTimer->connect("timeout", ChargeCall.bind(false));
	InputBuffer->connect("timeout", Callable(this, "InputBufferCall"));
	CoyoteBuffer->connect("timeout", Callable(this, "CoyoteBufferCall"));
	MaxDashTimer->connect("timeout", Callable(this, "MaxDashTimerCall"));
	ActiveShakeTimer->connect("timeout", Callable(this, "ActiveShakeTimerCall"));

	add_child(JumpTimer);
	add_child(DashTimer);
	add_child(InputBuffer);
	add_child(CoyoteBuffer);
	add_child(MaxDashTimer);
	add_child(ActiveShakeTimer);

	/*groundCheckRay = memnew(RayCast3D);
	wallCheckRay = memnew(RayCast3D);
	add_child(groundCheckRay);
	add_child(wallCheckRay);*/

	noise = memnew(FastNoiseLite);
	noise->set_noise_type(FastNoiseLite::TYPE_PERLIN);

	//if(groundCheckRay && CapsuleBody && wallCheckRay)
	//{
	//	groundCheckRay->set_target_position(Vector3(0, -CapsuleBody->get_height() / 3, 0)); // 2 units down
	//	groundCheckRay->set_enabled(true);
	//	groundCheckRay->set_exclude_parent_body(true); // Don't hit self
	//	groundCheckRay->set_visible(true); 
	//	wallCheckRay->set_enabled(true);
	//	wallCheckRay->set_exclude_parent_body(true); // Don't hit self
	//	wallCheckRay->set_visible(true);
	//}
	defaultFOV = Cam->get_fov(); 
	/*if (raycast->is_colliding()) {
		Vector3 hit_point = raycast->get_collision_point();
		Vector3 hit_normal = raycast->get_collision_normal();
		Object* hit_object = raycast->get_collider();

		UtilityFunctions::print("Raycast hit at: ", hit_point);
	}*/

	//UtilityFunctions::print("we've called whole of init"); 
	//GDVIRTUAL_CALL(init);
	//UtilityFunctions::print("we've called end of init");


}

void Playables::SetUpJumpTimer(bool isStart)
{
	if (isStart)
		JumpTimer->start();
	else
		JumpTimer->stop();

	//UtilityFunctions::print("JumpIn");

}

void Playables::SetUpDashTimer(bool isStart)
{
	if (isStart)
		DashTimer->start();
	else
		DashTimer->stop();

	//UtilityFunctions::print("DashIn");

}

bool Playables::ShouldCatchAir(Vector3 oldNorm, Vector3 newNorm)
{
	float oldYVel = std::clamp((float)VEL().slide(oldNorm.normalized()).y, 0.0f, VELMAG()); //float oldZVel = FMath::Clamp(FVector::VectorPlaneProject(Velocity, OldFloor.HitResult.Normal).Z, 0.0f, Velocity.Size());
	float newYVel = std::clamp((float)VEL().slide(newNorm.normalized()).y, 0.0f, VELMAG());//float newZVel = FMath::Clamp(FVector::VectorPlaneProject(Velocity, NewFloor.HitResult.Normal).Z, 0.0f, Velocity.Size());
	bool WillCatchAir = (oldNorm != newNorm) && std::clamp((oldYVel - newYVel), 0.f, VELMAG()) > 0.1; //bool willCatchAir = (OldFloor.HitResult.Normal != NewFloor.HitResult.Normal) && FMath::Clamp((oldZVel - newZVel), 0.0f, Velocity.Size()) > 10.0f;
	//default gravity is 980 so if the zcomponent is greater than this than we know that its prolly gonna catch some air cool? 
	//if(willCatchAir)
	//GEngine->AddOnScreenDebugMessage(5, 5.f, FColor::Green, FString::Printf(TEXT("Catching Air?: %f"), (oldZVel - newZVel)));
	//GEngine->AddOnScreenDebugMessage(5, 15.0f, FColor::Yellow, TEXT("Catching Air?: %f"));
	if (WillCatchAir)
	{
		set_velocity(VEL().slide(oldNorm).normalized() * VELMAG()); //Velocity.Z = willCatchAir ? oldZVel : 0;
		//UtilityFunctions::print("CaughtAir"); 
	}
	return WillCatchAir;
}

void Playables::CamUpdate(float delta)
{
	float Offset = Math::lerp(0.f, (float)Cam->get_position().y, std::clamp(CrouchBlendTime / CrouchBlendDuration, 0.f, 1.f));
	//UtilityFunctions::print(CrouchBlendTime);
	if (MovementMode == WallRunning) 
	{
		if (LastWallNormal.dot(GetPlayerRight()) > 0.4) RollTarget = -WallTilt * TORAD;
		else if (LastWallNormal.dot(GetPlayerRight()) < -0.4) RollTarget = WallTilt * TORAD;
		else RollTarget = 0;
	}
	else
	{
		if (RelativeInputDirection.dot(GetPlayerRight()) > 0.6) RollTarget = -WalkTilt * TORAD;
		else if (RelativeInputDirection.dot(GetPlayerRight()) < -0.6) RollTarget = WalkTilt * TORAD;
		else RollTarget = 0;
	}

	CrouchBlendTime = std::clamp(CrouchBlendTime + (IsCrouching() ? delta : -delta), 0.f, CrouchBlendDuration);	//

	if (isShaking && Cam && noise)
	{
		ShakeTime += delta * ShakeSpeed;

		Cam->set_h_offset(noise->get_noise_2d(ShakeTime, 0) * ShakeIntensity);
		Cam->set_v_offset(noise->get_noise_2d(0, ShakeTime) * ShakeIntensity);
		Cam->set_position(/*Vector3(noise->get_noise_2d(ShakeTime, 0) * ShakeIntensity
			, noise->get_noise_2d(0, ShakeTime) * ShakeIntensity, 0) +*/ UPWARDS * Offset);
		//UtilityFunctions::print("h: ", noise->get_noise_2d(ShakeTime, 0) * ShakeIntensity, " v: ", noise->get_noise_2d(0, ShakeTime) * ShakeIntensity); 
		ShakeIntensity = ShakeIntensity > 0 ? ShakeIntensity - delta * ShakeDecay : 0;
	}
	else if (Cam)
	{
		Cam->set_h_offset(Math::lerp((float)Cam->get_h_offset(), 0.f, OffsetLerpBackFactor * delta));
		Cam->set_v_offset(Math::lerp((float)Cam->get_v_offset(), 0.f, OffsetLerpBackFactor * delta));

		Cam->set_position(/*Vector3(Math::lerp((double)Cam->get_h_offset(), 0, OffsetLerpBackFactor * delta)
			, Math::lerp((double)Cam->get_v_offset(), 0, OffsetLerpBackFactor * delta), 0) +*/ UPWARDS * Offset);
	}

	CurrentRoll = Math::lerp(CurrentRoll, RollTarget, delta * TiltTimeFactor); 

	Vector3 currRot = Cam->get_rotation(); 
	Cam->set_rotation(Vector3(currRot.x, currRot.y, CurrentRoll));

	Cam->set_fov(Math::lerp(defaultFOV, defaultFOV + maxFOVIncrease, std::clamp(VELMAG()/FOVVelCap, 0.0f, 1.0f)));
}

void Playables::UpdateCapsuleSize()
{
	if (CapsuleBody) {
		if(IsCrouching())
		{
			CapsuleBody->set_height(CrouchHeight);
			Cam->set_position(UPWARDS * ((defaultHeight - CrouchHeight) / 2));
			set_global_position(get_global_position() + DOWNWARDS * (defaultHeight - CrouchHeight)/2);
		}
		else 
		{
			CapsuleBody->set_height(defaultHeight);
			Cam->set_position(DOWNWARDS * ((defaultHeight - CrouchHeight) / 2));
			set_global_position(get_global_position() + UPWARDS * (CrouchHeight)/2);
		}
		UtilityFunctions::print(CapsuleBody->get_height()); 
	}
}

#pragma region Jump/Dash
bool Playables::CheckToJump()
{
	//if (CheckCanClamber()) //ClamberActivation
	//{
	//	bool StartedCrouch = isCrouching();//bWantsToCrouch;
	//	AbleToClamber();
	//	//@todo Clamber 
	//	//		yo fuck clambering rn and fuck future self and you past self
	//	//if (ClamberCheck.bBlockingHit)
	//	//{
	//	//	CoyoteTimeActive = false;
	//	//	Velocity = FVector::ZeroVector;
	//	//	SetMovementMode(MOVE_Walking);
	//	//	PastLoc = UpdatedComponent->GetComponentLocation();
	//	//	bJustTeleported = true;
	//	//	FHitResult hit;

	//	//	bWantsToCrouch = true;
	//	//	PlayableOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(CrouchedHalfHeight, true);
	//	//	PlayableOwner->SetActorLocation(ClamberCheck.Location + FVector::UpVector * (CrouchedHalfHeight + 1));
	//	//	//DrawDebugSphere(GetWorld(), ClamberCheck.Location + FVector::UpVector * (CrouchedHalfHeight + 1), 10, 3, FColor::Red, true, 1);
	//	//	//DrawDebugLine(GetWorld(), GetActorLocation(), (ClamberCheck.Location + FVector::UpVector * (CrouchedHalfHeight + 1)), FColor::Cyan, true, 1.0f, 0, 5.0f);

	//	//	OnWantedToJump.Broadcast();

	//	//	if (PlayableOwner)
	//	//	{
	//	//		FTimerHandle Timer;
	//	//		MaxWalkSpeed = 0;
	//	//		GetWorld()->GetTimerManager().SetTimer(Timer, [this]() {
	//	//			MaxWalkSpeed = Walk_MaxWalkSpeed;
	//	//			}, 1 / Clamber_SpeedFactor, false);
	//	//	}
	//	//	CustomMovementFlags &= 240;
	//	//	BufferingJump = false;
	//	//	BufferingDash = false;
	//	//	CoyoteTimeActive = false;
	//	//	GetWorld()->GetTimerManager().ClearTimer(Safe_CoyoteTime);
	//	//	GetWorld()->GetTimerManager().ClearTimer(Safe_DashInputBuffer);
	//	//	GetWorld()->GetTimerManager().ClearTimer(Safe_JumpInputBuffer);


	//	//	FTimerHandle Timer;
	//	//	GetWorld()->GetTimerManager().SetTimer(Timer, [this]() {
	//	//		CrouchIn(false);
	//	//		}, 0.01, false);

	//	//	return false;
	//	//}
	//}

	if (CheckCanJump())//Jump Inputed
	{
		//ServerTransparency(1, (!Safe_bWantsToJump && Safe_bPrevWantsToJump) ? 1.0 : 0.0); 
		if (is_on_floor() || CoyoteTimeActive || MovementMode == EMovementMode::WallRunning)//(CurrentFloor.bBlockingHit || CoyoteTimeActive || IsCustomMovementMode(ECustomMovementMode::CMOVE_WallRun))
		{

			Vector3 JumpNorm; //FVector
			if (MovementMode != EMovementMode::WallRunning)//(!IsCustomMovementMode(ECustomMovementMode::CMOVE_WallRun)) //normal jump from the floor
			{
				JumpNorm = is_on_floor() ? get_floor_normal() : UPWARDS; //CurrentFloor.bBlockingHit ? CurrentFloor.HitResult.Normal : FVector::UpVector;
				OnGroundJumpBRIDGE();
			}
			else //(IsCustomMovementMode(ECustomMovementMode::CMOVE_WallRun)) //jump from the wall no dash
			{
				//@todo
				JumpNorm = get_wall_normal();//wallhit.Normal;
				OnWallJumpBRIDGE();
			}

			OnJumpDoneBRIDGE();

			return true;
		}

		if (!IsJumping() && WasJumping())
		{
			OnJumpFailedBRIDGE();
			return false;
		}

	}
	// Dash is capped, dash doesn't happen above 15-20ish degrees
	// You can run up wall you shouldn't be able to, get snagged at top
	// I want glass
	// Add back punching even if it has no function

	if (CheckCanDash())//Dash Inputted
	{
		if (is_on_floor() || CoyoteTimeActive || MovementMode == EMovementMode::WallRunning)//(CurrentFloor.bBlockingHit || CoyoteTimeActive || IsCustomMovementMode(ECustomMovementMode::CMOVE_WallRun))
		{
			float BufferedDashValue = VELMAG(); //Velocity.Size()

			if (MovementMode != EMovementMode::WallRunning)//(!IsCustomMovementMode(ECustomMovementMode::CMOVE_WallRun)) //dash on floor or during coyote time
			{
				OnGroundDashBRIDGE();
			}
			else //dash on wall
			{
				OnWallDashBRIDGE();
			}

			//IS THIS IDIOT CODE TF?
			//WHAT IS THE POINT OF THIS?????
				//if (MovementMode == EMovementMode::Sliding)//(IsCustomMovementMode(CMOVE_Slide))
				//{
				//	//BufferSlideClampTime = 0.3;
				//}
				//else
				//{
				//	if (MovementMode == EMovementMode::WallRunning)//(IsCustomMovementMode(CMOVE_WallRun) /*&& !HasCustomGravity()*/)
				//		ExitWallRun();
				//	SetMovementMode(EMovementMode::MOVE_Falling);
				//}
			OnDashDoneBRIDGE();

			return true;
		}

		if (!IsSprinting() && WasSprinting())
		{
			OnDashFailedBRIDGE();
			return false;
		}

		/*
			Checks if the player is looking up enough to freedash
			Checks is the player is not pressing wasd or w

			if the following was true do a free dash (dash in the direction you're looking)
				Gets the control rotation and multiply it by a factor set that as velocity
			if the following was false do a lateral dash (dash fast on the ground in a movement direction)
				Project the acceleration to the plane of the ground and apply the force in the direction of the acceleration
		*/
	}
	return false;
}

void Playables::OnGroundDash()
{	
	float power = std::clamp(GetDashPower(), 0.0f, MaxDashClamp);
	Vector3 Vel = VEL();
	Vel = IsPlayerFreeDashing()						//checks if pressing w or nothing
		? getForwardDir() * power					//All true? then just dash where u looking
		: power * (!InputDirection.is_zero_approx() //: ((FVector::VectorPlaneProject(
			? RelativeInputDirection.slide(get_floor_normal()) : (-get_global_basis().get_column(2)).slide(get_floor_normal())).normalized();
	//UtilityFunctions::print(IsPlayerFreeDashing() ? 0 : !InputDirection.is_zero_approx() ? 1 : 2);
	//UtilityFunctions::print(power);

	set_velocity(Vel);
}

void Playables::OnWallDash()
{
	//float HoldTime = (CustomMovementFlags >> 4) * 0.1;
	//float calcFreeDash = JumpPower * (FMath::Clamp(HoldTime, 0, 1.5) + 0.2);
	//float FreeDashStrength = (MaxDashStrength > calcFreeDash) ?/* Safe_MaxDashTimer > 0*/ MaxDashActive ? MaxDashStrength : Velocity.Size() : calcFreeDash;
	//Velocity = (PlayableOwner->GetControlRotation().Vector() * FreeDashStrength);
	//float calcFreeDash = JumpPower * (FMath::Clamp(HoldTime, 0, 1.5) + 0.2);
	float power = std::clamp(GetDashPower(), 0.0f, MaxDashClamp);
	Vector3 Vel = getForwardDir() * power;

	set_velocity(Vel);
	SetMovementMode(Falling);

	//UtilityFunctions::print(Vel.length());
}

void Playables::OnGroundJump()
{
	float HoldTime = GetCharge(false) * 0.1; //(CustomMovementFlags & 15) * 0.1;
	Vector3 Vel = VEL();
	Vel.y = !is_on_floor() ? std::clamp((float)Vel.y, 0.0f, VELMAG()) : std::clamp((float)Vel.slide(get_floor_normal()).y, 0.0f, VELMAG());//Velocity.Z = !CurrentFloor.bBlockingHit ? FMath::Clamp(Velocity.Z, 0.0f, Velocity.Size()) : FMath::Clamp(FVector::VectorPlaneProject(Velocity, CurrentFloor.HitResult.Normal).Z, 0.0f, Velocity.Size());
	Vel += is_on_floor() ? get_floor_normal() * std::clamp((std::clamp(HoldTime, 0.0f, 1.5f) + 0.4) * JumpPower, (double)JumpClamps.x, (double)JumpClamps.y)//Velocity += CurrentFloor.bBlockingHit ? CurrentFloor.HitResult.Normal * FMath::Clamp((FMath::Clamp(HoldTime, 0.0f, 1.5f) + .4f) * JumpPower, Jump_Clamps.X, Jump_Clamps.Y)
		: std::clamp((std::clamp(HoldTime, 0.0f, 1.5f) + 0.4f) * JumpPower, (float)JumpClamps.x, (float)JumpClamps.y) * UPWARDS + DOWNWARDS * JumpPower * 0.25f * CoyoteSlideRefreshCnt;//: FMath::Clamp((FMath::Clamp(HoldTime, 0.0f, 1.5f) + .4f) * JumpPower, Jump_Clamps.X, Jump_Clamps.Y) * FVector::UpVector + FVector::DownVector * JumpPower * 0.25f * CoyoteSlideRefreshCnt;

	if (!is_on_floor()) CoyoteSlideRefreshCnt++;

	set_velocity(Vel); //kill me 
}

void Playables::OnWallJump()
{
	float HoldTime = GetCharge(false) * 0.1;//(CustomMovementFlags & 15) * 0.1;
	Vector3 Vel = VEL();

	Vel.y = Vel.y < 0 ? 0 : Vel.y; //Velocity.Z = Velocity.Z < 0 ? 0 : Velocity.Z;
	Vel += (get_wall_normal().slide(UPWARDS) * JumpPower * (std::clamp(HoldTime, 0.0f, 1.5f) + .2)
		+ (UPWARDS * (std::clamp(HoldTime, 0.0f, 0.75f) * 1) * JumpPower));
	//Velocity += (wallhit.Normal.GetSafeNormal2D() * WallRun_AttractionForce * (FMath::Clamp(HoldTime, 0.0f, 1.5f) + 1))
			//+ (FVector::UpVector * (FMath::Clamp(HoldTime, 0.0f, 0.75f) * 2) * JumpPower);
	set_velocity(Vel); //kill me 
	SetMovementMode(Falling);
}

void Playables::OnJumpDone()
{
	if (MovementMode == EMovementMode::WallRunning || !(MovementMode == EMovementMode::WallRunning))
		//((IsCustomMovementMode(ECustomMovementMode::CMOVE_WallRun) && CanWallJump) || (!IsCustomMovementMode(ECustomMovementMode::CMOVE_WallRun) && CanJump))
	{
		SetMovementMode(EMovementMode::Falling);
		//@todo volume
			/*float volume = FMath::Clamp((JumpPower.Z - Velocity.Size2D()) / 1500.0f, 0.2f, 0.8f);
			UGameplayStatics::PlaySound2D(this, PlayableOwner->JumpEndSound, volume, FMath::Clamp((0.4f / volume), 0.0f, 1.2f));
			PlayableOwner->OnJumpIsDone(volume);*/


			//@todo CoyoteTime @todo Buffer
			//Note: fuck I hate this
				/*BufferingJump = false;
				CoyoteTimeActive = false;
				GetWorld()->GetTimerManager().ClearTimer(Safe_CoyoteTime);

				GetWorld()->GetTimerManager().ClearTimer(Safe_JumpInputBuffer);*/
		ScreenShake(VELMAG() * GetCharge(false) * .005 + ChargeActionShakeIntensity, ChargeActionShakeTime + VELMAG() * GetCharge(false) * .0005);
		MaxDashStrength = VELMAG();
		ChargeFlags &= 240;
		CoyoteTimeActive = false;
		BufferingJump = false;
		CanCoyoteTimeJump = false;
	}
}

void Playables::OnJumpFailed()
{
	//UtilityFunctions::print("jumpfailed");
	//@todo Buffer
	BufferingDash = false;
	BufferingJump = true;
	InputBuffer->start();

	//GetWorld()->GetTimerManager().SetTimer(Safe_JumpInputBuffer, [this]() {BufferingJump = false; }, BufferTime, false);
}

void Playables::OnDashDone()
{
	//@todo wtf
		////just checks if you actually dashed or could have dashed, stupid workaround I know
		//if (MovementMode == EMovementMode::WallRunning || 
		//	!(MovementMode == EMovementMode::WallRunning)
		//	&& IsPlayerFreeDashing())
		//	/*((IsCustomMovementMode(ECustomMovementMode::CMOVE_WallRun) && CanWallDash) ||
		//	(!IsCustomMovementMode(ECustomMovementMode::CMOVE_WallRun)
		//		&& ((IsPlayerFreeDashing() && CanDashFreely) || CanDashLaterally)))*/
		//{
		//	MaxDashStrength = VELMAG(); //Velocity.Size()
		//	//@todo Volume
		//		//float volume = FMath::Clamp((Velocity.Size() - DashPower) / 1500.0f, 0.2f, 0.8f);
		//		//UGameplayStatics::PlaySound2D(this, PlayableOwner->DashEndSound, volume, FMath::Clamp((0.4f / volume), 0.0f, 1.2f));
		//		//PlayableOwner->OnDashIsDone(volume);
		//	
		//	//@todo Buffer
		//		/*BufferingDash = false;
		//		CoyoteTimeActive = false;
		//		GetWorld()->GetTimerManager().ClearTimer(Safe_CoyoteTime);
		//		GetWorld()->GetTimerManager().ClearTimer(Safe_DashInputBuffer);*/

		//	ChargeFlags &= 15;
		//	UtilityFunctions::print("called"); 
		//}
	ScreenShake(VELMAG() * GetCharge(true) * .005 + ChargeActionShakeIntensity, ChargeActionShakeTime + VELMAG() * GetCharge(true) * .0005);
	MaxDashStrength = VELMAG();
	CanCoyoteTimeJump = false;
	CoyoteTimeActive = false;
	ChargeFlags &= 15;
	BufferingDash = false;
}

void Playables::OnDashFailed()
{
	//UtilityFunctions::print("dashfailed"); 
	//@todo Buffer
	BufferingJump = false;
	BufferingDash = true;
	InputBuffer->start();
	//GetWorld()->GetTimerManager().SetTimer(Safe_JumpInputBuffer, [this]() {BufferingDash = false; }, BufferTime, false);
}
#pragma endregion

//void Playables::AbleToClamber()
//{
//	//@todo Clamber 
//	//Note: yeah fuck you dealing with this later
//		//	float capsuleHH = (PlayableOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
//		//	FVector currLocation = UpdatedComponent->GetComponentLocation() + capsuleHH * FVector::DownVector + FVector::UpVector * MaxStepHeight;
//		//	FVector clamberHeight = currLocation + FVector::UpVector * Clamber_MaxHeight;
//		//	FVector currForward = UpdatedComponent->GetForwardVector() * Clamber_Distance;
//		//	FVector currRight = UpdatedComponent->GetRightVector();
//		//
//		//	TArray<AActor*> IgnoredActors;
//		//	IgnoredActors.Add(PlayableOwner);
//		//	bool canClamber;
//		//	//deprecated code reuse as is no changes needed
//		//	//Looking for better solutions
//		//
//		//
//		//	//Will Look in front of the player if there is a thing there
//		//	canClamber = (GetWorld()->LineTraceSingleByChannel(ClamberCheck, clamberHeight + currForward, currLocation + currForward, ECC_WorldStatic, WallRunParamsQ, WallRunParamsR));
//		//	//DrawDebugLine(GetWorld(), clamberHeight + currForward, currLocation + currForward, FColor::Cyan, true, 100.0f, 0, 5.0f);
//		//
//		//
//		//	if (!canClamber || (ClamberCheck.Normal | FVector::UpVector) < 0.8)
//		//	{
//		//		ClamberCheck.bBlockingHit = false;
//		//		return;
//		//	}
//		//
//		//	float rememberHeight = ClamberCheck.Location.Z;
//		//	FVector rememberNormal = ClamberCheck.Normal;
//		//
//		//	//PlayableCapsuleCanStand(ClamberCheck); 
//		//#pragma region Capsule Trace
//		//	FVector StartLocation = ClamberCheck.Location; // Starting point of the trace
//		//	FVector EndLocation = StartLocation + (capsuleHH + 1) * FVector::UpVector;   // End point of the trace (direction you want to trace)
//		//	float CapsuleRadius = PlayableOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();                // Radius of the capsule
//		//	float CapsuleHalfHeight = capsuleHH;
//		//	//DrawDebugSphere(GetWorld(), ClamberRunResult.Location, 10, 3, FColor::Red, true, 1);
//		//	FHitResult HitResult;
//		//	FHitResult NonInitialHit;
//		//
//		//	// Perform the capsule trace
//		//	bool SpaceOnTop =
//		//		!UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(),
//		//			EndLocation,
//		//			EndLocation,
//		//			CapsuleRadius, CapsuleHalfHeight, UEngineTypes::ConvertToTraceType(ECC_Visibility), true, IgnoredActors, EDrawDebugTrace::None, HitResult, true)
//		//		|| HitResult.PenetrationDepth <= 5
//		//		|| (!UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(),
//		//			(EndLocation + (HitResult.Normal.GetSafeNormal2D() * (HitResult.PenetrationDepth + 3))),
//		//			(EndLocation + (HitResult.Normal.GetSafeNormal2D() * (HitResult.PenetrationDepth + 3))),
//		//			CapsuleRadius, CapsuleHalfHeight, UEngineTypes::ConvertToTraceType(ECC_Visibility), true, IgnoredActors, EDrawDebugTrace::None, NonInitialHit, true)
//		//			&& (GetWorld()->LineTraceSingleByChannel(NonInitialHit,
//		//				(EndLocation + (HitResult.Normal.GetSafeNormal2D() * (HitResult.PenetrationDepth + 3))),
//		//				(EndLocation + (HitResult.Normal.GetSafeNormal2D() * (HitResult.PenetrationDepth + 3))) + FVector::DownVector * (CapsuleHalfHeight + Clamber_MaxHeight / 5),
//		//				ECC_WorldStatic, WallRunParamsQ, WallRunParamsR)
//		//				&& !((NonInitialHit.Normal | FVector::UpVector) < 0.8)));
//		//
//		//	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Value: %i"), SpaceOnTop));
//		//
//		//	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Dist to impact: %f"), (HitResult.Location - HitResult.ImpactPoint).Size()));
//		//	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Pen: %f"), HitResult.PenetrationDepth));
//		//
//		//	/*DrawDebugLine(GetWorld(), (EndLocation + (HitResult.Normal.GetSafeNormal2D() * (HitResult.PenetrationDepth + 3))),
//		//		(EndLocation + (HitResult.Normal.GetSafeNormal2D() * (HitResult.PenetrationDepth + 3))) + FVector::DownVector * (CapsuleHalfHeight + Clamber_MaxHeight / 5), FColor::Cyan, false, .0f, 0, 1.0f);*/
//		//
//		//
//		//		//Checks if crouching makes it able to go
//		//	if (!SpaceOnTop && !bWantsToCrouch)
//		//	{
//		//		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Enter Crouch check Pass"));
//		//
//		//		bool SpaceOnTopCrouched = !UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(),
//		//			ClamberCheck.Location + (FVector::UpVector * (CrouchedHalfHeight + 3)) + FVector(0, 0, 1),
//		//			ClamberCheck.Location + (FVector::UpVector * (CrouchedHalfHeight + 3)),
//		//			CapsuleRadius, CrouchedHalfHeight, UEngineTypes::ConvertToTraceType(ECC_Visibility), true, IgnoredActors, EDrawDebugTrace::None, HitResult, true)
//		//			|| HitResult.PenetrationDepth <= 5
//		//			|| (!UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(),
//		//				(ClamberCheck.Location + (FVector::UpVector * (CrouchedHalfHeight + 3)) + (HitResult.Normal.GetSafeNormal2D() * (HitResult.PenetrationDepth + 1))) + FVector(0, 0, 1),
//		//				(ClamberCheck.Location + (FVector::UpVector * (CrouchedHalfHeight + 3)) + (HitResult.Normal.GetSafeNormal2D() * (HitResult.PenetrationDepth + 1))),
//		//				CapsuleRadius, CrouchedHalfHeight, UEngineTypes::ConvertToTraceType(ECC_Visibility), true, IgnoredActors, EDrawDebugTrace::None, NonInitialHit, true)
//		//				&& (GetWorld()->LineTraceSingleByChannel(NonInitialHit,
//		//					(ClamberCheck.Location + FVector::UpVector * CrouchedHalfHeight + (HitResult.Normal.GetSafeNormal2D() * (HitResult.PenetrationDepth + 1))) + FVector(0, 0, 1),
//		//					(ClamberCheck.Location + FVector::UpVector * CrouchedHalfHeight + (HitResult.Normal.GetSafeNormal2D() * (HitResult.PenetrationDepth + 1))) + FVector::DownVector * (CrouchedHalfHeight + Clamber_MaxHeight / 5),
//		//					ECC_WorldStatic, WallRunParamsQ, WallRunParamsR)
//		//					&& !((NonInitialHit.Normal | FVector::UpVector) < 0.8)));
//		//		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Value: %i"), SpaceOnTop));
//		//
//		//		//DrawDebugLine(GetWorld(),ClamberRunResult.Location + (FVector::UpVector * (CrouchedHalfHeight + 3)),(ClamberRunResult.Location + (FVector::UpVector * (CrouchedHalfHeight + 3)) + ( HitResult.Normal.GetSafeNormal2D() * (HitResult.PenetrationDepth + 1))),FColor::Cyan, false, 1.0f, 0, 1.0f);
//		//
//		//		if (SpaceOnTopCrouched)
//		//		{
//		//			SpaceOnTop = true;
//		//			bWantsToCrouch = true;
//		//		}
//		//		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Value: %i"), SpaceOnTopCrouched));
//		//
//		//	}
//		//#pragma endregion
//		//
//		//	if (!SpaceOnTop || !canClamber)
//		//	{
//		//		ClamberCheck.bBlockingHit = false;
//		//		return;
//		//	}
//		//	canClamber = (GetWorld()->LineTraceSingleByChannel(ClamberCheck, currLocation, ClamberCheck.Location + FVector::DownVector, ECC_WorldStatic, WallRunParamsQ, WallRunParamsR));
//		//	//DrawDebugLine(GetWorld(), currLocation, ClamberCheck.Location + FVector::DownVector, FColor::Cyan, true, 100.0f, 0, 5.0f);
//		//
//		//	//Makes sure the hit of the capsule traces actually is reasonable if it hasn't returned already
//		//	//we return false if 
//		//	//we can't clamber | the normal of the floor isn't facing upwards
//		//	if (!canClamber || (HitResult.Normal | FVector::UpVector) > 0.8)
//		//	{
//		//		ClamberCheck.bBlockingHit = false;
//		//		return;
//		//	}
//		//
//		//	ClamberCheck.Location = FVector(ClamberCheck.Location.X, ClamberCheck.Location.Y, rememberHeight) - ClamberCheck.Normal * 10;
//		//	//DrawDebugLine(GetWorld(), currLocation, FVector(currLocation.X, currLocation.Y, rememberHeight + capsuleHH), FColor::Cyan, true, 1.0f, 0, 5.0f);
//		//	//DrawDebugLine(GetWorld(), FVector(currLocation.X, currLocation.Y, rememberHeight + capsuleHH), ClamberRunResult.Location + rememberNormal * 10, FColor::Cyan, true, 1.0f, 0, 5.0f);
//		//
//		//	//Commence final check so that the player isn't just underneath the platform and theres like a small gap to get in
//		//	if (GetWorld()->LineTraceSingleByChannel(HitResult, currLocation, FVector(currLocation.X, currLocation.Y, rememberHeight + capsuleHH), ECC_WorldStatic, WallRunParamsQ, WallRunParamsR)
//		//		|| GetWorld()->LineTraceSingleByChannel(HitResult, FVector(currLocation.X, currLocation.Y, rememberHeight + capsuleHH), ClamberCheck.Location + rememberNormal * 10, ECC_WorldStatic, WallRunParamsQ, WallRunParamsR))
//		//	{
//		//		ClamberCheck.bBlockingHit = false;
//		//		return;
//		//	}
//		//	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Cases Pass"));
//}
