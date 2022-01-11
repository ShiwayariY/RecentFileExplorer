Opens a list view of all files in the current directory *and its subdirectories, recursively*, sorted by modification date, newest first.

**NOTE**: Opening the view ***may be slow*** in directories close to the volume root, due to *recursively* listing *all* files

# Usage
- simple keyboard navigation with arrow keys etc.
- selection of arbitrary items with keyboard / mouse
- press Enter to open selected items with their default program
- drag and drop files out of the view into other applications, Windows explorer, ..

# Install / Compile

Requires an installed version of [Qt5](https://www.qt.io/) to compile. Compile with [`init_cpp_project`](https://github.com/ShiwayariY/init_cpp_project):
- Put `init_cpp_project` root on the `PATH`
- run `mk build` then `mk`
