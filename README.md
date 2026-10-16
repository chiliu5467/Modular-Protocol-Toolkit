
# Modular Protocol Toolkit

A modular **C++20 library** for encoding, decoding, and validating fixed-size binary protocol headers.

This project demonstrates modern C++ software architecture, including CMake build systems, public and private API separation, explicit error handling, dependency injection, and automated testing.

## Features

- **Binary Header Serialization:** Encode and decode a fixed 10-byte binary header.
- **Portable Byte Order:** Big-endian encoding independent of the host machine's endianness.
- **Defensive Parsing:** Validate header size, magic bytes, and protocol version.
- **Explicit Error Handling:** Use `std::optional` and `std::variant` to represent parsing results.
- **Dependency Injection:** Support customizable header validation through an abstract interface.
- **Modular Architecture:** Separate public APIs, private implementations, applications, and tests.
- **Automated Testing:** Use CTest to validate functionality in Debug and Release builds.

## Tech Stack

- **Language:** C++20
- **Build System:** CMake 3.20+
- **Compiler:** MSVC / Visual Studio 2022
- **Testing:** CTest
- **Core Features:** `std::span`, `std::byte`, `std::optional`, `std::variant`, runtime polymorphism

## Project Structure

```text
Modular-Protocol-Toolkit/
├── CMakeLists.txt
├── CMakePresets.json
├── include/
│   └── protocol/
│       ├── ProtocolHeader.h
│       ├── HeaderPolicy.h
│       └── BasicHeaderPolicy.h
├── src/
│   ├── ProtocolHeader.cpp
│   ├── ByteOrder.h
│   ├── ByteOrder.cpp
│   └── BasicHeaderPolicy.cpp
├── app/
│   └── main.cpp
└── tests/
    ├── test_protocol.cpp
    ├── test_policy.cpp
    └── header_self_contained.cpp
```

### Architecture

The project is organized around three components:

1. **Protocol Library:** Provides reusable encoding, decoding, and validation APIs.
2. **CLI Application:** Demonstrates how an application consumes the protocol library.
3. **Test Executables:** Validate protocol behavior, error handling, and dependency injection.

The public interfaces are located in `include/protocol/`, while implementation details such as byte-order helpers remain in `src/`.

## Binary Header Format

The protocol uses a fixed **10-byte header**.

| Offset | Size | Field | Description |
|---|---|---|---|
| 0–1 | 2 bytes | Magic | `4D 4C` (`ML`) |
| 2 | 1 byte | Version | `01` |
| 3 | 1 byte | Type | Unsigned 8-bit integer |
| 4–7 | 4 bytes | Sequence | Big-endian `uint32_t` |
| 8–9 | 2 bytes | Payload Length | Big-endian `uint16_t` |

### Example

Given the following header:

```cpp
protocol::Header header{
    2,
    0x12345678,
    3
};
```

The encoded binary representation is:

```text
4D 4C 01 02 12 34 56 78 00 03
```

Multi-byte values are encoded using big-endian order to ensure consistent binary representation across different platforms.

**Note:** This toolkit currently serializes and validates the header only. It does not serialize or validate the actual payload body. The payload length is stored as metadata.

## Public API

The library provides several decoding interfaces.

### EncodeHeader()

Converts a `Header` object into a 10-byte binary array.

```cpp
std::array<std::byte, HEADER_SIZE>
EncodeHeader(const Header& header);
```

### DecodeHeader()

Decodes a binary header and returns `std::nullopt` when parsing fails.

```cpp
std::optional<Header>
DecodeHeader(std::span<const std::byte> bytes);
```

### DecodeHeaderDetailed()

Returns either a decoded `Header` or a specific `DecodeError`.

```cpp
using HeaderDecodeResult =
    std::variant<Header, DecodeError>;

HeaderDecodeResult DecodeHeaderDetailed(
    std::span<const std::byte> bytes);
```

### DecodeHeaderWithPolicy()

Supports additional validation through a caller-provided policy.

```cpp
HeaderDecodeResult DecodeHeaderWithPolicy(
    std::span<const std::byte> bytes,
    const HeaderPolicy& policy);
```

This allows applications to customize header validation without modifying the core decoder.

## Error Handling

The library defines the following error types:

```cpp
enum class DecodeError
{
    InvalidSize,
    InvalidMagic,
    UnsupportedVersion,
    PolicyRejected
};
```

| Error | Description |
|---|---|
| `InvalidSize` | Header size is not exactly 10 bytes |
| `InvalidMagic` | Magic bytes do not match the protocol specification |
| `UnsupportedVersion` | Protocol version is unsupported |
| `PolicyRejected` | Header was rejected by the injected validation policy |

The decoder validates input in the following order:

```text
Input Bytes
    |
    v
Validate Header Size
    |
    v
Validate Magic Bytes
    |
    v
Validate Version
    |
    v
Decode Header Fields
    |
    v
Return Header or DecodeError
```

The policy-based decoder performs additional validation only after the binary header has been successfully decoded.

## Dependency Injection

One of the primary goals of this project is to practice **Dependency Injection (DI)** and reduce tight coupling between components.

### HeaderPolicy Interface

