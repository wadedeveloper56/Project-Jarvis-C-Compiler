---
applyTo: "jwcc2/**"
---
# C99-to-MASM Compiler

## Clean-Room & Solution Architecture Rules
- **Build From Scratch:** You are implementing **jwcc2** completely from scratch as a greenfield C++20 codebase. **Ignore and do not read, reference, include, or reuse any legacy or existing source files** elsewhere in the workspace outside of `jwcc2/`, `jwcc2lib/`, and `jwcc2test/`.
- **Strict 3-Project Separation:**
  1. **`jwcc2/jwcc2lib` (Static Library):** Contains 100% of the compiler functionality
  2. **`jwcc2/jwcc2` (Console Executable):** Thin entry-point wrapper only. Contains **only `main(int argc, char* argv[])`**, which forwards command-line arguments into `jwcc2lib` and returns its exit code. Zero compiler logic is permitted in `jwcc2`.
  3. **`jwcc2/jwcc2test` (Test Executable):** Unit and integration test suite linking solely against `jwcc2lib`.

- **Project rules:**
  1. **Language:** Strict ISO C++20 (use `std::variant`, `std::span`, `std::string_view`, `std::expected` if available, concepts, and smart pointers; avoid raw new/delete).
  2. **Input Standard**