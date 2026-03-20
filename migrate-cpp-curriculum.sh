#!/usr/bin/env bash
# ============================================================
# migrate-cpp-curriculum.sh
#
# Moves all C++ learning material from wiki/c++ into
# resources/curriculum/c++ with a clean 00–26 index order.
#
# Run from the ROOT of your repository:
#   bash migrate-cpp-curriculum.sh
#
# Dry-run first (prints what would happen, moves nothing):
#   DRY_RUN=1 bash migrate-cpp-curriculum.sh
# ============================================================

set -euo pipefail

SRC="wiki/c++"
DST="resources/curriculum/c++"
DRY="${DRY_RUN:-0}"

move() {
  local from="$1"
  local to="$2"
  if [[ "$DRY" == "1" ]]; then
    echo "[DRY RUN]  $from  →  $to"
  else
    mkdir -p "$(dirname "$to")"
    mv "$from" "$to"
    echo "Moved: $from  →  $to"
  fi
}

# ----------------------------------------------------------
# PHASE 1: Files that already carry an index (00–03)
# Re-index them into the new unified sequence.
# ----------------------------------------------------------
#   OLD index  →  NEW index  (reason for change)
#   00            00         no change
#   01            01         no change
#   02 Variables  03         "Understanding Your First Program" inserts at 02
#   03 DataTypes  04         pushed down by one

move "$SRC/00 - Introduction to C++ Programming Language.md" \
     "$DST/00 - Introduction to C++ Programming Language.md"

move "$SRC/01 - Setting Up C++ Development Environment.md" \
     "$DST/01 - Setting Up C++ Development Environment.md"

# "Understanding Your First Program" belongs right after setup,
# before the language proper starts — so it becomes 02.
move "$SRC/Understanding Your First C++ Program.md" \
     "$DST/02 - Understanding Your First C++ Program.md"

move "$SRC/02 - Introduction to Variables in C++.md" \
     "$DST/03 - Introduction to Variables in C++.md"

move "$SRC/03 - Introduction to Data Types in C++.md" \
     "$DST/04 - Introduction to Data Types in C++.md"

# ----------------------------------------------------------
# PHASE 2: Un-indexed files — placed in curriculum order
# ----------------------------------------------------------

move "$SRC/Operators and Expressions in C++.md" \
     "$DST/05 - Operators and Expressions in C++.md"

move "$SRC/Input Methods in C++.md" \
     "$DST/06 - Input Methods in C++.md"

move "$SRC/Conditional Statements (Decision Making) in C++.md" \
     "$DST/07 - Conditional Statements (Decision Making) in C++.md"

# Note: original filename had a trailing "1" — cleaned up here.
move "$SRC/Loops (Repetition) in C++ 1.md" \
     "$DST/08 - Loops (Repetition) in C++.md"

move "$SRC/Functions in C++.md" \
     "$DST/09 - Functions in C++.md"

move "$SRC/Arrays in C++.md" \
     "$DST/10 - Arrays in C++.md"

move "$SRC/Strings in C++.md" \
     "$DST/11 - Strings in C++.md"

move "$SRC/Pointers (Basics) in C++.md" \
     "$DST/12 - Pointers (Basics) in C++.md"

move "$SRC/Dynamic Memory Management in C++.md" \
     "$DST/13 - Dynamic Memory Management in C++.md"

move "$SRC/Structures (struct) in C++.md" \
     "$DST/14 - Structures (struct) in C++.md"

move "$SRC/Classes and Objects (OOP Basics) in C++.md" \
     "$DST/15 - Classes and Objects (OOP Basics) in C++.md"

move "$SRC/Advanced OOP Concepts in C++.md" \
     "$DST/16 - Advanced OOP Concepts in C++.md"

move "$SRC/Templates (Basics) in C++.md" \
     "$DST/17 - Templates (Basics) in C++.md"

move "$SRC/STL (Standard Template Library) - Containers in C++.md" \
     "$DST/18 - STL (Standard Template Library) - Containers in C++.md"

move "$SRC/STL Algorithms in C++.md" \
     "$DST/19 - STL Algorithms in C++.md"

move "$SRC/Exception Handling in C++.md" \
     "$DST/20 - Exception Handling in C++.md"

move "$SRC/File Handling in C++.md" \
     "$DST/21 - File Handling in C++.md"

move "$SRC/Preprocessor Directives in C++.md" \
     "$DST/22 - Preprocessor Directives in C++.md"

move "$SRC/Namespaces in C++.md" \
     "$DST/23 - Namespaces in C++.md"

move "$SRC/Multi-file Projects in C++.md" \
     "$DST/24 - Multi-file Projects in C++.md"

move "$SRC/Debugging Techniques in C++.md" \
     "$DST/25 - Debugging Techniques in C++.md"

move "$SRC/Best Practices and Code Style in C++.md" \
     "$DST/26 - Best Practices and Code Style in C++.md"

# ----------------------------------------------------------
# PHASE 3: Clean up the now-empty source directory
# ----------------------------------------------------------
if [[ "$DRY" != "1" ]]; then
  if [ -z "$(ls -A "$SRC" 2>/dev/null)" ]; then
    rmdir "$SRC"
    echo "Removed empty directory: $SRC"
  else
    echo "WARNING: $SRC is not empty — check for leftover files:"
    ls -la "$SRC"
  fi
fi

echo ""
echo "Done. Next steps:"
echo "  1. Place README.md (the curriculum index) into $DST/"
echo "  2. git add $DST/ README.md"
echo "  3. git commit -m 'refactor: move C++ wiki into curriculum with 00-26 index'"
