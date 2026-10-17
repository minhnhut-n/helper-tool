from pathlib import Path
import os

#regex to parse file name and format
import re
#for moving file to another place
import shutil

DEST_PATH = Path("E:\\gitRepos\\helper-tool\\classifier\\result")
SOURCE_PATH = None

REGEX_PATTERN = r'\d+(?=_TH)' 


def process_this_file(fpath):

    #get filename
    fileName = fpath.name
    #search file
    match = re.search(REGEX_PATTERN, fileName)
    if match:
        num_class = match.group()
        print(f"filename: {fileName}")
        print(f"class: {num_class}")
    
        #check and create folder if not available
        folder_name = f"class_{num_class}"
        target = DEST_PATH / folder_name

        target.mkdir(parents=True, exist_ok=True)
        shutil.copy2(str(fpath), str(target))
        print(f"-> Moved to: {target}\n")

    else:
        print("Not found")

def main():
    global SOURCE_PATH

    #collect info
    src = input("Path need to check: ")
    SOURCE_PATH = src

    if SOURCE_PATH == "":
        raise ValueError("Path unavailable!")
    
    source_file = Path(SOURCE_PATH)
    
    
    print("Recursive scaning...")
    for item in source_file.rglob("*"):
        
        if item.is_file():
            process_this_file(item)

if __name__ == "__main__":
    main()

