class Declination:
    def __init__(self, mianownik, dopelniacz, biernik, dopelniacz_mnogi, biernik_mnogi):
        self.m = mianownik
        self.d = dopelniacz
        self.b = biernik
        self.d_mn = dopelniacz_mnogi
        self.b_mn = biernik_mnogi


clientDeclination = Declination(
    'klient', 'klienta', 'klienta', 'klientów', 'klientów'
)
ingredientDeclination = Declination(
    'sklładnik', 'składnika', 'składnik', 'składników', 'składniki'
)

orderDeclination = Declination(
    'zamówienie', 'zamówienia', 'zamówienie', 'zamówień', 'zamówienia'
)

sizeDeclination = Declination(
    'rozmiar', 'rozmiaru', 'rozmiar', 'rozmiarów', 'rozmiary'
)

pizzaBasisDeclination = Declination(
    'podstawa do pizzy', 'podstawy do pizzy', 'podstawę do pizzy', 'podstaw do pizzy', 'podstawy do pizzy'
)
