# sb2cpp

A tool for transpiling [Microsoft Small Basic](https://smallbasic-publicwebsite.azurewebsites.net/) into C++. A standard library implementation and convenience scripts are also provided to make compilation easy.

sb2cpp is still incomplete. Some classes are missing or not implemented including but not limited to `Turtle`, `Controls` and `Timer`. Users of Small Basic are encouraged to try sb2cpp with their own programs and report any issues and inconsistencies they find. However, distributing software transpiled with sb2cpp is not recommended at this stage.

## Compatibility

All platforms supported by SDL2 should be supported. Experimental macOS CoreGraphics support is also provided but it is slower compared to SDL2.