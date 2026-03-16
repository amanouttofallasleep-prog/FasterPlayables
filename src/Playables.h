#ifndef PLAYABLES_H
#define PLAYABLES_H

#define CFLAG 1
#define JFLAG 2
#define SFLAG 4
#define CUSTOMFLAG1 8
#define CUSTOMFLAG2 16
#define CUSTOMFLAG3 32
#define CUSTOMFLAG4 64
#define CUSTOMFLAG5 128

//Add Camera Shake 
//Walk/Run head bobbing? 

//Add Clamber really don't wanna tbh Edgecases:
	//Make sure you teleport on top of the object
	//make sure that there is a clear path to that object 
	//make sure that the capsule can be on that object without clipping 
	//make sure that the capsule isn't 
	// 
//@todo func

#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/input_map.hpp>
#include <cstdint>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/classes/capsule_shape3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/ray_cast3d.hpp>
#include <godot_cpp/classes/fast_noise_lite.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>
#include <godot_cpp/classes/shape_cast3d.hpp>
#include "BindMacros.h"

enum EMovementMode
{
	  Walking,			//This occurs when you're on the ground that is to say basic movement using wasd.
	  Sliding,			//This occurs when you're crouching while moving faster than the @minSlideVel "drifty" movement using wasd
	  WallRunning,		//This occurs when touching a wall while in midair and higher than @_____. Sticks to a wall and moves using wasd
	  Falling,			//This occurs while in mid-air very little control using wasd based on @_____. 
	  None
};
using namespace godot;

// MAGIC NUMBERS
const float MAX_STEP_SIDE_Z = 0.08f;	// maximum z value for the normal on the vertical side of steps
const float SWIMBOBSPEED = -80.f;
const float VERTICAL_SLOPE_NORMAL_Z = 0.001f; // Slope is vertical if Abs(Normal.Z) <= this threshold. Accounts for precision problems that sometimes angle normals slightly off horizontal for vertical surface.

const float MIN_TICK_TIME = 1e-6f;
const float MIN_FLOOR_DIST = 1.9f;
const float MAX_FLOOR_DIST = 2.4f;
const float BRAKE_TO_STOP_VELOCITY = 10.f;
const float SWEEP_EDGE_REJECT_DISTANCE = 0.15f;
//MAX TIME for a step allowed 
//causes a slowdown for frame times greater than this
const float MAXTIMESTEP = 0.05; // 20 fps

//Cartesian Cardinal upward axis
const Vector3 UPWARDS = Vector3(0, 1, 0);

//Cartesian Cardinal Downward axis
const Vector3  DOWNWARDS = Vector3(0, -1, 0);

//Multiply this to any would be degree to transfer to radians
const float TORAD = (Math_PI / 180.0); 

//the absolute maximum allowed iterations before basically saying fuck it just leave
const int MAXITER = 25;
namespace godot {
	class Playables : public CharacterBody3D {
		GDCLASS(Playables, CharacterBody3D)

	protected:
#pragma region Movement Dynamic Constants
		//base walk speed
		float MaxWalkSpeed = 4.55;
		
		//base sprint speed
		float MaxRunSpeed = 8;
		
		//crouch speed
		float MaxCrouchSpeed = 3;
		
		//The height player assumes while crouching
		float CrouchHeight = 0.5; 
		
		//falling downwards force
		float Gravity = 9.8;
		
		//sliding falling downwawrds force
		float SlideGravity = 16.8; 
		
		//wallrunning falling downwards force
		float WallGravity = 4.9;
		
		//the influence of gravity on a slope while sliding
		float SlideFloorGravityInfluence = 1000; 

		//breaking factor durring walking
		float Deceleration = 12.;
		
		//the friction while sliding and touching the floor
		float SlideFriction = 0.17;
		
		//the friction whlie walking
		float WalkFriction = 2; 
		
		//the kind of "pull" factor for your direction keys (wasd)
		float DirectionDrift = 2; 

		//acceleration factor durring walking
		float Acceleration = 20.;

		//the minimum value to slide
		float minSlideVel = 7.;

