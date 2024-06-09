#!/bin/bash

if [ -z "$1" ]; then
    echo "Error: No commit message provided."
    echo "Usage: $0 <commit_message>"
    exit 1
fi

commit_message="$1"

git submodule update --remote AzureNexus

git add .

git commit -m "$commit_message"

git push
