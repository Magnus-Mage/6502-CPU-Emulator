#!/bin/bash

echo "Setting up git hooks for automatic formatting..."

# Pre-commit hook
cat > .git/hooks/pre-commit << 'EOF'
#!/bin/bash
STAGED_FILES=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(cpp|hpp|h)$')

if [ -n "$STAGED_FILES" ]; then
    echo "🔧 Formatting staged files..."
    for FILE in $STAGED_FILES; do
        clang-format -i "$FILE"
        git add "$FILE"
    done
    echo "✓ Files formatted"
fi
exit 0
EOF

chmod +x .git/hooks/pre-commit

echo "✅ Git hooks installed!"
echo ""
echo "Now your code will be automatically formatted on every commit."
echo ""
echo "To format all files manually, run:"
echo "  find . -name '*.cpp' -o -name '*.hpp' | xargs clang-format -i"
