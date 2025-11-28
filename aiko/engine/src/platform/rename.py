import os
import sys

# py .\rename.py ./bgfx opengl bgfx

def rename_files(folder_path, word_to_find, word_to_replace):
    # Check if folder exists
    if not os.path.isdir(folder_path):
        print(f"Error: The folder '{folder_path}' does not exist.")
        return

    for filename in os.listdir(folder_path):
        old_path = os.path.join(folder_path, filename)

        # Only rename if it's a file
        if os.path.isfile(old_path) and word_to_find in filename:
            new_filename = filename.replace(word_to_find, word_to_replace)
            new_path = os.path.join(folder_path, new_filename)

            # Rename the file
            os.rename(old_path, new_path)
            print(f"Renamed: {filename} -> {new_filename}")

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python rename_files.py <folder_path> <word_to_find> <word_to_replace>")
    else:
        folder = sys.argv[1]
        find_word = sys.argv[2]
        replace_word = sys.argv[3]
        rename_files(folder, find_word, replace_word)
