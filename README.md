# sb2cpp

A tool for transpiling [Microsoft Small Basic](https://smallbasic-publicwebsite.azurewebsites.net/) into C++. A standard library implementation and convenience scripts are also included to make compilation easy.

## Differences from official Small Basic

- Class and method names are case sensitive. Not using the correct casing will cause compiler errors.
- The program will only be interactive during `Program.Delay()` and while no Small Basic code is running. Programs using graphics windows will need to be adjusted accordingly by either adding delay statements into infinite loops or by using timers.
- Key down/up events are not repeated. Key press logic should be handled by the program.
- Graphics windows will not be displayed until `GraphicsWindow.Show()` is explicitly called.
- Many text window methods are not implemented.
- Extensions for official Small Basic cannot be used with sb2cpp.