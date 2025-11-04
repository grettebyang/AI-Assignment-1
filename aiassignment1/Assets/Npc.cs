using Godot;
using System;
using System.Collections;
using System.Diagnostics;
using System.Numerics;

public partial class Npc : CharacterBody3D
{
    [Export]
    public float _maxSpeed { get; set; } = 0;

    [Export]
    public float _maxAcc { get; set; } = 0;

    [Export]
    public float _fallAcceleration { get; set; } = 75;

    [Export]
    public Node3D _target;

    [Export]
    public Ui _modeController;

    private Godot.Vector3 _targetVelocity = Godot.Vector3.Zero;

    public override void _Ready()
    {
        GD.Randomize();
    }

    public override void _PhysicsProcess(double delta)
    {
        var direction = Godot.Vector3.Zero;
        if (Input.IsActionJustPressed("randomize_pos"))
        {
            var random = new RandomNumberGenerator();
            random.Randomize();
            Godot.Vector3 newPos = new Godot.Vector3();
            newPos.X = random.RandfRange(-19f, 19f);
            newPos.Z = random.RandfRange(-17f, 17f);
            newPos.Y = Position.Y;
            Position = newPos;

            switch (_modeController._activeFunction)
            {
                case 0: _targetVelocity = Seek().velocity; break;
                case 1: _targetVelocity = Flee().velocity; break;
                case 2: _targetVelocity = Seek().velocity; break;
                case 3: _targetVelocity = Seek().velocity; break;
                case 4: _targetVelocity = Seek().velocity; break;
                case 5: _targetVelocity = Seek().velocity; break;
                default: break;
            }
        }
        
        switch(_modeController._activeFunction)
        {
            case 0 : _targetVelocity = SeekSteering().linear; break;
            case 1 : _targetVelocity = FleeSteering().linear; break;
            case 2 : break;
            case 3 : break;
            case 4 : _targetVelocity = Arrive().velocity; break;
            case 5 : _targetVelocity = Wander().velocity; break;
            default : break;
        } 

        //Vertical Velocity
        if (IsOnFloor())
        {
            _targetVelocity.Y = 0;
        }
        else
        {
            _targetVelocity.Y -= _fallAcceleration * (float)delta;
        }

        if(_targetVelocity.Length() > _maxSpeed)
        {
            _targetVelocity.Normalized();
            _targetVelocity *= _maxSpeed;
        }
        Velocity = _targetVelocity;
        MoveAndSlide();
    }

    public Basis NewOrientation(Basis curOrientation, Godot.Vector3 velocity)
    {
        if (velocity.Length() > 0)
        {
			Basis bas = Basis.LookingAt(velocity.Normalized());
			bas.X = bas.X.Normalized();
			bas.Y = bas.Y.Normalized();
			bas.Z = bas.Z.Normalized();

            return bas;
        }
        else
        {
            return curOrientation;
        }
    }

    public KinematicSteeringOutput Seek()
    {
        KinematicSteeringOutput result = new KinematicSteeringOutput();

        //Get direction to target
        result.velocity = _target.Position - Position;

        result.velocity.Normalized();
        result.velocity *= _maxSpeed;

        GetNode<Node3D>("Pivot").Basis = GetNode<Node3D>("Pivot").Basis.Slerp(NewOrientation(Basis, result.velocity), .5f);

        result.rotation = 0;
        return result;
    }

    public SteeringOutput SeekSteering()
    {
        SteeringOutput result = new SteeringOutput();

        //Get direction to target
        result.linear = _target.Position - Position;

        result.linear.Normalized();
        result.linear *= _maxAcc;

        //GetNode<Node3D>("Pivot").Basis = GetNode<Node3D>("Pivot").Basis.Slerp(NewOrientation(Basis, result.linear), .5f);

        result.angular = 0;
        return result;
    }

    public KinematicSteeringOutput Flee()
    {
        KinematicSteeringOutput result = new KinematicSteeringOutput();

        //Get direction to target
        result.velocity = Position - _target.Position;

        result.velocity.Normalized();
        result.velocity *= _maxSpeed;

        GetNode<Node3D>("Pivot").Basis = GetNode<Node3D>("Pivot").Basis.Slerp(NewOrientation(Basis, result.velocity), .5f);

        result.rotation = 0;
        return result;
    }

    public SteeringOutput FleeSteering()
    {
        SteeringOutput result = new SteeringOutput();

        //Get direction to target
        result.linear = Position - _target.Position;

        result.linear.Normalized();
        result.linear *= _maxSpeed;

        GetNode<Node3D>("Pivot").Basis = GetNode<Node3D>("Pivot").Basis.Slerp(NewOrientation(Basis, result.linear), .5f);

        result.angular = 0;
        return result;
    }

    public KinematicSteeringOutput Arrive()
    {
        KinematicSteeringOutput result = new KinematicSteeringOutput();

        //Get direction to target
        result.velocity = _target.Position - Position;

        if (result.velocity.Length() < 2f)
        {
            return null;
        }

        result.velocity /= .25f;

        if (result.velocity.Length() > _maxSpeed)
        {
            result.velocity.Normalized();
            result.velocity *= _maxSpeed;
        }

        GetNode<Node3D>("Pivot").Basis = GetNode<Node3D>("Pivot").Basis.Slerp(NewOrientation(Basis, result.velocity), .5f);

        result.rotation = 0;
        return result;
    }
    
    public KinematicSteeringOutput Wander()
    {
        KinematicSteeringOutput result = new KinematicSteeringOutput();

        result.velocity = _maxSpeed * (-1) * GetNode<Node3D>("Pivot").Basis.Z;

        var random = new RandomNumberGenerator();
        random.Randomize();
        //GetNode<Node3D>("Pivot").Basis = GetNode<Node3D>("Pivot").Basis.Slerp(NewOrientation(Basis, result.velocity), .5f);
        result.rotation = random.RandfRange(-1f, 1f) * .25f;
        GetNode<Node3D>("Pivot").Basis = GetNode<Node3D>("Pivot").Basis.Slerp(GetNode<Node3D>("Pivot").Basis.Rotated(Godot.Vector3.Up, result.rotation).Orthonormalized(), .5f);
        return result;
    }


}
public class KinematicSteeringOutput()
{
    public Godot.Vector3 velocity;
    public float rotation;
}

public class SteeringOutput()
{
    public Godot.Vector3 linear;
    public float angular;
}
