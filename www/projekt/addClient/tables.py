import django_tables2 as tables


class PizzaTable(tables.Table):
    id = tables.Column()
    nazwa = tables.Column()
    cena = tables.Column()
    skladniki = tables.Column()


class OrderTable(tables.Table):
    id = tables.Column()
    imie = tables.Column()
    nazwisko = tables.Column()
    telefon = tables.Column()
    data = tables.Column()
    godzina = tables.Column()
    zamowione = tables.Column()
    cena = tables.Column()
