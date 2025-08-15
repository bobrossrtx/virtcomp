# DemiEngine Rename Manual Review Checklist

## 🔍 **Post-Rename Verification Steps**

After running the automated rename script, manually verify these areas:

### **1. Critical Build Components**
- [ ] `Makefile` - Verify TARGET and all build rules work
- [ ] `src/main.cpp` - Check main function and class instantiation
- [ ] Include paths in all `.cpp/.hpp` files
- [ ] Namespace usage throughout codebase

### **2. Source Code Integrity**
- [ ] `src/assembler/demi_assembler.hpp/cpp` - Verify headers match implementations
- [ ] `src/engine/` directory (renamed from vhardware)
- [ ] All `#include` statements point to correct files
- [ ] Class declarations match definitions

### **3. User-Facing Elements**
- [ ] Command-line help text (`--help` output)
- [ ] Debug GUI window titles and labels
- [ ] Error messages and logging output
- [ ] Test output messages

### **4. Documentation Consistency**
- [ ] `README.md` - All references updated
- [ ] `docs/` directory - All markdown files
- [ ] `roadmap.md` - Project descriptions
- [ ] Code comments and docstrings

### **5. Build Artifacts**
- [ ] Binary executable name: `bin/demi-engine`
- [ ] Object file paths in `build/` directory
- [ ] Clean and rebuild works without errors
- [ ] All tests still pass

### **6. External Dependencies**
- [ ] GitHub repository references
- [ ] Build badges and CI/CD scripts
- [ ] Example files and tutorials
- [ ] License and attribution files

## 🚨 **Common Issues to Watch For**

1. **Circular Dependencies**: Renamed headers causing include loops
2. **Case Sensitivity**: Linux filesystem case-sensitive file references
3. **Git History**: Ensure `git mv` preserves file history
4. **Hardcoded Paths**: Any absolute paths that include "demi-engine"
5. **External Tools**: Scripts or configs that reference old names

## 🔧 **Manual Fixes Likely Needed**

These areas often require manual attention after automated rename:

### **Complex String Replacements**
```bash
# Find remaining DemiEngine references that need context-aware fixes
grep -r "DemiEngine" . --exclude-dir=.git | grep -v "# DemiEngine"

# Check for any demi-engine in lowercase that might be missed
grep -r "demi-engine" . --exclude-dir=.git | grep -v "demi-engine"
```

### **Git Repository Updates**
```bash
# Update remote origin if renaming repository
git remote set-url origin https://github.com/bobrossrtx/demi.git

# Update README badges
# GitHub Actions badges, license badges, etc.
```

### **IDE/Editor Configuration**
- [ ] VS Code workspace settings
- [ ] Debug launch configurations
- [ ] IntelliSense paths

## ✅ **Verification Commands**

Run these after rename to ensure everything works:

```bash
# Build test
make clean && make -j4

# Runtime test
./bin/demi-engine --help
./bin/demi-engine examples/hello_world.asm

# Unit tests
./bin/demi-engine --test

# Memory test
valgrind ./bin/demi-engine examples/simple_addition.asm

# File reference check
find . -name "*.cpp" -o -name "*.hpp" | xargs grep -l "demi-engine\|DemiEngine" | grep -v .git
```

## 🎯 **Success Criteria**

The rename is successful when:
- [x] Project builds without errors
- [x] All tests pass
- [x] No "demi-engine" or "DemiEngine" references remain (except in git history)
- [x] Binary runs and processes assembly files correctly
- [x] Debug GUI opens with "DemiEngine" branding
- [x] Help text shows "demi-engine" usage

## 📝 **Post-Rename Tasks**

After successful rename:
1. Update GitHub repository name and description
2. Update any external documentation or wikis
3. Notify users of the name change
4. Consider creating redirect from old repository name
5. Update package manager entries if applicable
