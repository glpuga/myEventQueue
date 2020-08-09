#!/usr/bin/python3
#
# Copyright 2020, Gerardo Puga
# Automatic source formatter, using clang-format
#

# System modules
import os

FOLDERS_TO_ANALYZE = ['src', 'include', 'test']
EXTENSIONS_TO_PROCESS = ['c', 'cpp', 'h', 'hpp']
CMAKE_COMMAND = 'clang-format'

def process_files_in_sub_folder(dirpath, files_list):
    """
    Filter a list of full file paths whose file extension is within the target list.
    """
    files_to_process_in_folder = []
    for filename in files_list:
        for extension in EXTENSIONS_TO_PROCESS:
            if filename.endswith('.' + extension):
                files_to_process_in_folder.append(dirpath + '/' + filename)
    return files_to_process_in_folder

def process_root_folder(folder):
    """
    Walk recursively one of the root search folders, return
    a list of full filepaths within that root folder
    """
    files_to_process_in_root_folder = []
    for (curdir, subdirs, subfiles) in os.walk('./' + folder):
        print('Walking through %s '%(curdir), end="")
        sublist = process_files_in_sub_folder(curdir, subfiles)
        files_to_process_in_root_folder.extend(sublist)
        print('[%d files found]'%(len(files_to_process_in_root_folder)))
    return files_to_process_in_root_folder

def get_list_of_files():
    """
    Get a list of all the files of interest within any of the root search folders
    """
    files_to_process = []
    for folder in FOLDERS_TO_ANALYZE:
        print("Processing root folder: %s"%(folder))
        sublist = process_root_folder(folder)
        files_to_process.extend(sublist)
    return files_to_process

def process_queue(filelist):
    """
    Use the formater on each of the files in the file list, to fix their format.
    """
    for filepath in filelist:
        print('Processing ' + filepath)
        command = CMAKE_COMMAND + ' -i \"' + filepath + '\"'
        os.system(command)

def clang_is_ok():
    command = 'which ' + CMAKE_COMMAND + ' > /dev/null'
    retcode = os.system(command)
    return True if retcode == 0 else False

def main():
    if clang_is_ok():
        files_to_process = get_list_of_files()
        if files_to_process:
            process_queue(files_to_process)
        else:
            print("ERROR: No files to process were found. Are you at the root folder of the project?")
    else:
        print("ERROR: Could not find %s"%(CMAKE_COMMAND))

if __name__ == '__main__':
    main()
