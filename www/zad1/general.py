''' Functions of general use '''
import logging
from os import listdir
from os.path import join, exists, isdir
import shutil
import numpy as np
import matplotlib
# pylint: disable=C0413
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import container as cnt
import database as db




def lts(string_list):
    ''' List to string '''
    return "/".join(string_list)


def lgfs(string):
    ''' Last group from string '''
    return string.split('/')[-1]

# https://stackoverflow.com/questions/10953189/count-lower-case-characters-in-a-string


def n_upper_chars(string):
    ''' Returns the number of upper chars '''
    return sum(1 for c in string if c.isupper())


def ensure_exist_dir(directory):
    ''' Makes a directory if it doesn't exist ... '''
    from os import mkdir

    if not exists(directory):
        mkdir(directory)


def check_json():
    ''' Checks if json exists and returns its name or exits '''
    import sys

    if len(sys.argv) != 2:
        raise ValueError("[USAGE] python {} <filename>".format(sys.argv[0]))

    json_path = sys.argv[1]

    if not exists(json_path):
        raise FileNotFoundError("[FILE] {} doesn't exist".format(json_path))

    return json_path

# JSON


def get_data():
    ''' Returns config json '''
    import json
    with open(check_json()) as json_data:
        data = json.load(json_data)
        logging.getLogger().setLevel(logging.__dict__[data['logging']])
        return data

# GRAPH
def pie_graph(data, name, labels, values):
    ''' Generates graph and saves it to the file '''
    ensure_exist_dir(data['imgdir'])
    href = join(data['imgdir'], name + ".png")

    max_val = values.max()
    v_sum = values.sum()
    threshold = v_sum // 30

    percentage_labels = ['{} {:.2f}%'.format(x, 100 * values[i] / v_sum)
                         for i, x in enumerate(labels)]
    wrapped_labels = [x.replace(' ', '\n') if values[i] >
                      threshold else '' for i, x in enumerate(percentage_labels)]
    explode = np.array([0.1 if x == max_val else 0 for x in values])

    patches, _ = plt.pie(values, radius=1, explode=explode, \
                         labels=wrapped_labels, shadow=False, startangle=90, labeldistance=0.7)

    first_group_len = len(patches) // 2
    plot_1 = plt.legend(patches[first_group_len:], percentage_labels[first_group_len:], loc='best')
    plt.legend(patches[:first_group_len], percentage_labels[:first_group_len], loc='best')
    plt.gca().add_artist(plot_1)

    plt.axis('equal')
    plt.tight_layout()
    plt.savefig(href)
    plt.close()

    logging.info("[IMG] CREATED in %(href)s", {'href':href})
    return cnt.Link(name, join('..', href))


# XLS
def get_xls(file_path):
    ''' Return column names and values '''
    import pandas as pd
    logging.info("[OPENED] %(file_path)-30s", {'file_path': file_path})
    excel_file = pd.ExcelFile(file_path)
    dataframe = excel_file.parse(excel_file.sheet_names[0])
    return dataframe.columns.values, np.array(dataframe)


def get_dir(data, category):
    ''' Returns directory of files of given category '''
    return data['directories'][category]['directory']


def read(data, category):
    ''' Returns headers and values of merged tables of given category '''
    logging.info("[READING] %(cat)-20s", {'cat': category})
    directory = get_dir(data, category)

    values = []
    for file in listdir(directory):
        head, vals = get_xls(join(directory, file))
        values.append(vals)

    head = np.array([db.dbstr_mapping(x) for x in head])
    values = np.array([np.array([db.dbstr_mapping(z)
                                 if isinstance(z, str) else z for z in x])
                       for y in np.array(values) for x in y])

    return head, values


def dict_inplace(dic, keys, new_key=None):
    ''' Replaces keys in dict by new_key or removes elements '''
    val = dic[keys[0]]
    for key in keys:
        del dic[key]

    if new_key != None:
        dic[new_key] = val

# VOIVODESHIPS
def wojewodztwa(data):
    ''' Returns list of sorted by keys pairs (powiat: wojewodztwo) '''
    import urllib
    from bs4 import BeautifulSoup

    url = data['wojewodztwaurl']
    html = urllib.request.urlopen(url).read()
    soup = BeautifulSoup(html, 'html.parser')
    table = soup.find(lambda tag: tag.has_attr('class')
                      and 'sortable' in tag['class'])
    res = set()
    for row in table.findAll(lambda tag: tag.name == 'tr'):
        link = row.findAll(lambda tag: tag.name == 'a')
        powiat = link[0].text
        woj = link[1].text
        if powiat != 'Powiat':
            res.add(woj)

    return list(sorted({db.dbstr_mapping(x) for x in res}))


def copy_css_map_poland(data):
    ''' Copies all files necessary files for cssmap to work correctly to 'htmldir' '''
    src = data['cssmap_poland_dir']
    dst = data['htmldir']
    ensure_exist_dir(dst)
    for directory in listdir(src):
        src_temp = join(src, directory)
        if not isdir(src_temp):
            continue
        dst_temp = join(dst, directory)
        if exists(dst_temp):
            logging.info("[EXISTS] %(dst)s", {"dst": dst_temp})
        else:
            shutil.copytree(src_temp, dst_temp)
            logging.info("[COPY] FROM %(from)s TO %(to)s", {"from": src_temp, "to": dst_temp})



# OTHER
class HashTuple:
    ''' Tuple that can be an element of a set '''
    # pylint: disable=R0903
    def __init__(self, values, idx):
        self.values = values
        self.idx = idx

    def __hash__(self):
        return self.values[self.idx].tostring().__hash__()

    def __eq__(self, other):
        assert self.idx == other.idx
        for idx in self.idx:
            if self.values[idx] != other.values[idx]:
                return False
        return True


def decrease(array, idx):
    ''' Reduces data to such with unique data at idx's positions '''
    return np.array([y.values for y in set([HashTuple(x, idx) for x in array])])
