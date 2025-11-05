using Godot;
using System;

public partial class Function : Label
{
	// Called when the node enters the scene tree for the first time.
	[Export]
	public string _action;
	[Export]
	public Color _activeColor;
	[Export]
	public Color _inactiveColor;
	public bool _active = false;
	public override void _Ready()
    {
		
    }

	// Called every frame. 'delta' is the elapsed time since the previous frame.
	public override void _Process(double delta)
    {
        
    }

	public void SetActive()
	{
		_active = true;
		AddThemeColorOverride("font_color", _activeColor);
	}
	
	public void SetInactive()
    {
		_active = false;
		AddThemeColorOverride("font_color", _inactiveColor);
    }
}
