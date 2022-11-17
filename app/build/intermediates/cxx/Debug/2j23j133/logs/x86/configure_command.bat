@echo off
"C:\\Program Files\\CMake\\bin\\cmake.exe" ^
  "-HD:\\Studia\\PracaInzynierska\\board-game-ar\\app\\src\\main\\cpp" ^
  "-DCMAKE_SYSTEM_NAME=Android" ^
  "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON" ^
  "-DCMAKE_SYSTEM_VERSION=24" ^
  "-DANDROID_PLATFORM=android-24" ^
  "-DANDROID_ABI=x86" ^
  "-DCMAKE_ANDROID_ARCH_ABI=x86" ^
  "-DANDROID_NDK=C:\\Users\\aniab\\AppData\\Local\\Android\\Sdk\\ndk\\25.1.8937393" ^
  "-DCMAKE_ANDROID_NDK=C:\\Users\\aniab\\AppData\\Local\\Android\\Sdk\\ndk\\25.1.8937393" ^
  "-DCMAKE_TOOLCHAIN_FILE=C:\\Users\\aniab\\AppData\\Local\\Android\\Sdk\\ndk\\25.1.8937393\\build\\cmake\\android.toolchain.cmake" ^
  "-DCMAKE_MAKE_PROGRAM=C:\\Users\\aniab\\AppData\\Local\\Android\\Sdk\\cmake\\3.18.1\\bin\\ninja.exe" ^
  "-DCMAKE_CXX_FLAGS=-frtti -fexceptions" ^
  "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=D:\\Studia\\PracaInzynierska\\board-game-ar\\app\\build\\intermediates\\cxx\\Debug\\2j23j133\\obj\\x86" ^
  "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=D:\\Studia\\PracaInzynierska\\board-game-ar\\app\\build\\intermediates\\cxx\\Debug\\2j23j133\\obj\\x86" ^
  "-DCMAKE_BUILD_TYPE=Debug" ^
  "-BD:\\Studia\\PracaInzynierska\\board-game-ar\\app\\.cxx\\Debug\\2j23j133\\x86" ^
  -GNinja ^
  "-DANDROID_ARM_NEON=TRUE" ^
  "-DOpenCV_DIR=D:\\Studia\\PracaInzynierska\\board-game-ar/external-deps/OpenCV-android-sdk/sdk/native/jni"
