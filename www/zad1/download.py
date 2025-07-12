''' Download data from webpage '''
import urllib
import urllib.request
from general import lts, lgfs, check_json, ensure_exist_dir


def retrieve(urlroot, filename, directory, print_all_logs=False):
    ''' Downloads file 'urlroot' + 'filename' to the directory '''
    file_link = lts([urlroot, filename])
    file_destination = lts([directory, lgfs(filename)])  # parsuj tylko koniec
    urllib.request.urlretrieve(file_link, file_destination)

    if print_all_logs:
        print("[DOWNLOADS] FROM {} TO {}".format(file_link, file_destination))

    return file_destination


def download(urlroot, urldir, prefix, directory, **kwargs):
    '''
    Downloads all files, whose name contains prefix, that can be reached
    via direct link from the webpage 'urlroot + urldir' to 'directory'
    '''
    from bs4 import BeautifulSoup

    url = lts([urlroot, urldir])
    print("[READING] {} ".format(url))
    html = urllib.request.urlopen(url).read()
    ensure_exist_dir(directory)

    counter = 0
    soup = BeautifulSoup(html, 'html.parser')
    links = soup.findAll(lambda tag: tag.name == 'a')
    for link in links:
        href = link['href']
        if prefix in href:
            retrieve(urlroot, href, directory, **kwargs)
            counter += 1

    print("[DOWNLOADED] {} FILES FROM {} TO {}".format(counter, url, directory))


def zipdownload(urlroot, urldir, filename, directory, **kwargs):
    ''' Downloads zip file, unzips it, and deletes the .zip file '''
    import zipfile
    from os import remove

    download(urlroot, urldir, filename, directory, **kwargs)
    file_path = lts([directory, filename])
    zip_ref = zipfile.ZipFile(file_path, 'r')
    zip_ref.extractall(directory)
    zip_ref.close()
    print("[EXTRACTS] FROM {} TO {}".format(file_path, directory))
    remove(file_path)
    print("[REMOVES] {}".format(file_path))

def main():
    ''' Downloads files from selected categories '''
    import json
    json_path = check_json()

    with open(json_path) as json_data:
        data = json.load(json_data)
        root = data["urlroot"]
        logs = data['print_all_logs']
        directories = data['directories']
        for key in directories:
            query = directories[key]
            typ = query['type']

            if typ == 'all_xls':
                func = download
            elif typ == 'zip':
                func = zipdownload
            else:
                return

            func(root, query['urldir'], query['prefix'], query['directory'], print_all_logs=logs)


main()
