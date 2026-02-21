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

//completed

//todo
//Add Camera Shake 
	// Should be able to happen everytime you land
	// When you dash
	// When you jump
	// 
//Walk/Run head bobbing? 

//Add Clamber really don't wanna tbh Edgecases:
	//Make sure you teleport on top of the object
	//make sure that there is a clear path to that object 
	//make sure that the capsule can be on that object without clipping 
	//make sure that the capsule isn't 

//@todo Buffer
//@todo CoyoteTime
//@todo Clamber
//@todo func


//add physics tick functions specified in MovementModes
//add jumping 
//add crouching 
//add sprinting
//add wall running and sliding and all that fun stuff
//add gaddamn clambering


//polish
//Input buffering 
//Coyote time
//fov stretching
//camera shaking, running sliding up hill and what not
//settings and what not

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
const double MAX_STEP_SIDE_Z = 0.08f;	// maximum z value for the normal on the vertical side of steps
const double SWIMBOBSPEED = -80.f;
const double VERTICAL_SLOPE_NORMAL_Z = 0.001f; // Slope is vertical if Abs(Normal.Z) <= this threshold. Accounts for precision problems that sometimes angle normals slightly off horizontal for vertical surface.

const double MIN_TICK_TIME = 1e-6f;
const double MIN_FLOOR_DIST = 1.9f;
const double MAX_FLOOR_DIST = 2.4f;
const double BRAKE_TO_STOP_VELOCITY = 10.f;
const double SWEEP_EDGE_REJECT_DISTANCE = 0.15f;
//MAX TIME for a step allowed 
//causes a slowdown for frame times greater than this
const double MAXTIMESTEP = 0.05; // 20 fps

//Cartesian Cardinal upward axis
const Vector3 UPWARDS = Vector3(0, 1, 0);

//Cartesian Cardinal Downward axis
const Vector3  DOWNWARDS = Vector3(0, -1, 0);

//Multiply this to any would be degree to transfer to radians
const double TORAD = (Math_PI / 180.0); 

//the absolute maximum allowed iterations before basically saying fuck it just leave
const int MAXITER = 25;
namespace godot {
	class Playables : public CharacterBody3D {
		GDCLASS(Playables, CharacterBody3D)

	protected:
#pragma region Movement Dynamic Constants
		//base walk speed
		double MaxWalkSpeed = 6;
		
		//base sprint speed
		double MaxRunSpeed = 16;
		
		//crouch speed
		double MaxCrouchSpeed = 3;
		
		//The height player assumes while crouching
		double CrouchHeight = 0.5; 
		
		//falling downwards force
		double Gravity = 9.8;
		
		//sliding falling downwawrds force
		double SlideGravity = 16.8; 
		
		//wallrunning falling downwards force
		double WallGravity = 4.9;
		
		//the influence of gravity on a slope while sliding
		double SlideFloorGravityInfluence = 1000; 

		//breaking factor durring walking
		double Deceleration = 12.;
		
		//the friction while sliding and touching the floor
		double SlideFriction = 0.17;
		
		//the friction whlie walking
		double WalkFriction = 2; 
		
		//the kind of "pull" factor for your direction keys (wasd)
		double DirectionDrift = 2; 

		//acceleration factor durring walking
		double Acceleration = 20.;

		//the minimum value to slide
		double minSlideVel = 7.;

		//The power at which you jump AND dash sorry for naming it like this
		double JumpPower = 15.; 

		//the maximum actual power you are able to jump OR dash again im so if i PMO
		Vector2 JumpClamps = Vector2(0.0, 15.0); 

		//The control the player while falling
		double AirControlFactor = 0.5;

		//The time that you have with max dash active
		double MaxDashTime = 0.3;

		//The height at which you can wallrun note: tf? 7 meters? 
		double LowerAllowedWall = 7; 

		//how long landing shakes the player by default
		double LandShakeTime = .08; 
		
		//default landing shake intensity
		double LandShakeIntensity = 0.03;
		
		//default jump/dash shake time
		double ChargeActionShakeTime = .05; 
		
		//default jump/dash shake intensity
		double ChargeActionShakeIntensity = .04; 

		//Shake Speed at which you navigate the noise texture
		double ShakeSpeed = 2;

		//The rate at which the shake intensity goes down
		double ShakeDecay = 1;
		
