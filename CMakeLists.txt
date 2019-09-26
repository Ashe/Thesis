cmake_minimum_required (VERSION 3.7)
project (Project)

message("Using CMake version ${CMAKE_VERSION}")
message("Processing CMakeLists.txt")

# Compiler version
set (CMAKE_CXX_STANDARD 17)

# If on Windows, specify where these packages are installed
if (WIN32)
  set(SFML_STATIC_LIBRARIES TRUE)
  set(SFML_DIR "C:/Program Files (x86)/SFML-2.5.1/lib/cmake/SFML")
endif()

# Find X11
find_package(X11)

# Find multithreading package
find_package(Threads REQUIRED)

# Find OpenGL
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)

# Find SFML
find_package(SFML 2.5.1 REQUIRED COMPONENTS window system graphics audio)

# Create the executable
set(EXECUTABLE_NAME ${PROJECT_NAME})
add_executable (${EXECUTABLE_NAME} src/main.cpp)

# Add sources
target_sources(${EXECUTABLE_NAME} PRIVATE

  # Core
  src/App.h
  src/App.cpp
  src/Resources.h
  src/Resources.cpp
  src/Scene.h
  src/Scene.cpp

  # Controllers
  src/Controller/Common.h
  src/Controller/Random/Random.h
  src/Controller/AStar/AStar.h

  # Scenes
  src/Scenes/Welcome/Welcome.h
  src/Scenes/TicTacToe/TicTacToe.h
  src/Scenes/TicTacToe/TicTacToe.cpp
  src/Scenes/TicTacToe/Common.h
  src/Scenes/TicTacToe/GameState.h
  src/Scenes/TicTacToe/Cost.h

  # Development
  src/Console.h
  src/Console.cpp
  src/imgui/imconfig.h
  src/imgui/imgui.h
  src/imgui/imgui.cpp
  src/imgui/imgui-SFML.h
  src/imgui/imgui-SFML.cpp
  src/imgui/imgui_demo.cpp
  src/imgui/imgui_draw.cpp
  src/imgui/imgui_internal.h
  src/imgui/imgui_widgets.cpp
  src/imgui/imstb_rectpack.h
  src/imgui/imstb_truetype.h
)

# Platform specific building
if (WIN32)
  # Link libraries so that they can be used in the project
  target_link_libraries(${EXECUTABLE_NAME}
    sfml-window
    sfml-system
    sfml-graphics
    sfml-audio
    ${CMAKE_THREAD_LIBS_INIT}
  )
endif()
if(UNIX)

  # Link libraries so that they can be used in the project
  target_link_libraries(${EXECUTABLE_NAME} stdc++fs
    GL
    sfml-window
    sfml-system
    sfml-graphics
    sfml-audio
    ${X11_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
  )
endif()

# Copy config files and assets
file(COPY ${CMAKE_SOURCE_DIR}/Assets DESTINATION ${CMAKE_BINARY_DIR})