		//The power at which you jump AND dash sorry for naming it like this
		float JumpPower = 15.; 

		//the maximum actual power you are able to jump OR dash 
		Vector2 JumpClamps = Vector2(0.0, 15.0); 

		//The control the player while falling
		float AirControlFactor = 0.5;

		//The time that you have with max dash active
		float MaxDashTime = 0.3;

		//The height at which you can wallrun note: tf? 7 meters? 
		float LowerAllowedWall = 7; 

		//how long landing shakes the player by default
		float LandShakeTime = .08; 
		
		//default landing shake intensity
		float LandShakeIntensity = 0.03;
		
		//default jump/dash shake time
		float ChargeActionShakeTime = .05; 
		
		//default jump/dash shake intensity
		float ChargeActionShakeIntensity = .04; 

		//Shake Speed at which you navigate the noise texture
		float ShakeSpeed = 2;

		//The rate at which the shake intensity goes down
		float ShakeDecay = 1;
		
		//The max speed to get the full fov increase
		BIND_GETSET(float, FOVVelCap, 100 / 3.6); 
		//float FOVVelCap = 27.77; 

		//the maximum actual power you are able to jump OR dash 
		//float MaxDashClamp = 100/3.6; 
		BIND_GETSET(float, MaxDashClamp, 100.0 / 3.6);

		BIND_GETSET(float, DefaultSlopeAngle, 45);
		BIND_GETSET(float, AbsoluteMaxAllowedSlopeAngle, 85);

		BIND_GETSET(float, VerticalWallJumpMultiplier, .75); 
		BIND_GETSET(float, LateralWallJumpMultiplier, .75); 

		float CrouchBlendDuration = .05;

#pragma endregion

#pragma region User Adjustables
		//User Adjustables are things that the user can choose to change
		//This is for both accessibility and overrall quality of life 
		//Some user settings will change game mecahnics based on the value you've implemented.

		//Accessibility You can change how much tilt is applied to the camera while on the wall
		float WallTilt = 15.0;

		//Accessibility You can change how much tilt is applied to the camera while walking sideways
		float WalkTilt = 2.5;

		//Change how long your inputs are "out" decreasing this will increase precision however lowers grace
		float BufferTime = 0.3;

		//Change how long coyote time is out, decreasing this will increase precision however lowers grace
		float CoyoteTime = 0.3;

		//the rate at which you charge your jump decreasing this from 0.1 will change the mechanic
		//above or equal to 0.1 your charge will be held at max when it reaches max
		//lower your charge will not be held and you'll have a "misfire" which greatly screws up your input and actually makes you "trip"
		float ChargeIncrements = 0.1;

		//How fast you want the camera to tilt on a wall or while walking
		float TiltTimeFactor = 5; 

		//how much bobbing you want changes the amplitude of the bobbing
		float BobbingFactor = 1; 

		//how much going fast will increase fov
		float maxFOVIncrease = 15;

#pragma endregion

		InputMap* inMap= nullptr; 
		Input* in= nullptr;
		NodePath CamPath;
		Camera3D* Cam= nullptr; 
		CollisionShape3D* CapBody= nullptr; 
		CapsuleShape3D* CapsuleBody= nullptr; 
		NodePath CapPath;

		RayCast3D* GroundCheckRay = nullptr;
		NodePath GroundCheckRayPath;

		/*Area3D* CheckerArea = nullptr;
		NodePath CheckerAreaPath;*/
		ShapeCast3D* CrouchChecking = nullptr;

		Timer* JumpTimer = nullptr;
		Timer* DashTimer = nullptr;
		Timer* InputBuffer = nullptr;
		Timer* CoyoteBuffer = nullptr;
		Timer* MaxDashTimer = nullptr;

		static void _bind_methods();

		//GDVIRTUAL0(_OnGroundDash);

#pragma region Stateful/Input Variables

#pragma region Stateful MovementVars
		
		// what the player inputted this frame
		// each bit represents a different action input mapped as so:
		// custom - custom - custom - custom - custom - sprint - jump - crouch
		uint8_t InputFlags = 0;

