# og3 Development Mandates

## Coding Standards
- **Naming**:
  - **Methods**: `camelCase`.
  - **JSON Keys**: `camelCase` (Variables should use camelCase for native Svelte integration).
  - **Local Variables**: `snake_case`.
  - **Private Members**: `m_` prefix (e.g., `m_variable_name`).
  - **Constants**: `kPascalCase` (e.g., `kDefaultPort`).
- **Documentation**: All public/protected members in headers MUST use Doxygen-style comments (`/** ... */`, `@brief`, `@param`).
- **Namespaces**: Generally use the `og3::` namespace. Consider sub-namespaces (e.g., `og3::pkt`) only when adding large amounts of related code to maintain organization.

## Architecture & Memory
- **Module System**: Logic must be encapsulated in `Module` classes registered with the `ModuleSystem`. Use the `require<T>(name, &ptr)` pattern in constructors to manage lifecycle ordering and automate module linking.
- **Memory Management**:
  - **ESP8266 Sensitivity**: Minimize stack usage. Avoid local arrays > 64 bytes; move large structures to the heap during `setup()` or use static pools.
  - **Dependency Storage**: `ModuleSystem` uses transient contiguous storage for requirements during boot; all memory used for linking is reclaimed before `loop()`.
  - **Heap Usage**: Prefer not to perform heap allocations in `loop()`. Use static pools or pre-allocated buffers for recurring tasks (e.g., `TaskQueue` pool).
- **Task Scheduling**: Use the `Tasks` module for any asynchronous or delayed logic; avoid blocking `delay()` calls.

## Networking (v0.5.0+)
- **Portability**: Always use the `og3::Net*` abstraction layer (`NetRequest`, `NetResponse`, `NetHandler`).
- **Handler Signature**: Standard web handlers must use `(NetRequest* request, NetResponse* response)`.
- **Status Reporting**: Use the `NET_REPLY(request, status)` macro to return from web handlers.

## Maintenance & Quality
- **CHANGELOG**: Every major update MUST be documented in `CHANGELOG.md` following the [Keep a Changelog](https://keepachangelog.com/) format.
- **CI/CD**: Maintain automated CI in `.github/workflows/platformio-ci.yaml`. All changes must pass native tests and the `util/ci.sh` build script.
- **Static Analysis**: Use `pio check -e native` to identify performance or portability issues, especially when targeting the ESP8266.
- **Extensions**: New sensor support should follow the `og3x-<sensor>` naming convention and remain decoupled from the core library.