```cpp
class HeaderPolicy
{
public:
    virtual ~HeaderPolicy() = default;

    virtual bool Accept(const Header& header) const = 0;
};
```

The decoder depends on the `HeaderPolicy` abstraction rather than a specific concrete implementation.

### BasicHeaderPolicy

The included `BasicHeaderPolicy` accepts headers when:

- `type` is 1 or 2.
- `payloadLength` is less than or equal to 1024 bytes.

```cpp
bool BasicHeaderPolicy::Accept(const Header& header) const
{
    if (header.type == 1 || header.type == 2)
    {
        if (header.payloadLength <= 1024)
        {
            return true;
        }
    }

    return false;
}
```

### Example Usage

```cpp
#include "protocol/BasicHeaderPolicy.h"
#include "protocol/ProtocolHeader.h"

#include <variant>

int main()
{
    protocol::Header header{
        2,
        0x12345678,
        100
    };

    auto bytes = protocol::EncodeHeader(header);

    protocol::BasicHeaderPolicy policy;

    auto result = protocol::DecodeHeaderWithPolicy(
        bytes,
        policy
    );

    if (const auto* decoded =
            std::get_if<protocol::Header>(&result))
    {
        return decoded->sequence == header.sequence ? 0 : 1;
    }

    return 1;
}
```

Additional validation policies can be implemented without modifying `DecodeHeaderWithPolicy()`.

This demonstrates the benefits of dependency injection:

- Reduced coupling
- Improved extensibility
- Easier unit testing
- Separation of responsibilities
- Better maintainability

## Testing

The project uses **CTest** with two test executables.

### Protocol Tests

`tests/test_protocol.cpp`

Test coverage includes:

- Exact binary encoding
- Legacy header decoding
- Detailed header decoding
- Invalid magic bytes
- Unsupported protocol versions
- Truncated headers
- Oversized headers
- Empty input
- Validation priority
- Error message verification

### Policy Tests

`tests/test_policy.cpp`

Test coverage includes:

- `AcceptAllPolicy`
- `RejectAllPolicy`
- `BasicHeaderPolicy`
- `CountingPolicy`
- Type validation
- Payload length boundary conditions
- Verification that policies are not called when binary decoding fails

Test Doubles are used to isolate and verify the decoder's behavior.

### Release-Safe Testing

Tests use explicit validation checks and return nonzero exit codes when failures occur.

This ensures test conditions remain active in both Debug and Release configurations, unlike assertions that may be disabled when `NDEBUG` is defined.

## Build Instructions

### Requirements

- C++20-compatible compiler
- CMake 3.20 or later
- Visual Studio 2022 with MSVC
- Ninja build system

The included CMake presets are configured for Windows with MSVC and Ninja.

Open a **Visual Studio Developer PowerShell** in the project root directory.

### Debug Build

```powershell
cmake --preset x64-debug
cmake --build out/build/x64-debug --config Debug
```

Run tests:

```powershell
ctest --test-dir out/build/x64-debug -C Debug --output-on-failure
```

### Release Build

```powershell
cmake --preset x64-release
cmake --build out/build/x64-release --config Release
```

Run tests:

```powershell
ctest --test-dir out/build/x64-release -C Release --output-on-failure
```

### Run the CLI

For the Debug build:

```powershell
.\out\build\x64-debug\protocol_cli.exe
```

The CLI demonstrates header encoding and decoding and prints the decoded fields.

## Design Decisions

### 1. Explicit Binary Encoding

Multi-byte integers are encoded field by field instead of directly copying a C++ struct into a byte buffer.

This avoids relying on compiler-specific struct padding and host endianness.

### 2. Public and Private API Separation

Public headers expose reusable interfaces, while internal byte-order helpers remain private implementation details.

This reduces unnecessary dependencies between components.

### 3. Explicit Error Results

`std::optional` provides a simple interface for operations that may fail.

`std::variant<Header, DecodeError>` provides more detailed error information when callers need to understand the reason for failure.

### 4. Dependency Injection

The policy-based decoder accepts an externally supplied validation object.

This allows applications and tests to provide different validation rules without modifying the binary decoding logic.

### 5. Reusable Core Decoder

Binary parsing is implemented once in `DecodeHeaderDetailed()`.

Both the legacy decoder and the policy-based decoder reuse this implementation to avoid duplicated validation logic.

## Current Limitations

- Supports only protocol version `01`.
- Processes fixed-size 10-byte headers.
- Does not serialize or validate payload bodies.
- Does not implement checksums or streaming record parsing.
- The supplied CMake presets are Windows-specific.

## What I Learned

This project was developed as part of a 13-week Modern C++ learning roadmap focused on software engineering and systems programming.

Key learning outcomes include:

- Building modular C++ projects with CMake.
- Designing reusable static libraries.
- Separating public interfaces from private implementation details.
- Managing build dependencies through CMake targets.
- Implementing portable binary encoding and decoding.
- Designing explicit error handling with `std::optional` and `std::variant`.
- Applying runtime polymorphism and dependency injection.
- Creating Test Doubles for isolated testing.
- Writing boundary tests and validating malformed input.
- Running automated tests in both Debug and Release configurations.

The project demonstrates how to move from implementing individual C++ functions toward designing maintainable, testable, and reusable software components.
