# Compiler Definitions and Conditional Compilation

No repository-defined compiler definitions were found in the audited PlatformIO/CMake/SDK configuration. The public library architecture is configured through C++ types/templates and provider selection rather than repository-specific preprocessor switches.

Platform/framework macros supplied externally by a compiler, SDK or framework are not treated as EDP configuration knobs unless the repository explicitly defines them.

> Audited against `ea9af1ba833bcc2710fade18ea04bcb85227da9b` on `main` across 10 build-configuration file(s).