		//The max speed to get the full fov increase
		double FOVVelCap = 50; 
#pragma endregion

#pragma region User Adjustables
		//User Adjustables are things that the user can choose to change
		//This is for both accessibility and overrall quality of life 
		//Some user settings will change game mecahnics based on the value you've implemented.

		//Accessibility You can change how much tilt is applied to the camera while on the wall
		double WallTilt = 15.0;

		//Accessibility You can change how much tilt is applied to the camera while walking sideways
		double WalkTilt = 2.5;

		//Change how long your inputs are "out" decreasing this will increase precision however lowers grace
		double BufferTime = 0.3;

		//Change how long coyote time is out, decreasing this will increase precision however lowers grace
		double CoyoteTime = 0.3;

		//the rate at which you charge your jump decreasing this from 0.1 will change the mechanic
		//above or equal to 0.1 your charge will be held at max when it reaches max
		//lower your charge will not be held and you'll have a "misfire" which greatly screws up your input and actually makes you "trip"
		double ChargeIncrements = 0.1;

		//How fast you want the camera to tilt on a wall or while walking
		double TiltTimeFactor = 5; 

		//how much bobbing you want changes the amplitude of the bobbing
		double BobbingFactor = 1; 

		//how much going fast will increase fov
		double maxFOVIncrease = 15;

#pragma endregion

		InputMap* inMap; 
		Input* in;
		NodePath CamPath;
		Camera3D* Cam; 
		CollisionShape3D* CapBody; 
		CapsuleShape3D* CapsuleBody; 
		NodePath CapPath;

		RayCast3D* groundCheckRay;

		Timer* JumpTimer;
		Timer* DashTimer;
		Timer* InputBuffer;
		Timer* CoyoteBuffer;
		Timer* MaxDashTimer;

		static void _bind_methods();

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
		double MaxDashStrength = 0;

		double currFriction = 0;
#pragma endregion

#pragma region CameraFeelVars
		//The Roll we wanna go to
		double RollTarget = 0;

		//The Current roll
		double CurrentRoll = 0;

		//Distance away from Location(0,0,0) keep in mind this is local offset from the parent
		Vector2 CurrentOffset = Vector2();
		//Vector2 ShakeOffset = Vector2();
		//Vector2 CurrentOffset = Vector2();

		//	https://www.youtube.com/watch?v=pG4KGyxQp40 Quality Screen Shake in Godot 4.4 | Game Juice 
		// modifying a little bit

		double ShakeIntensity = 0;
		Timer* ActiveShakeTimer;
		bool isShaking = false; 
		double ShakeTime = 0;
		//cool didn't know this was a thing
		FastNoiseLite* noise;

		//The Camera lwerp 
		double OffsetLerpBackFactor = 10.5; 

		//
		double defaultFOV = 90;

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
		double GetSimulationTimeStep(double remainder, int iteration);

		void PhysTick(double delta, int iteration);   
		void WalkTick(double delta, int iteration); 

		void SlidingTick(double delta, int iteration);
		Vector3 GetSlideDirection(); 

		void FallingTick(double delta, int iteration); 

		void WallRunTick(double delta, int iteration); 


		void StartNewPhysics(double deltaTime, int Iterations);
		void OnMovementModeChanged(EMovementMode PreviousMovementMode);
		void OnMovementUpdated(double DeltaSeconds, const Vector3& OldLocation, const Vector3& OldVelocity);
		void UpdateCharacterStateBeforeMovement(double deltaSeconds); //this sets the state of the player 
		void UpdateCharacterStateAfterMovement(double deltaSeconds); //this sets the state of the player 

		//enter and exit physics functions
		void ExitWallRun(); 

#pragma endregion

#pragma region Setter-Getters

		double GetCrouchHeight() { return CrouchHeight; }
		void SetCrouchHeight(double newVal) { CrouchHeight = newVal; }

		double GetSlideGravity() { return SlideGravity; }
		void SetSlideGravity(double newVal) { SlideGravity = newVal; }

		double GetWallGravity() { return WallGravity;  }
		void SetWallGravity(double newVal) { WallGravity = newVal; }

		double GetSlideFloorGravityInfluence() { return SlideFloorGravityInfluence; }
		void SetSlideFloorGravityInfluence(double newVal) { SlideFloorGravityInfluence = newVal; }

		double GetSlideFriction() { return SlideFriction; }
		void SetSlideFriction(double newVal) { SlideFriction = newVal; }

		double GetWalkFriction() { return SlideFriction; }
		void SetWalkFriction(double newVal) { WalkFriction = newVal; }

