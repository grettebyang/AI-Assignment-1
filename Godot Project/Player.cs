using Godot;
using System;

public partial class Player : CharacterBody3D
{
	[Export]
	public float Speed { get; set; } = 0;
	
	[Export]
	public float FallAcceleration { get; set; } = 75;

	[Export]
	public float Acc { get; set; } = 1;

	[Export]
	public float Ease { get; set; } = .1f;

	[Export]
	public float Dec { get; set; } = 1;

	[Export]
	private Node3D cameraNode;
	
	private Godot.Vector3 _targetVelocity = Godot.Vector3.Zero;
	
	private int _moveToggle = 1;

	private Godot.Vector3 prevDir = Godot.Vector3.Zero;
	
	private bool jumped = false;
	private bool inAir = false;

	private float squashTime = 0.0f;
	private float airTime = 0.0f;
	private float coyoteTime = 0.0f;

	private int crouched = -1;
	private float maxSpeed = 15.0f;
	private Godot.Vector3 halfScale = new Godot.Vector3(1.0f, 0.5f, 1.0f);
	private Godot.Vector3 fullScale = new Godot.Vector3(1.0f, 2.0f, 1.0f);

	public override void _PhysicsProcess(double delta)
	{
		var direction = Godot.Vector3.Zero;

		direction.X = Input.GetActionStrength("move_right") - Input.GetActionStrength("move_left");
		direction.Z = Input.GetActionStrength("move_back") - Input.GetActionStrength("move_forward");

		// Rotate the direction vector
		direction = cameraNode.Basis * direction;
		direction.Y = 0.0f;

		//DebugDraw3D.DrawArrow(Godot.Vector3.Zero,cameraNode.Basis.X * 5,Colors.Red,0.5f,false,0.01f);
		//DebugDraw3D.DrawArrow(Godot.Vector3.Zero,cameraNode.Basis.Y * 5,Colors.Green,0.5f,false,0.01f);
		//DebugDraw3D.DrawArrow(Godot.Vector3.Zero,cameraNode.Basis.Z * 5,Colors.Blue,0.5f,false,0.01f);
		
		if(direction != Godot.Vector3.Zero)
		{
			direction = direction.Normalized();
			Godot.Basis bas = Basis.LookingAt(direction);
			bas.X = bas.X.Normalized();
			bas.Y = bas.Y.Normalized();
			bas.Z = bas.Z.Normalized();
			GetNode<Node3D>("Pivot").Basis = GetNode<Node3D>("Pivot").Basis.Slerp(bas, .5f);
			switch(_moveToggle)
			{
				case 1: Speed = maxSpeed; break;
				case 2: Speed = Math.Min(maxSpeed, Speed + Acc); break;
				case 3: Acc = Math.Max(0, Acc - Ease); Speed = Math.Min(maxSpeed, Speed + Acc); break;
				default: break;
			}
		}
		else 
		{
			//Speed = Math.Max(0, Speed - Dec);
			switch(_moveToggle)
			{
				case 1: Speed = 0; break;
				case 2: Speed = Math.Max(0, Speed - Dec); break;
				case 3: Speed = Math.Max(0, Speed - Dec); Acc = 3.0f; break;
				default: break;
			}
			//Ground velocity
			direction = _targetVelocity.Normalized();
		}
		
		//Ground velocity
		_targetVelocity.X = direction.X * Speed;
		_targetVelocity.Z = direction.Z * Speed;

		//Vertical velocity
		
		if(IsOnFloor() || coyoteTime <= .1)
		{
			if(!jumped && Input.IsActionJustPressed("jump") && crouched < 0)
			{
				jumped = true;
				//Squash the character
				Scale *= halfScale;
			}
			if(jumped)
			{
				squashTime += (float)delta;
				if(squashTime >= .1)
				{
					//Make it jump
					_targetVelocity.Y = 70;
					squashTime = 0.0f;
					Scale *= fullScale;
					coyoteTime += .2f;
					jumped = false;
					inAir = true;
				}
			}
			else if(IsOnFloor())
			{
				coyoteTime = 0;
				inAir = false;
				airTime = 0.0f;
			}
			else
			{
				coyoteTime += (float)delta;
				_targetVelocity.Y = 0;
			}
		}
		else
		{
			if(inAir && Velocity.Y <= 0)
			{
				airTime += (float)delta;
				if(airTime >= .2)
				{
					inAir = false;
					airTime = 0.0f;
				}
			}
			else
			{
				_targetVelocity.Y -= FallAcceleration * (float)delta;
			}
		}
		
		//Moving the character
		Velocity = _targetVelocity;
		MoveAndSlide();
	}
}
