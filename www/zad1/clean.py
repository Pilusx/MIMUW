''' Removes files from selected directories '''
from shutil import rmtree
import logging
from os.path import exists

from general import get_data

def main():
    ''' Main function '''
    data = get_data()

    directories = [data['htmldir'], data['imgdir']]

    for key in data['directories']:
        directories.append(data['directories'][key]['directory'])

    for directory in directories:
        if exists(directory):
            rmtree(directory)
            info = "RMDIR"
        else:
            info = "NOT EXISTS"

        logging.info("[%(info)s] %(dir)s", {"info": info, "dir" : directory})

main()