		//players previous frame input
		// each bit represents a different action input mapped as so:
		// custom - custom - custom - custom - custom - sprint - jump - crouch
		uint8_t PrevInputFlags = 0;

		//the charge of dash and jump respectively like so :
		// (Dash | Jump)
		// (^^^^ | ^^^^)
		// (0000 | 0000)
		uint8_t ChargeFlags = 0;

		//the current movement physics
		// updated in UpdateCharacterStateBeforeMovement
		//Look at @EMovementMode for definitions for each mode
		EMovementMode MovementMode = EMovementMode::Falling;
		void SetMovementMode(EMovementMode newMode) {
			EMovementMode previousMode = MovementMode;
			MovementMode = newMode;
			OnMovementModeChanged(previousMode);
		}

		//input direction THIS IS NOT RELATIVE TO PLAYER 
		//updated in getInputDirection() which is assumed first in gd script _process()
		Vector2 InputDirection;

		//input direction relative to the player updated in UpdateCharacterStateBeforeMovement
		Vector3 RelativeInputDirection;

		Vector3 PlayerForwardInput;
		Vector3 PlayerRightInput;

		Vector3 LastWallNormal;
		float LastYTouchedWall;

		int CoyoteSlideRefreshCnt = 0;
		bool BufferingJump = false;
		bool BufferingDash = false;
		bool CoyoteTimeActive = false;
		bool CanCoyoteTimeJump = false;
		bool MaxDashActive = false;
		bool PrevFloor = false;
		float MaxDashStrength = 0;

		float currFriction = 0;

		float defaultHeight = 0; 

		float CrouchBlendTime = 0;

		bool canStand = true; 
#pragma endregion

#pragma region CameraFeelVars
		//The Roll we wanna go to
		float RollTarget = 0;

		//The Current roll
		float CurrentRoll = 0;

		//Distance away from Location(0,0,0) keep in mind this is local offset from the parent
		Vector2 CurrentOffset = Vector2();
		//Vector2 ShakeOffset = Vector2();
		//Vector2 CurrentOffset = Vector2();

		//	https://www.youtube.com/watch?v=pG4KGyxQp40 Quality Screen Shake in Godot 4.4 | Game Juice 
		// modifying a little bit

		float ShakeIntensity = 0;
		Timer* ActiveShakeTimer;
		bool isShaking = false; 
		float ShakeTime = 0;
		//cool didn't know this was a thing
		FastNoiseLite* noise;

		//The Camera lwerp 
		float OffsetLerpBackFactor = 10.5; 

		//
		BIND_GETSET(float, defaultFOV, 90); 
		//float defaultFOV = 90;
		BIND_GETSET(bool, IsInputActive, true); 
		//not sure if i really want head bobbing yet
			////based of unreal engine
			////Amplitude of the x and y location axis 
			//Vector2 OscilAmps = Vector2(4, 4); 
			////The period of the oscilation for each axis
			//Vector2 OscilationSpeedFactor = Vector2(5, 2);
		
#pragma endregion

#pragma endregion

	public:
		Playables();
		~Playables();
		//In Playabels this is used to initialize the input bindings and such 
		virtual void _ready() override;
		void _process(double delta) override;
		void _input(const Ref<InputEvent>& event) override;

#pragma region Physic Ticks
		//we're assuming you've called set input direction before this

		/*
		tick proceses 
		UpdateCharacterStateBeforeMovement 
		- sets movement mode
		- sets up movement mode
		PhysTick - moves us to the correct movement logic 
		Movement logic
		- set up while statement for iterations
		- set up the current time simulation
		- movement logic with velocity 
		- use move_and_slide()
		- movement edgecases and maybe switch to another movement state

		//boilerplate for iterative movement
		while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && PlayableOwner && (PlayableOwner->Controller || bRunPhysicsWithNoController || HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity() || (PlayableOwner->GetLocalRole() == ROLE_SimulatedProxy)))
		{
			Iterations++;
			bJustTeleported = false;
			const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
			remainingTime -= timeTick;
		}
		*/

		//helper functions	
		float GetSimulationTimeStep(float remainder, int iteration);