		double GetDirectionDrift() { return DirectionDrift; }
		void SetDirectionDrift(double newVal) { DirectionDrift = newVal; }

		double GetMinSlideVel() { return minSlideVel; }
		void SetMinSlideVel(double newVal) { minSlideVel = newVal; }

		double GetJumpPower() { return JumpPower; }
		void SetJumpPower(double newVal) { JumpPower = newVal; }

		Vector2 GetJumpClamps() { return JumpClamps; }
		void SetJumpClamps(Vector2 newVal) { JumpClamps = newVal; }

		double GetAirControlFactor() { return AirControlFactor; }
		void SetAirControlFactor(double newVal) { AirControlFactor = newVal; }

		double GetMaxDashTime() { return MaxDashTime; }
		void SetMaxDashTime(double newVal) { MaxDashTime = newVal; }

		double GetLowerAllowedWall() { return LowerAllowedWall; }
		void SetLowerAllowedWall(double newVal) { LowerAllowedWall = newVal; }

		double GetLandShakeTime() { return LandShakeTime; }
		void SetLandShakeTime(double newVal) { LandShakeTime = newVal; }

		double GetLandShakeIntensity() { return LandShakeIntensity; }
		void SetLandShakeIntensity(double newVal) { LandShakeIntensity = newVal; }

		double GetChargeActionShakeTime() { return ChargeActionShakeTime; }
		void SetChargeActionShakeTime(double newVal) { ChargeActionShakeTime = newVal; }

		double GetChargeActionShakeIntensity() { return ChargeActionShakeIntensity; }
		void SetChargeActionShakeIntensity(double newVal) { ChargeActionShakeIntensity = newVal; }

		double GetShakeSpeed() { return ShakeSpeed; }
		void SetShakeSpeed(double newVal) { ShakeSpeed = newVal; }

		double GetShakeDecay() { return ShakeDecay; }
		void SetShakeDecay(double newVal) { ShakeDecay = newVal; }

		double GetFOVVelCap() { return FOVVelCap; }
		void SetFOVVelCap(double newVal) { FOVVelCap = newVal; }

		double GetWallTilt() { return WallTilt; }
		void SetWallTilt(double newVal) { WallTilt = newVal; }

		double GetWalkTilt() { return WalkTilt; }
		void SetWalkTilt(double newVal) { WalkTilt = newVal; }

		double GetCoyoteTime() { return CoyoteTime; }
		void SetCoyoteTime(double newVal) { CoyoteTime = newVal; }

		double GetChargeIncrements() { return ChargeIncrements; }
		void SetChargeIncrements(double newVal) { ChargeIncrements = newVal; }

		double GetTiltTimeFactor() { return TiltTimeFactor; }
		void SetTiltTimeFactor(double newVal) { TiltTimeFactor = newVal; }

		double GetBobbingFactor() { return BobbingFactor; }
		void SetBobbingFactor(double newVal) { BobbingFactor = newVal; }

		double GetMaxFOVIncrease() { return maxFOVIncrease; }
		void SetMaxFOVIncrease(double newVal) { maxFOVIncrease = newVal; }

		double GetBufferTime() { return BufferTime; }
		void SetBufferTime(double newVal) { BufferTime = newVal; }

		void SetCrouchFlag(bool newVal) {
			if (newVal != IsCrouching()) UpdateCapsuleSize();
			InputFlags = newVal ? InputFlags | CFLAG : InputFlags >> CFLAG << CFLAG;
		}
		bool IsCrouching() { return (InputFlags & CFLAG) != 0; }
		bool WasCrouching() { return (PrevInputFlags & CFLAG) != 0; }

		void SetJumpFlag(bool newVal) {
			if (newVal != IsJumping())
				SetUpJumpTimer(newVal);
			InputFlags = newVal ? InputFlags | JFLAG : InputFlags & ~(JFLAG);
		}
		bool IsJumping() { return (InputFlags & JFLAG) != 0; }
		bool WasJumping() { return (PrevInputFlags & JFLAG) != 0; }

		void SetSprintFlag(bool newVal) {
			if (newVal != IsSprinting())
				SetUpDashTimer(newVal);
			InputFlags = newVal ? InputFlags | SFLAG : InputFlags & ~(SFLAG);
		}
		bool IsSprinting() { return (InputFlags & SFLAG) != 0; }
		bool WasSprinting() { return (PrevInputFlags & SFLAG) != 0; }

