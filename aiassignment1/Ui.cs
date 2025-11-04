using Godot;
using System;
using System.Collections.Generic;
using System.Diagnostics;

public partial class Ui : Control
{
	// Called when the node enters the scene tree for the first time.
	[Export]
	public Function[] _functions;
	[Export]
	public int _activeFunction;
	public override void _Ready()
    {
        for(int i = 0; i < _functions.Length; i++)
        {
			if (i == _activeFunction)
			{
				_functions[i].SetActive();
			}
			else
            {
				_functions[i].SetInactive();
            }
        }
    }

	// Called every frame. 'delta' is the elapsed time since the previous frame.
	public override void _Process(double delta)
	{
		for(int i = 0; i < _functions.Length; i++)
        {
			if(Input.IsActionJustPressed(_functions[i]._action))
            {
				_functions[i].SetActive();
				_activeFunction = i;
				for(int j = 0; j < _functions.Length; j++)
                {
                    if(j != i && _functions[j]._active == true)
                    {
						_functions[j].SetInactive();
                    }
                }
            }
        }
    }
}
