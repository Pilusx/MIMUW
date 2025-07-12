''' Script generating pages '''
import numpy as np
import models as mod
from models import idx
import general as gen
import container as cnt
import database as db


HTuple = gen.HashTuple
RESULTS = 17  # Liczba kolumn bezposrednio opisujaca wynik


def prepare_db(data, cur):
    ''' Prepares db for using selects '''

    ob_head, obwody = gen.read(data, data['datasrc'])
    res_head = ob_head[-RESULTS:]

    wojewodztwa = gen.wojewodztwa(data)

    def gm_woj(row):
        ''' Maps 'KOD_GMINY' to the name of 'voivodeship' '''
        return wojewodztwa[int(row[1])//20000 - 1]

    ob_head = np.insert(ob_head, -RESULTS, 'Województwo')
    obwody = np.insert(obwody, -RESULTS, np.array([gm_woj(x) for x in obwody]), axis=1)
    ob_head = np.insert(ob_head, -RESULTS, 'Id')
    obwody = np.insert(obwody, -RESULTS, np.array(range(len(obwody))), axis=1)

    # Ustawia pola w bazie danych
    for col_name in res_head:
        mod.Ward.fields()[col_name] = int

    for col_name in ob_head[:-RESULTS]:
        mod.Ward.fields()[col_name] = str

    mod.create_tables(cur)

    for obwod in obwody:
        mod.Ward(**dict(zip(ob_head, obwod))).save(cur)

    ob_head, obwody = ob_head[:-RESULTS], obwody[:, :-RESULTS]
    return cnt.Table(ob_head, obwody)


def divide_columns(cols):
    ''' Divides columns into stats columns and results columns '''
    stats = lambda x: x <= 2
    graph = lambda x: x > 2
    filtering = lambda lam: np.array([i for i, x in enumerate(cols) if lam(gen.n_upper_chars(x))])
    return filtering(stats), filtering(graph)


def dict_link(obwody, cls, idx_name, idx_value=None):
    ''' Creates a dictionary, whose values are links '''
    index = idx(idx_name)
    if idx_value:
        value = idx(idx_value)
    else:
        value = index
    place = gen.decrease(obwody, [index])
    return {x[index]: cls(db.dbstr_backmapping(x[index]),
                          name=db.dbstr_backmapping(x[value]))
            for x in place}


def mapping(dic, row, index):
    ''' Maps element to list with one mapped element '''
    return [dic[row[index]]]


class Generator:
    ''' Enables page generation '''
    # pylint: disable=R0902,R0913

    name = property(lambda self: self.smallmap[0])
    my_dict_link = property(lambda self: self.smallmap[1])
    son = property(lambda self: self.sonmap[0])
    son_dict_link = property(lambda self: self.sonmap[1])
    places = property(lambda self: self.placemap[0])
    places_mapping = property(lambda self: self.placemap[1])

    def __init__(self, data, short_select, smallmap, \
            sonmap=(None, None), gen_img=False, \
            placemap=(None, None), select=False):
        self.smallmap = smallmap
        self.sonmap = sonmap
        self.gen_img = gen_img
        self.placemap = placemap
        self.select = select
        self.data = data
        self.short_select = short_select
        self.kwarg = dict()

    def generate_table(self, elem, method=mod.Ward.select_format):
        ''' Appends table to container '''
        table = self.short_select([self.name], [elem], method=method)
        stats, graph = divide_columns(table.columns)

        results = cnt.Table(table.columns[stats], table.values[:, stats])
        self.kwarg['table'] = results
        return table, graph

    def generate_table_cand(self, elem, method=mod.Ward.select_format):
        ''' Appends table_cand to container '''
        if self.select:
            table = self.short_select([self.name], [elem], method=method, select=self.select)
            _, graph = divide_columns(table.columns)
            graph = np.insert(graph, 0, 0)  # Poprawić na adres

            results = cnt.Table(table.columns[graph], table.values[:, graph])
            self.kwarg['table_cand'] = results

    def generate_links(self, elem, table, method=mod.Ward.select_format):
        ''' Appends the links parameter for container '''
        if not self.select and self.son and self.son_dict_link:
            table = self.short_select([self.name], [elem], distinct=[self.son], method=method)
            links = [self.son_dict_link[x] for [x] in table.values]
            self.kwarg['links'] = links


    def generate_image(self, table, link, graph):
        ''' Appends image or table_cand to container '''
        if self.gen_img:
            img_name = link.name.replace(' ', '_')
            img = gen.pie_graph(self.data, img_name, table.columns[graph], \
                table.values[:, graph].squeeze().astype(int))
            self.kwarg['image'] = img
        elif not self.select:
            results = cnt.Table(table.columns[graph], table.values[:, graph])
            self.kwarg['table_cand'] = results


    def generate_places(self, elem):
        ''' Appends a parameter 'place' for container '''
        if self.places and self.places_mapping:
            plc = self.short_select([self.name], [elem], distinct=self.places)
            place = self.places_mapping(plc.values[0])
            self.kwarg['place'] = place

    def generate(self, columns):
        ''' Generates group of pages of given category '''
        for key in sorted(self.my_dict_link):
            elem = key
            link = self.my_dict_link[key]

            if mod.Ward.ftype(columns[idx(self.name)]) == str:
                elem = '\'{}\''.format(key)

            table, graph = self.generate_table(elem)
            self.generate_image(table, link, graph)
            self.generate_table_cand(elem)
            self.generate_links(elem, table)
            self.generate_places(elem)

            cnt.Container(link, **self.kwarg).save(self.data)


def generate_country(data, short_select, woj_link):
    ''' Generates a map with voivodeships '''
    table = short_select(['ID'], ['0 OR 0=0'])
    _, graph = divide_columns(table.columns)
    img_name = data['krajurl']
    labels = np.array([db.dbstr_backmapping(x) for x in table.columns[graph]])
    wyniki = gen.pie_graph(data, img_name, labels, table.values[:, graph].squeeze().astype(int))

    cnt.Map(image=wyniki).save(data, woj_link)


def main():
    ''' Main function '''
    data = gen.get_data()
    conn, cur = db.get_connection_cursor(data)

    gen.copy_css_map_poland(data)

    # READ DATA
    obwody = prepare_db(data, cur)

    gm_link = dict_link(obwody.values, cnt.GminaLink, 'KOD_GMINY', 'GMINA')
    woj_link = dict_link(obwody.values, cnt.WojewodztwoLink, 'WOJEWODZTWO')
    ok_link = dict_link(obwody.values, cnt.OkragLink, 'NR_OKR')
    obw_link = dict_link(obwody.values, cnt.WardLink, 'ID', 'ADRES')

    print(len(obw_link), len(gm_link), len(ok_link), len(woj_link))


    def short_select(col_names, values, method=mod.Ward.select, distinct=None, **kwargs):
        ''' Shortened version of mod.Ward.select, selecting columns with col_names and results_names
            where col_names[i] == values[i] '''
        kwarg_distinct = None
        if distinct:
            kwarg_distinct = [obwody.columns[idx(x)] for x in distinct]
        cols, values = method(cur, [obwody.columns[idx(i)] for i in col_names], values, \
            distinct=kwarg_distinct, **kwargs)
        return cnt.Table(cols, values)

    # GENERATE RESULTS
    generate_country(data, short_select, woj_link)

    def generate(*args, **kwargs):
        ''' Generating local function '''
        Generator(data, short_select, *args, **kwargs).generate(obwody.columns)

    oksmap = ('NR_OKR', ok_link)
    gmsmap = ('KOD_GMINY', gm_link)

    # GENERATE PAGES
    generate(('WOJEWODZTWO', woj_link), sonmap=oksmap, gen_img=True)

    woj_map = lambda x: mapping(woj_link, x, 0)
    places = ['WOJEWODZTWO']
    generate(oksmap, sonmap=gmsmap, placemap=(places, woj_map), gen_img=False)

    places = places + ['NR_OKR']
    okr_map = lambda x: woj_map(x) + mapping(ok_link, x, 1)
    generate(gmsmap, sonmap=('ADRES', obw_link), \
        placemap=(places, okr_map), select=True, gen_img=False)

    db.close_connection_cursor(conn, cur)


main()
