''' Creates and manages tables and databases '''
import sqlite3
import re
import logging
from abc import abstractmethod
import numpy as np


TYPES = {
    int: 'INTEGER',
    str: 'TEXT',
}


def to_adapt(objects_list):
    ''' Creates a string from list of stringable objects '''
    return ', '.join([str(i) for i in objects_list])


def to_repr(objects_list):
    ''' Creates a string (tuple representation of a list) '''
    return "({})".format(to_adapt(objects_list))


def classname(cls):
    ''' Returns classname in lower case '''
    name = re.search("'(.*)'", str(cls)).group(0)
    return name.replace('\'', '').split('.')[-1].lower().format()


def dict_val_none(dic):
    ''' Returns dict with keys from dic and None values '''
    return {x: None for x in dic.keys()}


class Table:
    ''' Abstract table interface '''
    __inited__ = set()

    @classmethod
    @abstractmethod
    def fields(cls):
        ''' Returns dict, whose field_names are keys and field_types are values '''
        pass

    def adapt(self):
        ''' Returns string containing values in format acceptable by sqlite VALUES(...) '''
        return to_adapt(["'{}'".format(x) for x in self.values()])

    @abstractmethod
    def values(self):
        ''' Returns dict, whose field_names are keys and field_values are values '''
        pass

    @classmethod
    def addable(cls):
        ''' Returns fields of type int '''
        fields = cls.fields()
        return [x for x in sorted(fields) if fields[x] == int]

    @classmethod
    def fmt(cls):
        ''' Generates string in format: name[0] type[0], ..., name[n] type[n] '''
        field_list = ["{} {}".format(n, TYPES[t]) for n, t in cls.fields().items()]
        return to_adapt(field_list)

    @classmethod
    def fmt_short(cls):
        ''' Generates string in format: name[0], name[1], ..., name[n] '''
        field_list = ["{}".format(x) for x in sorted(cls.fields().keys())]
        return to_adapt(field_list)

    @classmethod
    def init_fields(cls):
        ''' Inits base classes and adds fields from base classes '''

        if super(cls) not in Table.__inited__ and hasattr(super(cls), 'init_fields'):
            super(cls).init_fields()

        if cls not in Table.__inited__:
            if super(cls) in Table.__inited__ and hasattr(super(cls), 'fields'):
                fields = super(cls).fields()
                for field_name in fields.keys():
                    cls.fields()[field_name] = fields[field_name]

        Table.__inited__.add(cls)
        logging.info("[INIT] FIELDS in class %(name)-15s", {'name':classname(cls)})

    @classmethod
    def create(cls, cur):
        ''' Creates empty table with fields as columns '''
        cls.init_fields()
        name = classname(cls)
        cmd = "CREATE TABLE {}({})".format(name, cls.fmt())

        logging.debug(cmd)
        cur.execute(cmd)

    def save(self, cur):
        ''' Inserts values into table '''
        table = classname(self.__class__)
        cmd = "INSERT INTO {}({}) VALUES ({})".format(
            table, self.__class__.fmt_short(), self.adapt())
        logging.debug(cmd)
        cur.execute(cmd)

    @classmethod
    def select(cls, cur, fields, where, **kwargs):
        ''' Returns selected fields in dbstr_mapping format '''
        return SelectForm(fields, where, **kwargs).run(cur, cls)

    @classmethod
    def select_format(cls, cur, fields, where, **kwargs):
        ''' Returns selected fields in dbstr_backmapping format '''
        head, res = SelectForm(fields, where, **kwargs).run(cur, cls)
        vec_backmapping = np.vectorize(dbstr_backmapping)
        return vec_backmapping(head), vec_backmapping(res)

    @classmethod
    def drop(cls, cur):
        ''' Removes the table from database '''
        name = classname(cls)
        cur.execute("drop table if exists {}".format(name))


class SelectForm:
    ''' Class with query parameters '''
    # pylint: disable=R0903
    def __init__(self, fields, where, distinct=None, select=None):
        self.fields = fields
        self.where = where
        self.distinct = distinct
        self.select = select

    def run(self, cur, ocls):
        ''' Runs select query to db and returns result '''
        table_name = classname(ocls)

        if not self.distinct:
            self.distinct = []

        distinct_text = ''
        if self.select:
            select_fields = ['Adres'] + [x for x in ocls.addable()]
            head = np.array(['Adres'] + ocls.addable())

        elif self.distinct == []:
            select_fields = self.fields + \
                ["SUM({}.{})".format(table_name, x) for x in ocls.addable()]
            head = np.array(self.fields + ocls.addable())
        else:
            distinct_text = 'DISTINCT'
            select_fields = self.distinct
            head = np.array(self.distinct)

        where_conditions = ['{}.{} = {}'.format(table_name, x, y) \
                            for x, y in zip(self.fields, self.where)]
        where_conditions = "" if self.where == [] else "WHERE " + 'AND '.join(where_conditions)
        cmd = "SELECT {} {} FROM {} {}" \
            .format(distinct_text, ', '.join(select_fields), table_name, where_conditions)

        logging.debug(cmd)
        cur.execute(cmd)

        res = np.array(cur.fetchall())
        return head, res



class FTable(Table):
    ''' Implementation of Table '''
    __fields__ = dict()

    @classmethod
    def fields(cls):
        return cls.__fields__

    @classmethod
    def ftype(cls, name):
        ''' Returns field type '''
        return cls.__fields__[name]

    def __repr__(self):
        ''' Returns string represantation '''
        return to_repr(self.values())

    def values(self):
        ''' Returns list of values in sorted by keys '''
        return [self.__dict__[x] for x in sorted(self.__dict__)]

    def getf(self, name):
        ''' Returns field value '''
        assert name in self.__dict__, "Wrong field name {}".format(name)
        return self.__dict__[name]

    def svf(self, name, value):
        ''' Set value field '''
        assert name in self.__dict__, "Wrong field name {} {}'".format(
            name, self.__fields__)

        self.__dict__[name] = value

    def __init__(self, **kwargs):
        assert self.__class__ in Table.__inited__, \
            " Table should be created or at least 'init_fielded' before creating instances "

        self.__dict__ = dict_val_none(self.__fields__)
        for kwarg in kwargs:
            self.svf(kwarg, kwargs[kwarg])



def get_connection_cursor(data):
    ''' Opens connection with database '''
    # pylint: disable=E1101
    conn = sqlite3.connect(data['database'])#, detect_types=sqlite3.PARSE_COLNAMES)
    cur = conn.cursor()
    return conn, cur



def dbstr_mapping(string):
    ''' Removes unacceptable chars '''
    return string.replace('-', '$').replace('\'', '*').replace('\n', ' ') \
        .replace('.', '_').replace(' ', '_')


def dbstr_backmapping(string):
    ''' Reverses dbstr_mapping ... sometimes '''
    return string.replace('_', ' ').replace('$', '-').replace('*', '\'') \
        .replace('_', '.')


def close_connection_cursor(conn, cur):
    ''' Closes connection with database '''
    conn.commit()
    cur.close()
    conn.close()
