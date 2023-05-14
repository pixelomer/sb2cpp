# sb2cpp

A tool for transpiling [Microsoft Small Basic](https://smallbasic-publicwebsite.azurewebsites.net/) into C++. A standard library implementation and convenience scripts are also provided to make compilation easy.

sb2cpp is still incomplete. Some classes are missing or not implemented including but not limited to `Turtle`, `Controls` and `Timer`. Users of Small Basic are encouraged to try sb2cpp with their own programs and report any issues and inconsistencies they find. However, distributing software transpiled with sb2cpp is not recommended at this stage.

## Differences from official Small Basic

- Class and method names are case sensitive. Not using the correct casing will cause compiler errors.
- The program will only be interactive during `Program.Delay()` and while no Small Basic code is running. Programs using graphics windows will need to be adjusted accordingly by either adding delay statements into infinite loops or by using timers.
- Graphics windows will not be displayed until `GraphicsWindow.Show()` is explicitly called.
- Many text window methods are not implemented.
- Extensions for official Small Basic cannot be used with sb2cpp.

## Compatibility

All platforms supported by SDL2 should be supported. Experimental macOS CoreGraphics support is also provided but it is slower compared to SDL2.