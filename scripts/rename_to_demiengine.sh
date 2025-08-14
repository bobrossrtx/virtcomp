#!/bin/bash
# DemiEngine Rename Script
# Systematically rename VirtComp to DemiEngine across the entire codebase

set -e  # Exit on error

echo "🚀 Starting VirtComp → DemiEngine rename process..."

# Backup current state
echo "📦 Creating backup..."
git add -A
git commit -m "Pre-rename backup: VirtComp codebase before DemiEngine transition" || true
git tag "pre-demiengine-rename" || true

# Phase 1: File and Directory Renames
echo "📁 Renaming files and directories..."

# Rename source files
if [ -f "src/assembler/virtcomp_assembler.hpp" ]; then
    git mv src/assembler/virtcomp_assembler.hpp src/assembler/demi_assembler.hpp
fi
if [ -f "src/assembler/virtcomp_assembler.cpp" ]; then
    git mv src/assembler/virtcomp_assembler.cpp src/assembler/demi_assembler.cpp
fi

# Rename vhardware to engine
if [ -d "src/vhardware" ]; then
    git mv src/vhardware src/engine
fi

# Phase 2: Content Replacements
echo "🔧 Updating file contents..."

# Update include statements
find . -name "*.cpp" -o -name "*.hpp" | xargs sed -i 's|assembler/virtcomp_assembler|assembler/demi_assembler|g'
find . -name "*.cpp" -o -name "*.hpp" | xargs sed -i 's|vhardware/|engine/|g'

# Update namespaces
find . -name "*.cpp" -o -name "*.hpp" | xargs sed -i 's|VirtComp_Registers|DemiEngine_Registers|g'
find . -name "*.cpp" -o -name "*.hpp" | xargs sed -i 's|namespace VirtComp_Registers|namespace DemiEngine_Registers|g'

# Update class names
find . -name "*.cpp" -o -name "*.hpp" | xargs sed -i 's|class VirtComp|class DemiEngine|g'

# Update binary references
find . -name "*.cpp" -o -name "*.hpp" -o -name "*.md" | xargs sed -i 's|bin/virtcomp|bin/demi-engine|g'
find . -name "*.cpp" -o -name "*.hpp" -o -name "*.md" | xargs sed -i 's|./bin/virtcomp|./bin/demi-engine|g'

# Update GUI titles and user-facing text
find . -name "*.cpp" | xargs sed -i 's|VirtComp Debugger|DemiEngine Debugger|g'
find . -name "*.cpp" | xargs sed -i 's|VirtComp Virtual Machine|DemiEngine Runtime|g'
find . -name "*.cpp" | xargs sed -i 's|Running VirtComp|Running DemiEngine|g'
find . -name "*.cpp" | xargs sed -i 's|VirtComp Unit Tests|DemiEngine Unit Tests|g'
find . -name "*.cpp" | xargs sed -i 's|VirtComp Integration Tests|DemiEngine Integration Tests|g'

# Update help text
find . -name "*.cpp" | xargs sed -i 's|virtcomp Usage: virtcomp|demi-engine Usage: demi-engine|g'

# Phase 3: Makefile Updates
echo "🔨 Updating build system..."

# Update Makefile target
sed -i 's|TARGET := $(BIN_DIR)/virtcomp|TARGET := $(BIN_DIR)/demi-engine|g' Makefile

# Phase 4: Documentation Updates
echo "📚 Updating documentation..."

# Update main README
sed -i 's|# VirtComp|# DemiEngine|g' README.md
sed -i 's|VirtComp provides|DemiEngine provides|g' README.md
sed -i 's|git clone.*virtcomp|git clone https://github.com/bobrossrtx/demi.git|g' README.md || true

# Update all documentation
find docs/ -name "*.md" | xargs sed -i 's|VirtComp|DemiEngine|g'
find docs/ -name "*.md" | xargs sed -i 's|virtcomp|demi-engine|g'

# Update roadmap
if [ -f "roadmap.md" ]; then
    sed -i 's|VirtComp|DemiEngine|g' roadmap.md
    sed -i 's|virtcomp|demi-engine|g' roadmap.md
fi

# Phase 5: Test and verify
echo "🧪 Testing build..."
make clean
if make -j4; then
    echo "✅ Build successful after rename!"
else
    echo "❌ Build failed - manual fixes needed"
    exit 1
fi

# Phase 6: Run tests to ensure nothing broke
echo "🔍 Running tests..."
if ./bin/demi-engine --test; then
    echo "✅ Tests passed after rename!"
else
    echo "⚠️  Some tests failed - may need manual fixes"
fi

echo "🎉 VirtComp → DemiEngine rename complete!"
echo ""
echo "📋 Summary of changes:"
echo "  • Binary: bin/virtcomp → bin/demi-engine"
echo "  • Namespace: VirtComp_Registers → DemiEngine_Registers"
echo "  • Class: VirtComp → DemiEngine"
echo "  • Directory: src/vhardware → src/engine"
echo "  • Files: virtcomp_assembler → demi_assembler"
echo ""
echo "🔧 Next steps:"
echo "  1. Test all functionality thoroughly"
echo "  2. Update any remaining references manually"
echo "  3. Consider renaming GitHub repository"
echo "  4. Update build badges and external references"
