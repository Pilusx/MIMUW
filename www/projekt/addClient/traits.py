from .declination import Declination
from .forms import DelOneFormFunctor
from .models import Klient

from django.conf.urls import url
from django.db.models import Model
from django.shortcuts import render
from django.contrib import messages
from django.views import View
from django.http import HttpResponseRedirect, HttpResponse
from django.forms import modelform_factory, Select
import django_filters

# Texts
worker_header = ', Pracowniku'
client_header = ', Kliencie'


class Trait:
    def __init__(self, view, header, clsform, table):
        self.view = view
        self.header = header
        self.clsform = clsform
        self.table = table

    def merge(self, prefix, name):
        return prefix + name + '.'

    def to_feature(self, name):
        return (self.urlprefix + name, self.button_text, self.style)

    def to_url(self, traits):
        return url(self.urlprefix + traits.name, self.view.as_view(), kwargs={'traits': traits, 'trait': self})


class Traits:
    def __init__(self, name, model, traits=[]):
        self.name = name
        self.model = model
        self.traits = traits

        self.parse()

    def parse(self):
        self.features = []
        self.urls = []

        for trait in self.traits:
            self.features.append(trait.to_feature(self.name))
            self.urls.append(trait.to_url(self))


class GetView(View):
    def rend(self, request, form, trait: Trait, traits: Traits):
        return render(request, 'form.html', {'header': trait.header,
                                             'help': trait.helper_text,
                                             'form': form})

    def get(self, request, trait: Trait, traits: Traits):
        return self.rend(request, trait.clsform(), trait, traits)


class GetPostView(GetView):
    def post(self, request, trait: Trait, traits: Traits):
        form = trait.clsform(request.POST)

        if form.is_valid():
            form.save()
            messages.add_message(
                request, messages.INFO, trait.successMessage)
            return HttpResponseRedirect('')
        return self.rend(request, form, trait, traits)


class ClientFilter(django_filters.FilterSet):
    class Meta:
        model = Klient
        fields = ['telefon']


class ShowTableView(GetView):
    def get(self, request, trait: Trait, traits: Traits):
        if(traits.model != Klient):
            return render(request, 'table.html', {'header': trait.header,
                                                  'help': trait.helper_text,
                                                  'table': trait.table})

        else:
            f = ClientFilter(
                request.GET, queryset=Klient.objects.all())
            return render(request, 'table.html', {'header': trait.header,
                                                  'help': trait.helper_text,
                                                  'filter': f})


class ClearTableView(View):
    confirmationMessage = 'Czy jesteś pewny, że chcesz wyczyścić tę tabelę?'
    successMessage = 'Wyczyszczono tę tabelę.'

    def get(self, request, trait: Trait, traits: Traits):
        return render(request, 'form.html', {'header': trait.header,
                                             'help': self.confirmationMessage})

    def post(self, request, trait: Trait, traits: Traits):
        traits.model.objects.all().delete()
        return render(request, 'message.html', {'header': trait.header,
                                                'help': self.successMessage})


class AddSimpleTrait(Trait):
    urlprefix = 'add'
    buttonprefix = 'Dodać '
    helperprefix = 'Wprowadź dane '
    successMessage = 'Obiekt został dodany.'
    style = 'add_style'

    def __init__(self, header, decl: Declination, table, clsform=None):
        super().__init__(GetPostView, header, clsform, table)
        self.button_text = self.merge(self.buttonprefix, decl.b)
        self.helper_text = self.merge(self.helperprefix, decl.d)


class ShowTrait(Trait):
    urlprefix = 'show'
    buttonprefix = 'Wyświetlić tabelę '
    helperprefix = 'Oto tabela '
    style = 'show_style'

    def __init__(self, header, decl: Declination, table, clsform=None):
        super().__init__(ShowTableView, header, clsform, table)
        self.button_text = self.merge(self.buttonprefix, decl.d_mn)
        self.helper_text = self.merge(self.helperprefix, decl.d_mn)


class ClearTrait(Trait):
    urlprefix = 'clear'
    buttonprefix = 'Usunąć wszystki(e/ch) '
    helperprefix = 'Tabela '
    helpersuffix = ' została wyczyszczona.'
    style = 'clear_style'

    def __init__(self, header, decl: Declination, table, clsform=None):
        super().__init__(ClearTableView, header, clsform, table)
        self.button_text = self.merge(self.buttonprefix, decl.b_mn)
        self.helper_text = self.helperprefix + decl.d_mn + self.helpersuffix


class DeleteOneTrait(Trait):
    urlprefix = 'delOne'
    buttonprefix = 'Usunąć jed(en/ną/no) '
    helperprefix = 'Któr(ą/e/go) '
    helpersuffix = ' chcesz usunąć?'
    style = 'clear_style'
    successMessage = 'Obiekt został usunięty.'

    def __init__(self, header, decl: Declination, table, clsform=None):
        super().__init__(GetPostView, header, clsform, table)
        self.button_text = self.merge(self.buttonprefix, decl.b)
        self.helper_text = self.helperprefix + decl.b + self.helpersuffix


class ASCTraits(Traits):
    def __init__(self, name, model: Model, decl: Declination, form=False, hidden_clear=False, show_table=False, hidden_delone=True):
        default_table = model.objects.all()
        show_table = default_table if not show_table else show_table

        if not form:
            clsform = modelform_factory(model, fields='__all__')
        else:
            clsform = form

        wh = worker_header
        trait_list = [AddSimpleTrait(wh, decl, default_table, clsform),
                      ShowTrait(wh, decl, show_table, clsform)]

        if not hidden_clear:
            trait_list.append(ClearTrait(
                wh, decl, default_table, clsform))
        if not hidden_delone:
            delform = DelOneFormFunctor(decl.m, model)
            trait_list.append(DeleteOneTrait(wh, decl, default_table, delform))

        super().__init__(name, model, traits=trait_list)