		void PhysTick(float delta, int iteration);   
		void WalkTick(float delta, int iteration); 

		void SlidingTick(float delta, int iteration);
		Vector3 GetSlideDirection(); 

		void FallingTick(float delta, int iteration); 

		void WallRunTick(float delta, int iteration); 

		BIND_BRIDGE_VOID_2( StartNewPhysics, float, deltatime, int, Iteration); 
		//void StartNewPhysics(float deltaTime, int Iterations);
		//void OnMovementModeChanged(EMovementMode PreviousMovementMode);
		BIND_BRIDGE_VOID_1( OnMovementModeChanged, int, PreviousMovementModeINT);

	//	void OnMovementUpdated(float DeltaSeconds, const Vector3& OldLocation, const Vector3& OldVelocity);
	//	BIND_BRIDGE_3(void, OnMovementModeChanged, EMovementMode, PreviousMovementMode);


		//void UpdateCharacterStateBeforeMovement(float deltaSeconds); //this sets the state of the player 
		BIND_BRIDGE_VOID_1( UpdateCharacterStateBeforeMovement, float, deltaSeconds);

		//void UpdateCharacterStateAfterMovement(float deltaSeconds); //this sets the state of the player 
		BIND_BRIDGE_VOID_1( UpdateCharacterStateAfterMovement, float, deltaSeconds);

		//enter and exit physics functions
		void ExitWallRun(); 

#pragma endregion

#pragma region Setter-Getters
		/*RayCast3D* groundCheckRay;
		RayCast3D* wallCheckRay;*/
		NodePath GetGroundCheckRayPath() { return GroundCheckRayPath; }
		void SetGroundCheckRayPath(const NodePath& p_path) { GroundCheckRayPath = p_path; }

		/*NodePath GetCheckerAreaPath() { return CheckerAreaPath; }
		void SetCheckerAreaPath(const NodePath& p_path) { CheckerAreaPath = p_path; }*/

		/*NodePath GetWallCheckRayPath() { return WallCheckRayPath; }
		void SetWallCheckRayPath(const NodePath& p_path) { WallCheckRayPath = p_path; }*/

		float GetCrouchHeight() { return CrouchHeight; }
		void SetCrouchHeight(float newVal) { CrouchHeight = newVal; }

		float GetSlideGravity() { return SlideGravity; }
		void SetSlideGravity(float newVal) { SlideGravity = newVal; }

		float GetWallGravity() { return WallGravity;  }
		void SetWallGravity(float newVal) { WallGravity = newVal; }

		float GetSlideFloorGravityInfluence() { return SlideFloorGravityInfluence; }
		void SetSlideFloorGravityInfluence(float newVal) { SlideFloorGravityInfluence = newVal; }

		float GetSlideFriction() { return SlideFriction; }
		void SetSlideFriction(float newVal) { SlideFriction = newVal; }

		float GetWalkFriction() { return SlideFriction; }
		void SetWalkFriction(float newVal) { WalkFriction = newVal; }

		float GetDirectionDrift() { return DirectionDrift; }
		void SetDirectionDrift(float newVal) { DirectionDrift = newVal; }

		float GetMinSlideVel() { return minSlideVel; }
		void SetMinSlideVel(float newVal) { minSlideVel = newVal; }

		float GetJumpPower() { return JumpPower; }
		void SetJumpPower(float newVal) { JumpPower = newVal; }

		Vector2 GetJumpClamps() { return JumpClamps; }
		void SetJumpClamps(Vector2 newVal) { JumpClamps = newVal; }

		float GetAirControlFactor() { return AirControlFactor; }
		void SetAirControlFactor(float newVal) { AirControlFactor = newVal; }

		float GetMaxDashTime() { return MaxDashTime; }
		void SetMaxDashTime(float newVal) { MaxDashTime = newVal; }

		float GetLowerAllowedWall() { return LowerAllowedWall; }
		void SetLowerAllowedWall(float newVal) { LowerAllowedWall = newVal; }

		float GetLandShakeTime() { return LandShakeTime; }
		void SetLandShakeTime(float newVal) { LandShakeTime = newVal; }

