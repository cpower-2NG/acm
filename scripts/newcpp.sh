#!/usr/bin/env bash
set -euo pipefail

if [ $# -ne 1 ]; then
  echo "Usage: $0 <target-file>"
  exit 1
fi

target="$1"
template="$(dirname "$0")/../templates/cpp/base.cpp"

if [ ! -f "$template" ]; then
  echo "Error: template not found: $template"
  exit 1
fi

if [ -e "$target" ]; then
  echo "Error: target exists: $target"
  exit 1
fi

mkdir -p "$(dirname "$target")"
cp "$template" "$target"
echo "Created: $target"
code "$target"
