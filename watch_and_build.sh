#!/bin/bash
# Watch for file changes and auto-rebuild

# ===== CONFIGURATION =====
# Number of file changes before triggering a rebuild
# Set to 1 for immediate rebuild, higher values wait for more changes
CHANGE_THRESHOLD=1

# Timeout in seconds - rebuild if no new changes after this time
# Even if threshold isn't reached, build after this delay
TIMEOUT_SECONDS=1
# =========================

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${YELLOW} Watching for changes in src/ and include/${NC}"
echo -e "${BLUE} Config: Rebuild after ${CHANGE_THRESHOLD} changes or ${TIMEOUT_SECONDS}s timeout${NC}"
echo "Press Ctrl+C to stop"
echo ""

change_count=0
last_change_time=0

build() {
    echo -e "\n${YELLOW} Rebuilding after ${change_count} change(s)...${NC}"
    change_count=0
    
    # Build the GUI executable
    if cmake --build build --target ez_architecture_gui 2>&1 | grep -E "(error|warning|Built target)"; then
        if [ ${PIPESTATUS[0]} -eq 0 ]; then
            echo -e "${GREEN} Build successful!${NC}\n"
            # Optionally auto-run (commented out by default)
            # ./build/bin/ez_architecture_gui &
        else
            echo -e "${RED}❌ Build failed!${NC}\n"
        fi
    fi
}

while true; do
    # Wait for any .cpp or .hpp file to be modified (with timeout)
    if inotifywait -q -r -e modify,create,delete -t $TIMEOUT_SECONDS src/ include/ 2>/dev/null; then
        change_count=$((change_count + 1))
        last_change_time=$(date +%s)
        
        echo -e "${BLUE} Change detected (${change_count}/${CHANGE_THRESHOLD})${NC}"
        
        # Check if we've hit the threshold
        if [ $change_count -ge $CHANGE_THRESHOLD ]; then
            build
        fi
    else
        # Timeout occurred - build if we have pending changes
        if [ $change_count -gt 0 ]; then
            echo -e "${BLUE}  Timeout reached${NC}"
            build
        fi
    fi
done