		float GetLandShakeIntensity() { return LandShakeIntensity; }
		void SetLandShakeIntensity(float newVal) { LandShakeIntensity = newVal; }

		float GetChargeActionShakeTime() { return ChargeActionShakeTime; }
		void SetChargeActionShakeTime(float newVal) { ChargeActionShakeTime = newVal; }

		float GetChargeActionShakeIntensity() { return ChargeActionShakeIntensity; }
		void SetChargeActionShakeIntensity(float newVal) { ChargeActionShakeIntensity = newVal; }

		float GetShakeSpeed() { return ShakeSpeed; }
		void SetShakeSpeed(float newVal) { ShakeSpeed = newVal; }

		float GetShakeDecay() { return ShakeDecay; }
		void SetShakeDecay(float newVal) { ShakeDecay = newVal; }

		/*float GetFOVVelCap() { return FOVVelCap; }
		void SetFOVVelCap(float newVal) { FOVVelCap = newVal; }*/

		float GetWallTilt() { return WallTilt; }
		void SetWallTilt(float newVal) { WallTilt = newVal; }

		float GetWalkTilt() { return WalkTilt; }
		void SetWalkTilt(float newVal) { WalkTilt = newVal; }

		float GetCoyoteTime() { return CoyoteTime; }
		void SetCoyoteTime(float newVal) { CoyoteTime = newVal; }

		float GetChargeIncrements() { return ChargeIncrements; }
		void SetChargeIncrements(float newVal) { 
		ChargeIncrements = newVal; 
		JumpTimer->set_wait_time(ChargeIncrements);
		DashTimer->set_wait_time(ChargeIncrements);
		}

		float GetTiltTimeFactor() { return TiltTimeFactor; }
		void SetTiltTimeFactor(float newVal) { TiltTimeFactor = newVal; }

		float GetBobbingFactor() { return BobbingFactor; }
		void SetBobbingFactor(float newVal) { BobbingFactor = newVal; }

		float GetMaxFOVIncrease() { return maxFOVIncrease; }
		void SetMaxFOVIncrease(float newVal) { maxFOVIncrease = newVal; }

		float GetBufferTime() { return BufferTime; }
		void SetBufferTime(float newVal) { BufferTime = newVal; }

#pragma region Input Flag Getters and setters
		void SetCrouchFlag(bool newVal) {
			InputFlags = newVal ? InputFlags | CFLAG : InputFlags >> CFLAG << CFLAG;
			if (newVal != WasCrouching()) UpdateCapsuleSize();
		}
		bool IsCrouching() { return (InputFlags & CFLAG) != 0; }
		bool WasCrouching() { return (PrevInputFlags & CFLAG) != 0; }

		void SetJumpFlag(bool newVal) {
			if (newVal != IsJumping()) SetUpJumpTimer(newVal);
			InputFlags = newVal ? InputFlags | JFLAG : InputFlags & ~(JFLAG);
		}
		bool IsJumping() { return (InputFlags & JFLAG) != 0; }
		bool WasJumping() { return (PrevInputFlags & JFLAG) != 0; }

		void SetSprintFlag(bool newVal) {
			if (newVal != IsSprinting()) SetUpDashTimer(newVal);
			InputFlags = newVal ? InputFlags | SFLAG : InputFlags & ~(SFLAG);
		}
		bool IsSprinting() { return (InputFlags & SFLAG) != 0; }
		bool WasSprinting() { return (PrevInputFlags & SFLAG) != 0; }

		void SetCustomFlag1(bool newVal) {
			if (newVal != IsCustomFlag1()) emit_signal("CustomFlagValSwitched1", newVal);
			InputFlags = newVal ? InputFlags | CUSTOMFLAG1 : InputFlags & ~(CUSTOMFLAG1);
		}
		bool IsCustomFlag1() { return (InputFlags & CUSTOMFLAG1) != 0; }
		bool WasCustomFlag1() { return (PrevInputFlags & CUSTOMFLAG1) != 0; }

