# C++ Graphical IDE

A lightweight graphical text editor and C++ development environment built in C++ using **SDL2** and **SDL_ttf**. The project provides a graphical interface for writing, editing, compiling, and running C++ programs.

## Overview

This project is a custom graphical C++ editor designed to provide core features of a lightweight Integrated Development Environment (IDE).

The application combines a graphical text editor with basic development tools, including syntax highlighting, code editing features, project management, and C++ compilation and execution.

## Features

- Graphical text editor built with SDL2
- SDL_ttf-based text rendering
- C++ syntax highlighting
- Light and dark editor themes
- Code editing and navigation
- Undo and redo functionality
- Clipboard support
- Automatic pairing of characters such as brackets and quotation marks
- Code suggestions and basic autocomplete
- Line navigation and scrolling
- Project creation and project management
- Save and load projects
- C++ compilation using `g++`
- Program execution from within the IDE
- File and project management menus
- Debug and compile workflow
- Custom graphical user interface

## Syntax Highlighting

The editor provides syntax highlighting to improve code readability.

Different programming language elements are displayed using different styles, including:

- Keywords
- Data types
- Functions
- Variables
- Numbers
- Strings and characters
- Comments
- Preprocessor directives

The editor supports both light and dark visual themes with dedicated syntax-highlighting configurations.

## Editor Features

The text editor includes several features commonly found in modern development environments:

### Editing

- Text insertion and deletion
- Cursor movement
- Line-based navigation
- Scrolling
- Clipboard operations
- Undo and redo

### Code Assistance

- Basic code suggestions
- Automatic pairing of brackets and quotation marks
- Syntax-aware text rendering
- Recognition of common C++ constructs

### Project Management

The IDE provides a basic project-oriented workflow that allows users to:

1. Create a new project.
2. Save a project.
3. Open an existing project.
4. Switch between saved projects.
5. Compile the current project.
6. Run the generated executable.

## Compilation and Execution

The IDE integrates the C++ compiler into the graphical workflow.

A C++ source file can be compiled using `g++`, after which the generated executable can be launched directly from the application.

This provides a simple development cycle:

```text
Write Code
    ↓
Compile
    ↓
Debug / Check
    ↓
Run
```

## Graphical Interface

The application provides a graphical interface containing components such as:

- Menu bar
- Text editing area
- Project list
- Save and project controls
- Compile and debug controls
- Run functionality
- Light/dark mode controls

The interface is implemented using SDL2, while SDL_ttf is used for rendering text.

## Technologies

- **C++**
- **SDL2**
- **SDL_ttf**
- **CMake**
- **MinGW / g++**
- Object-Oriented Programming
- GUI Programming
- Compiler Integration

## Project Structure

```text
cpp-graphical-ide/
│
├── CMakeLists.txt
├── cpp-graphical-ide.cpp
└── README.md
```

## Skills Demonstrated

This project demonstrates practical experience with:

- C++ programming
- Object-oriented programming
- GUI development
- SDL2-based application development
- Text rendering with SDL_ttf
- Syntax highlighting
- Editor design and implementation
- File and project management
- Compiler integration
- CMake-based project configuration
- Debugging and software development workflows

## Project Context

This project was developed collaboratively as an undergraduate course project.

The implementation involved designing and developing a graphical C++ editing environment with integrated code editing, project management, compilation, and execution features.

## Contributors

- Avin Salmanpour
- Project collaborator

## License

This project was developed for educational purposes.
