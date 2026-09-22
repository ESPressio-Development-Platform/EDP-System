#!/usr/bin/env python3

"""Build and execute EDP-System host tests, including expected compile failures."""

from __future__ import annotations

import os
from pathlib import Path
import subprocess
import sys
import tempfile


ROOT = Path(__file__).resolve().parents[1]
INCLUDE = ROOT / "src"
CXX = os.environ.get("CXX", "c++")

POSITIVE_TESTS = (
    ROOT / "tests" / "composition" / "CompositionTests.cpp",
    ROOT / "tests" / "composition" / "FlagPropertyTests.cpp",
    ROOT / "tests" / "composition" / "ProviderReferencesTests.cpp",
    ROOT / "tests" / "flags" / "FlagSetTests.cpp",
    ROOT / "tests" / "identity" / "IdentityTests.cpp",
)

COMPILE_FAIL_TESTS = (
    ROOT / "tests" / "compile_fail" / "flag_property_wrong_domain.cpp",
    ROOT / "tests" / "compile_fail" / "flag_property_out_of_range.cpp",
    ROOT / "tests" / "compile_fail" / "flag_constraint_empty.cpp",
    ROOT / "tests" / "compile_fail" / "provider_references_wrong_binding.cpp",
    ROOT / "tests" / "compile_fail" / "provider_references_incompatible_callable.cpp",
)

COMMON_ARGUMENTS = (
    "-std=c++20",
    "-Wall",
    "-Wextra",
    "-Werror",
    "-pedantic",
    f"-I{INCLUDE}",
)


def run(command: list[str], expect_success: bool) -> bool:
    """Execute one compiler/test command and validate the expected result."""
    result = subprocess.run(
        command,
        cwd=ROOT,
        check=False,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )

    succeeded = result.returncode == 0

    if succeeded == expect_success:
        return True

    sys.stderr.write(result.stdout)
    sys.stderr.write(result.stderr)
    return False


def main() -> int:
    """Compile positive tests, execute them, and confirm negative tests fail compilation."""
    with tempfile.TemporaryDirectory(prefix="edp-system-tests-") as temporary_directory:
        output_directory = Path(temporary_directory)

        for source in POSITIVE_TESTS:
            executable = output_directory / source.stem

            if not run(
                [
                    CXX,
                    *COMMON_ARGUMENTS,
                    str(source),
                    "-o",
                    str(executable),
                ],
                True,
            ):
                print(f"FAIL: positive test did not compile: {source.relative_to(ROOT)}")
                return 1

            if not run(
                [str(executable)],
                True,
            ):
                print(f"FAIL: positive test execution failed: {source.relative_to(ROOT)}")
                return 1

            print(f"PASS: {source.relative_to(ROOT)}")

        for source in COMPILE_FAIL_TESTS:
            object_file = output_directory / f"{source.stem}.o"

            if not run(
                [
                    CXX,
                    *COMMON_ARGUMENTS,
                    "-c",
                    str(source),
                    "-o",
                    str(object_file),
                ],
                False,
            ):
                print(f"FAIL: compile-fail test unexpectedly compiled: {source.relative_to(ROOT)}")
                return 1

            print(f"PASS (expected compile failure): {source.relative_to(ROOT)}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
