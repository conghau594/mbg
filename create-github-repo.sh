#!/bin/bash

# Script to create a new GitHub repository from CLI
# Usage: ./create-github-repo.sh

set -e

echo "🚀 GitHub Repository Creator"
echo "=============================="
echo ""

# Get user input
read -p "Enter your GitHub username: " GITHUB_USER
read -sp "Enter your Personal Access Token: " GITHUB_TOKEN
echo ""
read -p "Enter repository name: " REPO_NAME
read -p "Make repository private? (y/n): " IS_PRIVATE

# Set private flag
if [[ "$IS_PRIVATE" == "y" || "$IS_PRIVATE" == "Y" ]]; then
    PRIVATE_FLAG="true"
    echo "Creating PRIVATE repository..."
else
    PRIVATE_FLAG="false"
    echo "Creating PUBLIC repository..."
fi

# Create repository using GitHub API
echo ""
echo "📦 Creating repository '$REPO_NAME' on GitHub..."

RESPONSE=$(curl -s -u "$GITHUB_USER:$GITHUB_TOKEN" \
  https://api.github.com/user/repos \
  -d "{\"name\":\"$REPO_NAME\",\"private\":$PRIVATE_FLAG}")

# Check if successful
if echo "$RESPONSE" | grep -q "\"full_name\""; then
    echo "✅ Repository created successfully!"

    REPO_URL="https://github.com/$GITHUB_USER/$REPO_NAME.git"
    echo "📍 Repository URL: $REPO_URL"

    # Ask if user wants to add remote
    echo ""
    read -p "Add this repository as remote 'origin'? (y/n): " ADD_REMOTE

    if [[ "$ADD_REMOTE" == "y" || "$ADD_REMOTE" == "Y" ]]; then
        # Check if origin already exists
        if git remote get-url origin &>/dev/null; then
            echo "⚠️  Remote 'origin' already exists!"
            read -p "Remove existing remote and add new one? (y/n): " REMOVE_EXISTING
            if [[ "$REMOVE_EXISTING" == "y" || "$REMOVE_EXISTING" == "Y" ]]; then
                git remote remove origin
                git remote add origin "$REPO_URL"
                echo "✅ Remote updated!"
            else
                echo "❌ Cancelled. Remote not changed."
                exit 0
            fi
        else
            git remote add origin "$REPO_URL"
            echo "✅ Remote 'origin' added!"
        fi

        # Ask if user wants to push
        echo ""
        CURRENT_BRANCH=$(git branch --show-current)
        read -p "Push current branch ($CURRENT_BRANCH) to remote? (y/n): " DO_PUSH

        if [[ "$DO_PUSH" == "y" || "$DO_PUSH" == "Y" ]]; then
            echo "🚀 Pushing to GitHub..."
            git push -u origin "$CURRENT_BRANCH"
            echo "✅ Code pushed successfully!"
        else
            echo "ℹ️  You can push later with: git push -u origin $CURRENT_BRANCH"
        fi
    fi

else
    echo "❌ Failed to create repository!"
    echo ""
    echo "Response from GitHub:"
    echo "$RESPONSE" | grep "message" || echo "$RESPONSE"
    echo ""
    echo "Common issues:"
    echo "  - Invalid Personal Access Token"
    echo "  - Token missing 'repo' scope"
    echo "  - Repository name already exists"
    exit 1
fi

echo ""
echo "🎉 All done!"
