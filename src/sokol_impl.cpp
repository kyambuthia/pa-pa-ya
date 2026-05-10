// ── Sokol implementation ──────────────────────────────────────
// Define SOKOL_IMPL exactly once per translation unit for each
// sokol header used.  SOKOL_GLCORE selects desktop GL 3.3+ core
// profile (via GLX on X11 / WGL on Win32).
//
// SOKOL_NO_ENTRY prevents sokol from defining its own main()
// so we can provide our own in main.cpp.

#define SOKOL_IMPL
#define SOKOL_NO_ENTRY
#define SOKOL_GLCORE

#include <sokol_gfx.h>
#include <sokol_app.h>
#include <sokol_glue.h>
#include <sokol_log.h>
#include <sokol_time.h>