		void SetCustomFlag2(bool newVal) {
			if (newVal != IsCustomFlag2()) emit_signal("CustomFlagValSwitched2", newVal);
			InputFlags = newVal ? InputFlags | CUSTOMFLAG2 : InputFlags & ~(CUSTOMFLAG2);
		}
		bool IsCustomFlag2() { return (InputFlags & CUSTOMFLAG2) != 0; }
		bool WasCustomFlag2() { return (PrevInputFlags & CUSTOMFLAG2) != 0; }

		void SetCustomFlag3(bool newVal) {
			if (newVal != IsCustomFlag3()) emit_signal("CustomFlagValSwitched3", newVal);
			InputFlags = newVal ? InputFlags | CUSTOMFLAG3 : InputFlags & ~(CUSTOMFLAG3);
		}
		bool IsCustomFlag3() { return (InputFlags & CUSTOMFLAG3) != 0; }
		bool WasCustomFlag3() { return (PrevInputFlags & CUSTOMFLAG3) != 0; }

		void SetCustomFlag4(bool newVal) {
			if (newVal != IsCustomFlag4()) emit_signal("CustomFlagValSwitched4", newVal);
			InputFlags = newVal ? InputFlags | CUSTOMFLAG4 : InputFlags & ~(CUSTOMFLAG4);
		}
		bool IsCustomFlag4() { return (InputFlags & CUSTOMFLAG4) != 0; }
		bool WasCustomFlag4() { return (PrevInputFlags & CUSTOMFLAG4) != 0; }

		void SetCustomFlag5(bool newVal) {
			if (newVal != IsCustomFlag5()) emit_signal("CustomFlagValSwitched5", newVal);
			InputFlags = newVal ? InputFlags | CUSTOMFLAG5 : InputFlags & ~(CUSTOMFLAG5);
		}
		bool IsCustomFlag5() { return (InputFlags & CUSTOMFLAG5) != 0; }
		bool WasCustomFlag5() { return (PrevInputFlags & CUSTOMFLAG5) != 0; }



		
#pragma endregion



		////////////////////////////////////////////////////////////////////////////////////////
		void SetGravity(float newVal) { Gravity = newVal; }
		float GetGravity() { return Gravity; } 

		void SetCoyoteTimeActive(bool newVal) { CoyoteTimeActive = newVal; }
		bool GetCoyoteTimeActive() { return CoyoteTimeActive; }

		void SetDeceleration(float newVal) { Deceleration = newVal; }
		float GetDeceleration() { return Deceleration; }

		void SetAcceleration(float newVal) { Acceleration = newVal; }
		float GetAcceleration() { return Acceleration; }

		void SetInputDirection(Vector2 inDirection) { InputDirection = inDirection; }
		Vector2 GetInputDirection() { return InputDirection; }

		void SetMaxWalkSpeed(float speed) { MaxWalkSpeed = speed; };
		float GetMaxWalkSpeed() const { return MaxWalkSpeed;  }

		void SetMaxRunSpeed(float speed) { MaxRunSpeed = speed; }
		float GetMaxRunSpeed() const { return MaxRunSpeed; }

		void SetMaxCrouchSpeed(float speed) { MaxCrouchSpeed = speed; }
		float GetMaxCrouchSpeed() const { return MaxCrouchSpeed; }

		Camera3D* GetCam() const { return Cam; }
		void SetCam(Camera3D* new_Cam) { Cam = new_Cam; }

		CollisionShape3D* GetCapBody() const { return CapBody; }
		void SetCapBody(CollisionShape3D* new_Cam) { CapBody = new_Cam; }

		CapsuleShape3D* GetCapsuleBody() const { return CapsuleBody; }
		void SetCapsuleBody(CapsuleShape3D* newCap) { CapsuleBody = newCap; }

		Input* Getinput() const { return in; }
		void SetInput(Input* new_input) { in = new_input; }

		NodePath GetCamPath() { return CamPath; }
		void SetCamPath(const NodePath& p_path) { CamPath = p_path; }

		NodePath GetCapPath() { return CapPath; }
		void SetCapPath(const NodePath& p_path) { CapPath = p_path; }

