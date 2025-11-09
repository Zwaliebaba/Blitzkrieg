These instructions define how GitHub Copilot should assist with this project. The goal is to ensure consistent, high-quality code generation aligned with our conventions, stack, and best practices.

Context
Project Type: RTS Game
Project Name: Space Combat Outpost
Language: C++
Framework / Libraries: STL / Visual Studio
Architecture: Modular / RAII / OOP

General Guidelines
Code Style: The project uses an .editorconfig file to enforce coding standards. Follow the rules defined in .editorconfig for indentation, line endings, and other formatting. The code requires C++11/C++14 features.

Error Handling: Use C++ exceptions for error handling and uses RAII smart pointers to ensure resources are properly managed. For some functions that return HRESULT error codes, they are marked noexcept, use std::nothrow for memory allocation, and should not throw exceptions.

Security: This project uses secure coding practices from the Microsoft Secure Coding Guidelines. Functions that read input from image file, geometry files, and audio files are subject to OneFuzz fuzz testing to ensure they are secure against malformed files.

No speculation
When creating documentation:

Document Only What Exists
Only document features, patterns, and decisions that are explicitly present in the source code.
Only include configurations and requirements that are clearly specified.
Do not make assumptions about implementation details.

Handle Missing Information
Ask the user questions to gather missing information.
Document gaps in current implementation or specifications.
List open questions that need to be addressed.

Source Material
Always cite the specific source file and line numbers for documented features.
Link directly to relevant source code when possible.
Indicate when information comes from requirements vs. implementation.

Verification Process
Review each documented item against source code whenever related to the task.
Remove any speculative content.
Ensure all documentation is verifiable against the current state of the codebase.

Code Review Instructions
When reviewing code, focus on the following aspects:
Adherence to coding standards defined in .editorconfig 
Make coding recommendations based on the C++ Core Guidelines.
Proper use of RAII and smart pointers.
Correct error handling practices and C++ Exception safety.
Clarity and maintainability of the code.
Adequate comments where necessary.
Compliance with the project's architecture and design patterns.
Check for performance implications, especially in geometry processing algorithms.
Provide brutally honest feedback on code quality, design, and potential improvements as needed.

Documentation Review Instructions

When reviewing documentation, do the following:
Read the code located in this git repository in the main branch.
