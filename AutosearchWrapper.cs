using System;

public class AutosearchWrapper
{
	public AutosearchWrapper(Autosearch autosearch)
	{
        InitializeIronPython();
        Dictionary<string, object> locals = new Dictionary<string, object>()
        {
            {"autosearch",autosearch}
        };
        pythonEngine.executeFile("pathfinder.py", pythonEngine.DefaultModule, locals);

	}

    private void InitializeIronPython()
    {
        Options.PrivateBinding = true;
        pythonEngine = new PythonEngine();

        MessageBoxStream s = new MessageBoxStream();

        pythonEngine.SetStandardError(s);
        pythonEngine.SetStandardOutput(s);

        pythonEngine.AddToPath(AppDomain.CurrentDomain.BaseDirectory);

        //pythonEngine.Globals.Add("form", this);
        //pythonEngine.Globals.Add("bl", bl);
    }

    private PythonEngine pythonEngine;
    private Autosearch autosearch;
}