		inline float VELMAG() const { return get_velocity().length(); }
		inline Vector3 VEL() const { return get_velocity(); }
		void UpdateCapsuleSize();


#pragma endregion

#pragma region Jump/Dash Functions
		BIND_BRIDGE(bool, CheckToJump);
		//bool CheckToJump();
		virtual bool CheckCanJump() { return (!IsJumping() && WasJumping()) || BufferingJump/*@todo Buffer*/; }	//return (!Safe_bWantsToJump && Safe_bPrevWantsToJump) || BufferingJump;
		virtual bool CheckCanDash() { return (!IsSprinting() && WasSprinting()) || BufferingDash/*@todo Buffer && @todo IsPlayerFreeDashing()*/; }
		//return (!Safe_bWantsToSprint && Safe_bPrevWantsToSprint || BufferingDash) && ((IsPlayerFreeDashing() && CanDashFreely) || (!IsPlayerFreeDashing() && CanDashLaterally));

		BIND_BRIDGE_VOID(OnGroundDash);
		BIND_BRIDGE_VOID(OnWallDash);
		BIND_BRIDGE_VOID(OnGroundJump);
		BIND_BRIDGE_VOID(OnWallJump); 
		BIND_BRIDGE_VOID(OnJumpDone);
		BIND_BRIDGE_VOID(OnJumpFailed);
		BIND_BRIDGE_VOID(OnDashDone);
		BIND_BRIDGE_VOID(OnDashFailed);

		/*void OnGroundDash();
		void OnWallDash();
		virtual void OnGroundJump();
		virtual void OnWallJump();
		virtual void OnJumpDone(Vector3 JumpPower);
		virtual void OnJumpFailed();
		virtual void OnDashDone(float DashPower);
		virtual void OnDashFailed(); */
		//virtual bool CheckCanClamber() { return VELMAG() <= MaxRunSpeed && (!IsJumping() && WasJumping()); }
		//void AbleToClamber();

		//BIND_BRIDGE(float, GetDashPower); 
		float GetDashPower();
		//BIND_BRIDGE(bool, IsPlayerFreeDashing);
		bool IsPlayerFreeDashing(); 
		//BIND_BRIDGE(Vector3, getForwardDir);
		Vector3 getForwardDir() const; 
#pragma endregion

		void ChargeAction(bool isDash);

		int GetCharge(bool isDash) { return !isDash ? ChargeFlags & 15 : ChargeFlags >> 4; }

		void init(); 

		void SetUpJumpTimer(bool isStart); 
		void SetUpDashTimer(bool isStart); 

		void InputBufferCall() { BufferingJump = false; BufferingDash = false; }
		void CoyoteBufferCall() { CoyoteTimeActive = false; }
		void MaxDashTimerCall() { MaxDashStrength = VELMAG();  MaxDashActive = false; }
		void ActiveShakeTimerCall() 
		{			
			isShaking = false; 
		}

		BIND_BRIDGE_VOID_2(ScreenShake, float, intensity, float, time);
		

		Vector3 GetMirroredVector(Vector3 a, Vector3 b) { return a - b * (2.f * (a.dot(b))); }

		bool IsPlayerSlidingUphill() { return MovementMode == Sliding && (GetSlideDirection().normalized().slide(get_floor_normal()).dot(VEL()) < 0.0); }
		BIND_BRIDGE_VOID_1(CamUpdate, float, delta);
		//void CamUpdate(float delta); 

		inline Vector3 GetPlayerFoward() { return -get_global_basis().get_column(2); }
		inline Vector3 GetPlayerRight() { return get_global_basis().get_column(0); }

		bool ShouldCatchAir(Vector3 oldNorm, Vector3 newNorm); 

		float Size2D(Vector3 val) { return sqrt(val.x * val.x + val.z * val.z);  }
		Vector3 GetSafeNormal2D(const Vector3& vec) const 
		{
			Vector3 horizontal = Vector3(vec.x, 0, vec.z);
			float len = horizontal.length();

			if (len < 1e-6) {  // Near-zero check
				return Vector3(0, 0, 0).normalized();
			}

			return horizontal / len;
		}

		void BufferStanding();
	};
}

#endif