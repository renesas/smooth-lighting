import filecmp
import os
import sys


def are_dirs_identical(dir1, dir2):
    """Recursively and deeply checks if two directories are perfectly synchronized."""
    d1 = os.path.normpath(dir1)
    d2 = os.path.normpath(dir2)

    if not os.path.exists(d1) or not os.path.exists(d2):
        print(f"Error: Path does not exist -> {d1} or {d2}")
        return False

    comparison = filecmp.dircmp(d1, d2)

    # Structural Check
    if comparison.left_only or comparison.right_only:
        if comparison.left_only:
            print(f"Missing in target: {comparison.left_only} from {d1}")
        if comparison.right_only:
            print(f"Extra files in target: {comparison.right_only} in {d2}")
        return False

    # Content Check
    for filename in comparison.common_files:
        file1 = os.path.join(d1, filename)
        file2 = os.path.join(d2, filename)
        # shallow=False forces Python to read and compare the actual byte data
        if not filecmp.cmp(file1, file2, shallow=False):
            print(f"Content mismatch in file: {os.path.join(d1, filename)}")
            return False

    # System / Unreadable file checks
    if comparison.funny_files or comparison.common_funny:
        print(
            f"Cannot compare or access system files: {comparison.funny_files or comparison.common_funny}"
        )
        return False

    # 4. Deep Recursive Check for Subdirectories
    for common_dir in comparison.common_dirs:
        sub_dir1 = os.path.join(d1, common_dir)
        sub_dir2 = os.path.join(d2, common_dir)
        if not are_dirs_identical(sub_dir1, sub_dir2):
            return False

    return True

# Check src vs targets
src_dir = "./src"
target1_dir = (
    "./test/Smooth_LED_Lighting_FPB_RA4E2/src/rs_smooth_led_lighting"
)
target2_dir = "./test/unity/Smooth_LED_Lighting_FPB_RA4E2/src/rs_smooth_led_lighting"

print("Starting pipeline synchronization check...")

print(f"Comparing directories: {src_dir} <---> {target1_dir}")
sync_target1 = are_dirs_identical(src_dir, target1_dir)

print(f"Comparing directories: {src_dir} <---> {target2_dir}")
sync_target2 = are_dirs_identical(src_dir, target2_dir)

if not sync_target1 or not sync_target2:
    print(
        "\n Comparing failed. Pipeline stopped. Directories are out of sync."
    )
    sys.exit(1)

print("\n Target directories matched")
sys.exit(0)