		////////////////////////////////////////////////////////////////////////////////////////
		void SetGravity(double newVal) { Gravity = newVal; }
		double GetGravity() { return Gravity; } 

		void SetCoyoteTimeActive(bool newVal) { CoyoteTimeActive = newVal; }
		bool GetCoyoteTimeActive() { return CoyoteTimeActive; }

		void SetDeceleration(double newVal) { Deceleration = newVal; }
		double GetDeceleration() { return Deceleration; }

		void SetAcceleration(double newVal) { Acceleration = newVal; }
		double GetAcceleration() { return Acceleration; }

		void SetInputDirection(Vector2 inDirection) { InputDirection = inDirection; }
		Vector2 GetInputDirection() { return InputDirection; }

		void SetMaxWalkSpeed(double speed) { MaxWalkSpeed = speed; };
		double GetMaxWalkSpeed() const { return MaxWalkSpeed;  }

		void SetMaxRunSpeed(double speed) { MaxRunSpeed = speed; }
		double GetMaxRunSpeed() const { return MaxRunSpeed; }

		void SetMaxCrouchSpeed(double speed) { MaxCrouchSpeed = speed; }
		double GetMaxCrouchSpeed() const { return MaxCrouchSpeed; }

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

		inline double VELMAG() const { return get_velocity().length(); }
		inline Vector3 VEL() const { return get_velocity(); }
		void UpdateCapsuleSize();


#pragma endregion

#pragma region Jump/Dash Functions
		bool CheckToJump();
		virtual bool CheckCanJump() { return (!IsJumping() && WasJumping()) || BufferingJump/*@todo Buffer*/; }	//return (!Safe_bWantsToJump && Safe_bPrevWantsToJump) || BufferingJump;
		virtual bool CheckCanDash() { return (!IsSprinting() && WasSprinting()) || BufferingDash/*@todo Buffer && @todo IsPlayerFreeDashing()*/; }
		//return (!Safe_bWantsToSprint && Safe_bPrevWantsToSprint || BufferingDash) && ((IsPlayerFreeDashing() && CanDashFreely) || (!IsPlayerFreeDashing() && CanDashLaterally));

		virtual void OnGroundDash();
		virtual void OnWallDash();
		virtual void OnGroundJump();
		virtual void OnWallJump();
		virtual void OnJumpDone(Vector3 JumpPower);
		virtual void OnJumpFailed();
		virtual void OnDashDone(float DashPower);
		virtual void OnDashFailed();
		virtual bool CheckCanClamber() { return VELMAG() <= MaxRunSpeed && (!IsJumping() && WasJumping()); }
		//return Velocity.Size() <= Sprint_MaxWalkSpeed && (!Safe_bWantsToJump && Safe_bPrevWantsToJump);
		void AbleToClamber();

		double GetDashPower();
		bool IsPlayerFreeDashing(); 

		Vector3 getForwardDir() const; 
#pragma endregion


		void ChargeAction(bool isDash);

	/*	void chargeDash() {
			if ((ChargeFlags >> 4 != 15)) 	ChargeFlags = (((ChargeFlags >> 4) + 1) << 4) + (ChargeFlags & 15);
		}
		void chargeJump() {
			if (((ChargeFlags & 15) != 15)) ChargeFlags = ((ChargeFlags & 15) + 1) + (ChargeFlags & 240);
		}*/
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

		void ScreenShake(double intensity, double time)
		{
			if (noise) {
				noise->set_seed(rand()); 
				noise->set_frequency(2.0); //why is this const? 
			}

			ShakeIntensity = abs(intensity); 
			if(ActiveShakeTimer)
			{
				ActiveShakeTimer->set_wait_time(time); 
				isShaking = true; 
				ActiveShakeTimer->start(); 
			}
			//UtilityFunctions::print("Intensity: ", ShakeIntensity, " sec: ", time);
			ShakeTime = 0; 
		}

		Vector3 GetMirroredVector(Vector3 a, Vector3 b) { return a - b * (2.f * (a.dot(b))); }

		bool IsPlayerSlidingUphill() { return MovementMode == Sliding && (GetSlideDirection().normalized().slide(get_floor_normal()).dot(VEL()) < 0.0); }

		void CamUpdate(double delta); 

		Vector3 GetPlayerFoward() { return -get_global_basis().get_column(2); }
		Vector3 GetPlayerRight() { return get_global_basis().get_column(0); }

	};
}

#endif