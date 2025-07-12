''' Defines data stored in database '''
import enum
import database as db


class Ward(db.FTable):
    ''' Tabela obwodów wyborczych '''
    pass


class Columns(enum.Enum):
    ''' Numery wybranych kolumn w tabeli: column_name = column_id '''
    NR_OKR = 0
    KOD_GMINY = 1
    GMINA = 2
    POWIAT = 3
    NR_OBW = 4
    TYP_OBW = 5
    ADRES = 6
    WOJEWODZTWO = 7
    ID = 8


def idx(column_name):
    ''' Zwraca numer kolumny o podanej nazwie '''
    return Columns.__dict__[column_name].value


def init_tables(**kwargs):
    ''' Inicjuje pola klasy tabela. '''
    for ftable in TABLES:
        ftable.init_fields(**kwargs)


def create_tables(cur, **kwargs):
    ''' Tworzy tabele w bazie danych na podstawie listy klas TABLES'''
    init_tables(**kwargs)

    for ftable in TABLES:
        ftable.drop(cur, **kwargs)
        ftable.create(cur, **kwargs)


TABLES = [Ward]
