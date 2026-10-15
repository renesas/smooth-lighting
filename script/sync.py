import hashlib
import shutil
from pathlib import Path

# Sync directories
src_dir = Path(r"src")
target_dirs = [
    Path(r"test\Smooth_LED_Lighting_FPB_RA4E2\src\rs_smooth_led_lighting"),
    Path(r"test\unity\Smooth_LED_Lighting_FPB_RA4E2\src\rs_smooth_led_lighting")
]
#target_dirs = [
#    Path(r"test\Smooth_LED_Lighting_FPB_RA4E2\src\rs_smooth_led_lighting"),
#    Path(r"test\unity\Smooth_LED_Lighting_FPB_RA4E2\src\rs_smooth_led_lighting")
#]
def get_file_md5(file_path: Path) -> str:
    """Check differences..."""
    hash_md5 = hashlib.md5()
    with open(file_path, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_md5.update(chunk)
    return hash_md5.hexdigest()

def sync_and_overwrite(source: Path, targets: list[Path]):
    if not source.exists():
        print(f"Source directory does not exist: {source}")
        return

    # Files in the src directory
    src_files = {f.relative_to(source): f for f in source.rglob("*") if f.is_file()}

    for target_dir in targets:
        if not target_dir.exists():
            target_dir.mkdir(parents=True, exist_ok=True)

        # Delete files in targets that are no longer exist in src
        for target_file in list(target_dir.rglob("*")):
            if target_file.is_file():
                rel_path = target_file.relative_to(target_dir)
                if rel_path not in src_files:
                    target_file.unlink()
                    print(f"Deleted extra file from targest: {rel_path}")

        # Copy missing or modified files
        for rel_path, src_file in src_files.items():
            target_file = target_dir / rel_path

            if not target_file.exists():
                # File is missing
                target_file.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(src_file, target_file)
                print(f"Sync job:Copied (New): {rel_path} -> {target_dir}")
            else:
                # File exists, compare hashes to diff
                if get_file_md5(src_file) != get_file_md5(target_file):
                    shutil.copy2(src_file, target_file)
                    print(f"Sync job: Copied (Overwrote Modified): {rel_path} -> {target_dir}")
                else:
                    print(f"Sync job: Identical: {rel_path}")

if __name__ == "__main__":
    sync_and_overwrite(src_dir, target_dirs)
    print("Sync completed!")
