
from django.utils import html
from django.shortcuts import render


from .models import *
from .forms import *
from .declination import *
from .traits import *

# TODO na pozniej
# dodać dodawanie wielu pizz naraz
# dodać wyświetlanie zamówień konkretnego klienta

# Uprościć interfejs guzikowy
# Powywalac polskie nazwy z modeli... i nie tylko, a teksty i deklinacje do osobnego pliku

# TODO na najpozniej
# zoptymalizowac Traits np. po co delform dostaje table.. ale przynajmniej działa XD


traitss = [
    ASCTraits('Client', Klient, clientDeclination,
              hidden_clear=True),
    ASCTraits('Ingredient', Skladnik,
              ingredientDeclination, hidden_clear=True),
    ASCTraits('Size', Rozmiar, sizeDeclination, hidden_clear=True),
    ASCTraits('PizzaBasis', PodstawaDoPizzy, pizzaBasisDeclination,
              PodstawaDoPizzyAddForm, hidden_clear=True,
              show_table=PodstawaDoPizzy.objects.with_ingredients, hidden_delone=False),
    ASCTraits('Order', Zamowienie, orderDeclination,
              ZamowienieAddForm, hidden_clear=False,
              show_table=ZamowionaPizza.objects.with_pizzas, hidden_delone=False)
]

features = [feature for traits in traitss for feature in traits.features]
urls = [url for traits in traitss for url in traits.urls]


# Create your views here.


def index(request):
    header = ""
    helper = "Witam! Słyszałem, że chcesz coś zrobić."

    parsed_features = [{'link': x[0],
                        'name': x[1],
                        'link_style': x[2]} for x in features]

    return render(request, 'menu.html', {'header': header,
                                         'help': helper,
                                         'features': parsed_features})
