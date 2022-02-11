https://superuser.com/questions/416182/control-sequence-of-startup-programs-in-windows/777417
his is the method I use:

Go to: %APPDATA%\Microsoft\Windows\Start Menu\Programs\Startup

Startup directory is also under Start > All Programs
You can also use Win+r and type shell:startup
Create a shortcut of the .exe to launch at startup, then cut/paste it into Startup


https://stackoverflow.com/questions/16790287/programmatically-changing-the-presentation-display-mode
 private void SetDisplayMode(DisplayMode mode)
        {
            var proc = new Process();
            proc.StartInfo.FileName = "DisplaySwitch.exe";
            switch (mode)
            {
                case DisplayMode.External:
                    proc.StartInfo.Arguments = "/external";
                    break;
                case DisplayMode.Internal:
                    proc.StartInfo.Arguments = "/internal";
                    break;
                case DisplayMode.Extend:
                    proc.StartInfo.Arguments = "/extend";
                    break;
                case DisplayMode.Duplicate:
                    proc.StartInfo.Arguments = "/clone";
                    break;
            }
            proc.Start();
        }
        enum DisplayMode
        {
            Internal,
            External,
            Extend,
            Duplicate
        }

