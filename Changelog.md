# Changelog
## v1.0.0.0 - Initial Release
Optrone is now production ready! (*hopefully*)

## v1.0.0.1 - Make CMake's files more reusable and more
Few CMake files I use in cmake/ folder are mostly reused throughout my project, so rename them to have generic name and configure them with proper name in CMakeLists.txt itself.
Also fix the uninstall target.
Also added [dependency helpers](cmake/depman.cmake). And removed submodules.
