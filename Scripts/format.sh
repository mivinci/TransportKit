#!/bin/bash

FILES=$(git diff --name-only --diff-filter=d HEAD | grep -E '\.(h|cc)$')

if [ -z "$FILES" ]; then
    echo "No .h or .cc files to format."
    exit 0
fi

echo "🔍 Formatting modified .h and .cc files..."

for file in $FILES; do
  echo "🐳 $file"
done

clang-format -i --style=file $FILES
echo "✅  Done."

