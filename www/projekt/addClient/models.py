from django.db.models import (Model, BooleanField, CharField,
                              DateField, TimeField, IntegerField,
                              EmailField, ForeignKey, DecimalField,
                              CASCADE, ManyToManyField, Manager)

from .tables import *
from django.db import connection

# Create your models here.


class Klient(Model):
    # id auto-increment added by default
    imie = CharField(max_length=20)
    nazwisko = CharField(max_length=50)
    email = EmailField()
    ulica = CharField(max_length=60)
    miasto = CharField(max_length=60)
    telefon = IntegerField(unique=True)

    def __str__(self):
        return '{} {} (Tel: {})'.format(self.imie, self.nazwisko, str(self.telefon))


class Zamowienie(Model):
    klient = ForeignKey(Klient, null=True, on_delete=CASCADE)
    data = DateField(auto_now=True)
    godzina = TimeField(auto_now=True)

    def __str__(self):
        return '{} {} {} {}'.format(self.id, self.klient, self.data, self.godzina)


class Skladnik(Model):
    nazwa = CharField(max_length=30, unique=True)
    cena_domyslna = DecimalField(max_digits=6, decimal_places=2)

    def __str__(self):
        return "{} (cena: {})".format(self.nazwa, str(self.cena_domyslna))


def query(sql_statement):
    with connection.cursor() as cursor:
        cursor.execute(sql_statement)

        results = cursor.fetchall()

        x = cursor.description
        resultsList = []
        for r in results:
            i = 0
            d = {}
            while i < len(x):
                d[x[i][0]] = r[i]
                i = i+1
            resultsList.append(d)
        # print(resultsList)
        return resultsList


class PodstawaDoPizzyManager(Manager):
    def with_ingredients(self):

        # pdp = PodstawaDoPizzy._meta.db_table
        # sp = SkladnikiPodstawy._meta.db_table
        # sk = Skladnik._meta.db_table

        sql = '''
SELECT pdp.id, pdp.nazwa, pdp.cena, group_concat(sk.nazwa) AS 'skladniki'
    FROM addClient_podstawadopizzy pdp
    LEFT JOIN addClient_skladnikipodstawy sp ON pdp.id = sp.podstawa_id
    LEFT JOIN addClient_skladnik sk ON sp.skladnik_id = sk.id
    GROUP BY pdp.nazwa;'''

        return PizzaTable(query(sql))


class PodstawaDoPizzy(Model):
    nazwa = CharField(max_length=30, unique=True)
    cena = DecimalField(max_digits=6, decimal_places=2)

    def __str__(self):
        return "{} ({} zł)".format(self.nazwa, str(self.cena))

    objects = PodstawaDoPizzyManager()


class Rozmiar(Model):
    nazwa = CharField(max_length=20)
    srednica = IntegerField()
    mnoznik_ceny = DecimalField(max_digits=6, decimal_places=2)

    def __str__(self):
        return "{} (r={}) (x{})".format(self.nazwa, str(self.srednica), str(self.mnoznik_ceny))


class ZamowionaPizzaManager(Manager):
    def with_pizzas(self):

        sql = '''
SELECT z.id, kl.imie, kl.nazwisko, kl.telefon, z.data, z.godzina,
    GROUP_CONCAT(z.pizza || ' za ' || z.cena || ' zl', ',') as zamowione,  
    PRINTF("%.2f", SUM(z.cena)) || ' zl' as cena
    FROM (
        SELECT zam.id as id,
            zam.data as data,
            zam.godzina as godzina, 
            zam.klient_id as klid,
            roz.nazwa || " " || pdp.nazwa || IFNULL("(+" || GROUP_CONCAT(sk.nazwa, ',') || ")", '') as pizza, 
            PRINTF("%.2f", (pdp.cena + IFNULL(SUM(cena_domyslna),0)) * roz.mnoznik_ceny) as cena
        FROM addClient_zamowienie zam
        LEFT JOIN addClient_zamowionapizza zp ON zp.zamowienie_id = zam.id
        LEFT JOIN addClient_podstawadopizzy pdp ON zp.podstawa_id = pdp.id
        LEFT JOIN addClient_rozmiar roz ON zp.rozmiar_id = roz.id
        LEFT JOIN addClient_dodatkoweskladniki ds ON ds.zamowiona_id = zp.id
        LEFT JOIN addClient_skladnik sk ON ds.skladnik_id = sk.id
        GROUP BY zp.id) z
    LEFT JOIN addClient_klient kl ON z.klid = kl.id
    GROUP BY z.id;'''
        return OrderTable(query(sql))


class ZamowionaPizza(Model):
    podstawa = ForeignKey(
        PodstawaDoPizzy, on_delete=CASCADE)
    rozmiar = ForeignKey(
        Rozmiar, related_name='rozmiar', on_delete=CASCADE)
    zamowienie = ForeignKey(Zamowienie, on_delete=CASCADE)
    objects = ZamowionaPizzaManager()


class DodatkoweSkladniki(Model):
    zamowiona = ForeignKey(ZamowionaPizza, on_delete=CASCADE)
    skladnik = ForeignKey(Skladnik, on_delete=CASCADE)


class SkladnikiPodstawy(Model):
    podstawa = ForeignKey(
        PodstawaDoPizzy, on_delete=CASCADE, related_name='podstawy')
    skladnik = ForeignKey(Skladnik, on_delete=CASCADE,
                          related_name='skladniki')
