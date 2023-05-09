#ifndef SMALLBASIC_PLATFORM_CONFIG_HPP

#if defined(__APPLE__) && defined(__OBJC__)
#define SMALLBASIC_APPLE
#else
#define SMALLBASIC_SDL
#endif

#endif