#!/bin/bash
set -e

REPO_PATH="$HOME/Test/AquaGuard"
REPO_NAME="AquaGuard"
GIT_USER="mingunkim123"     # <-- GitHub 사용자명으로 수정
GIT_URL="https://github.com/mingunkim123/AquaGuard.git"

cd "$REPO_PATH"

if [ ! -d ".git" ]; then
    echo "[+] Initializing new git repo..."
    git init
    git branch -M main
    git remote add origin "$GIT_URL"
fi

echo "[+] Adding files..."
git add .

echo "[+] Committing..."
git commit -m "Auto upload from Arduino IDE"

echo "[+] Pushing to GitHub..."
git push -u origin main

echo "[✅] Upload complete!"
