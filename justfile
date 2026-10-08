build:
    {{ if os() == "windows" { "cmake -S . -B build -DPUM_VENDORED_SDL=ON" } else if os() == "macos" { "cmake -S . -B build -DCMAKE_BUILD_TYPE=Release" } else { "cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DPUM_VENDORED_SDL=ON" } }}
    cmake --build build{{ if os() == "windows" { " --config Release" } else { "" } }}

run:
    {{ if os() == "windows" { '.\build\Release\pum.exe' } else { './build/pum' } }}
