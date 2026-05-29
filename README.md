# mosaico-cpp-sdk

C++ client SDK for the Mosaico cloud data server (Arrow Flight transport,
metadata/query helpers). Builds a static library target `mosaico_sdk`.

Arrow (+ Arrow Flight) and `nlohmann_json` are **provided by the parent
project** — the SDK does not `find_package()` them, so consumers must create
the `arrow::arrow`/`Arrow::arrow_static` (+ flight) and
`nlohmann_json::nlohmann_json` targets before adding this SDK.

## Consuming via CPM

```cmake
include(cmake/CPM.cmake)
CPMAddPackage(
  NAME mosaico_sdk
  URL https://github.com/PlotJuggler/mosaico-cpp-sdk/archive/refs/heads/development.zip
)
target_link_libraries(your_target PRIVATE mosaico_sdk)
```
