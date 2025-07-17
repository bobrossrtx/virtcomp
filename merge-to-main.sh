#!/bin/bash

# VirtComp Branch to Main Merge Script
# This script merges changes from a source branch to main
# while excluding memory-bank/ and .github/*.chatmode.md files
#
# Usage:
#   ./merge-to-main.sh                    # Merges from private-memory-bank (default)
#   ./merge-to-main.sh feature-branch     # Merges from specified branch
#   ./merge-to-main.sh other-private      # Merges from other-private branch

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Parse command line arguments
if [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    echo "VirtComp Branch to Main Merge Script"
    echo "===================================="
    echo ""
    echo "This script merges changes from a source branch to main while"
    echo "excluding private files (memory-bank/ and .github/*.chatmode.md)"
    echo ""
    echo "Usage:"
    echo "  $0                    # Merges from private-memory-bank (default)"
    echo "  $0 [branch-name]      # Merges from specified branch"
    echo "  $0 -h, --help         # Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                    # Default: merge from private-memory-bank"
    echo "  $0 feature-auth       # Merge from feature-auth branch"
    echo "  $0 dev-experiments    # Merge from dev-experiments branch"
    echo ""
    echo "The script will:"
    echo "1. Analyze commits in source branch not in main"
    echo "2. Cherry-pick commits that don't modify private files"
    echo "3. Warn about commits with mixed public/private changes"
    echo "4. Merge clean commits to main"
    echo "5. Sync source branch with updated main"
    exit 0
fi

SOURCE_BRANCH="${1:-private-memory-bank}"  # Default to private-memory-bank if no argument
TARGET_BRANCH="main"

# Get current branch
CURRENT_BRANCH=$(git branch --show-current)

print_status "VirtComp Branch to Main Merge Script"
print_status "===================================="
print_status "Source branch: $SOURCE_BRANCH"
print_status "Target branch: $TARGET_BRANCH"

# Check if source branch exists
if ! git show-ref --verify --quiet refs/heads/$SOURCE_BRANCH; then
    print_error "Source branch '$SOURCE_BRANCH' does not exist"
    print_status "Available branches:"
    git branch --list
    exit 1
fi

# Verify we're on the source branch or switch to it
if [ "$CURRENT_BRANCH" != "$SOURCE_BRANCH" ]; then
    print_status "Current branch: $CURRENT_BRANCH"
    print_status "Switching to $SOURCE_BRANCH branch..."
    git checkout $SOURCE_BRANCH
fi

# Check if we have uncommitted changes
if ! git diff-index --quiet HEAD --; then
    print_error "You have uncommitted changes. Please commit or stash them first."
    git status --short
    exit 1
fi

# Get list of commits that are in source branch but not in main
print_status "Finding commits to merge..."
COMMITS_TO_CHECK=$(git log --oneline $TARGET_BRANCH..$SOURCE_BRANCH --format="%H")

if [ -z "$COMMITS_TO_CHECK" ]; then
    print_warning "No new commits found to merge from $SOURCE_BRANCH to $TARGET_BRANCH"
    exit 0
fi

# Create a temporary branch for the merge
TEMP_BRANCH="temp-merge-$(date +%s)"
print_status "Creating temporary branch: $TEMP_BRANCH"
git checkout $TARGET_BRANCH
git checkout -b $TEMP_BRANCH

# Cherry-pick commits that don't modify private files
print_status "Analyzing commits for private file changes..."
COMMITS_TO_MERGE=""
SKIPPED_COMMITS=""

for commit in $COMMITS_TO_CHECK; do
    # Check if commit modifies private files
    MODIFIED_FILES=$(git show --name-only --format="" $commit)

    # Check if any modified files are private
    PRIVATE_FILES_MODIFIED=$(echo "$MODIFIED_FILES" | grep -E "^(memory-bank/|\.github/.*\.chatmode\.md)" || true)

    if [ -z "$PRIVATE_FILES_MODIFIED" ]; then
        # This commit is safe to merge
        COMMITS_TO_MERGE="$COMMITS_TO_MERGE $commit"
        print_status "✓ Commit $commit: $(git show --format="%s" --no-patch $commit)"
    else
        # This commit has private files, check if it has public changes too
        PUBLIC_FILES_MODIFIED=$(echo "$MODIFIED_FILES" | grep -v -E "^(memory-bank/|\.github/.*\.chatmode\.md)" || true)

        if [ -n "$PUBLIC_FILES_MODIFIED" ]; then
            print_warning "⚠ Commit $commit has both public and private changes:"
            print_warning "  Private: $(echo $PRIVATE_FILES_MODIFIED | tr '\n' ' ')"
            print_warning "  Public: $(echo $PUBLIC_FILES_MODIFIED | tr '\n' ' ')"
            print_warning "  Manual merge required for this commit"
            SKIPPED_COMMITS="$SKIPPED_COMMITS $commit"
        else
            print_status "⊘ Commit $commit: private files only, skipping"
            SKIPPED_COMMITS="$SKIPPED_COMMITS $commit"
        fi
    fi
done

# Cherry-pick the safe commits
if [ -n "$COMMITS_TO_MERGE" ]; then
    print_status "Cherry-picking safe commits..."
    for commit in $COMMITS_TO_MERGE; do
        if git cherry-pick $commit; then
            print_success "✓ Cherry-picked $commit"
        else
            print_error "✗ Failed to cherry-pick $commit"
            print_status "Resolve conflicts manually, then run:"
            print_status "  git add ."
            print_status "  git cherry-pick --continue"
            print_status "  Then re-run this script"
            exit 1
        fi
    done
else
    print_warning "No safe commits found to merge"
fi

# Check if we have any changes to merge
if git diff --quiet $TARGET_BRANCH $TEMP_BRANCH; then
    print_warning "No changes to merge to $TARGET_BRANCH"
    git checkout $TARGET_BRANCH
    git branch -D $TEMP_BRANCH
    exit 0
fi

# Show what will be merged
print_status "Changes to be merged to $TARGET_BRANCH:"
git log --oneline $TARGET_BRANCH..$TEMP_BRANCH

# Confirm merge
echo
read -p "Do you want to proceed with the merge? (y/N): " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    print_status "Merge cancelled. Cleaning up..."
    git checkout $TARGET_BRANCH
    git branch -D $TEMP_BRANCH
    exit 0
fi

# Merge to main
print_status "Merging changes to $TARGET_BRANCH..."
git checkout $TARGET_BRANCH

# Fast-forward merge if possible, otherwise create merge commit
if git merge --ff-only $TEMP_BRANCH 2>/dev/null; then
    print_success "Fast-forward merge completed"
else
    git merge --no-ff $TEMP_BRANCH -m "merge: integrate changes from $SOURCE_BRANCH (excluding private files)"
    print_success "Merge commit created"
fi

# Clean up temporary branch
git branch -D $TEMP_BRANCH
print_success "Temporary branch cleaned up"

# Switch back to source branch and sync with main
print_status "Switching back to $SOURCE_BRANCH and syncing with $TARGET_BRANCH..."
git checkout $SOURCE_BRANCH

# Merge main into source branch to keep it in sync
git merge $TARGET_BRANCH --no-edit
print_success "Source branch synced with $TARGET_BRANCH"

# Summary
print_success "=========================================="
print_success "Merge completed successfully!"
print_status "Summary:"
print_status "- Merged changes from $SOURCE_BRANCH to $TARGET_BRANCH"
print_status "- Excluded memory-bank/ and .github/*.chatmode.md files"
print_status "- Synced $SOURCE_BRANCH with $TARGET_BRANCH"

if [ -n "$SKIPPED_COMMITS" ]; then
    print_warning "Skipped commits (manual review needed):"
    for commit in $SKIPPED_COMMITS; do
        print_warning "  $commit: $(git show --format="%s" --no-patch $commit)"
    done
fi

print_status "Current branch: $SOURCE_BRANCH"
print_status "Ready for continued development!"
