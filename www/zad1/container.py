''' Classes displayed in templates '''
import locale
import logging
from abc import abstractproperty
from os.path import join
import jinja2

# pylint: disable=R0903, R0913


class Link:
    ''' Objects convertable to <a href="{{ href }}">{{ name }}</a> '''
    def __init__(self, name, href):
        self.name = name
        self.href = href

    def __repr__(self):
        return '<{}: name={}, href={}>'.format(
            self.__class__.__name__, self.name, self.href)


class PrefLink(Link):
    ''' Link with specific href format '''

    @abstractproperty
    def href_pref(self):
        ''' Prefix added before href '''
        pass

    @abstractproperty
    def name_pref(self):
        ''' Prefix added before name '''
        pass

    def __init__(self, _id, name=None):
        href = '_'.join([self.href_pref, str(_id) + '.html']
                       ).replace(' ', '_').replace('/', '_')
        name = str(_id) if not name else name
        super().__init__(name, href)


class Table:
    ''' Simple table with columns and values '''
    def __init__(self, columns, values):
        self.columns = columns
        self.values = values


def log_create_html(file_path):
    ''' Logs creation of a HTML file '''
    logging.info("[CREATED] HTML FILE %(path)s ", {"path": file_path})


class Container:
    ''' Container generating all data except Poland '''
    def __init__(self, link: Link, place: list = None, links: list = None,
                 image: Link = None, table: Table = None, table_cand: Table = None):
        self.link = link
        self.links = links
        self.image = image
        self.table = table
        self.table_cand = table_cand
        if not place:
            place = []
        self.place = place + [link]

    def save(self, data):
        ''' Generates HTML page and saves it to the directory'''
        con = self
        con.place = [Link('Polska', data['krajurl'])] + self.place
        file_path = join(data['htmldir'], self.link.href)
        html = jinja2.Environment(loader=jinja2.FileSystemLoader(data['templates'])).get_template(
            'table.html').render({
                "container": con,
                "css": join("..", data['cssdir'], data['cssfile'])})

        with open(file_path, "w") as html_file:
            html_file.write(html)
            log_create_html(file_path)


class Map:
    ''' Container generating Poland's map '''
    def __init__(self, image: Link = None):
        self.image = image

    def save(self, data, woj_dict):
        ''' Generates HTML page and saves it to the directory'''
        locale.setlocale(locale.LC_COLLATE, "pl_PL.UTF-8")
        file_path = join(data['htmldir'], data['krajurl'])
        wojs = [{"cls": 'pl' + str(i + 1),
                 "name": woj_dict[x].name,
                 "href": woj_dict[x].href} for i,
                x in enumerate(sorted(woj_dict,
                                      key=locale.strxfrm))]

        html = jinja2.Environment(
            loader=jinja2.FileSystemLoader(
                data['templates'])).get_template('kraj.html').render({
                    "container": self,
                    "wojs": wojs,
                    "css": join(
                        "..",
                        data['cssdir'],
                        data['cssfile'])})

        with open(file_path, "w") as html_file:
            html_file.write(html)
            log_create_html(file_path)


class OkragLink(PrefLink):
    ''' Link generated for Okrag '''
    name_pref = 'Okrąg'
    href_pref = "okr"


class WojewodztwoLink(PrefLink):
    ''' Link generated for Wojewodztwo '''
    name_pref = 'Województwo'
    href_pref = 'woj'


class GminaLink(PrefLink):
    ''' Link generated for Gmina '''
    name_pref = 'Gmina'
    href_pref = 'gm'


class WardLink(PrefLink):
    ''' Link generated for Ward '''
    name_pref = 'Obwod'
    href_pref = 'obw'
