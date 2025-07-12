from django import forms
from django.forms import ModelForm, Form, ModelChoiceField
from .models import *
from django.db import models
import itertools


class ZamowienieForm(ModelForm):
    class Meta:
        model = Zamowienie
        fields = ['klient']


class PodstawaDoPizzyAddForm(ModelForm):
    class Meta:
        model = PodstawaDoPizzy
        fields = ['nazwa', 'cena']

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.fields['skladniki'] = forms.ModelMultipleChoiceField(
            queryset=Skladnik.objects.all(), widget=forms.CheckboxSelectMultiple(), required=False)

    def save(self, commit=True):
        instance = super().save(commit=False)
        # print(self.fields['skladniki'])

        if commit:
            instance = super().save()
            chosen = list(self.cleaned_data['skladniki'])
            for ingredient in chosen:
                print(instance.pk, ingredient.pk, ingredient)
                SkladnikiPodstawy(podstawa=instance,
                                  skladnik=ingredient).save()
        return instance


class ZamowienieAddForm(Form):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.fields['klient'] = forms.ModelChoiceField(
            queryset=Klient.objects.all())

        self.fields['rozmiar'] = forms.ModelChoiceField(
            queryset=Rozmiar.objects.all(), required=True
        )
        self.fields['podstawa'] = forms.ModelChoiceField(
            queryset=PodstawaDoPizzy.objects.all(), required=True
        )

        self.fields['skladniki'] = forms.ModelMultipleChoiceField(
            queryset=Skladnik.objects.all(), widget=forms.CheckboxSelectMultiple(), required=False)

    def save(self, commit=True):
        if commit:
            klient = self.cleaned_data['klient']
            rozmiar = self.cleaned_data['rozmiar']
            podstawa = self.cleaned_data['podstawa']
            zamowienie = Zamowienie(klient=klient)
            zamowienie.save()
            zamowionapizza = ZamowionaPizza(podstawa=podstawa,
                                            rozmiar=rozmiar,
                                            zamowienie=zamowienie)

            zamowionapizza.save()
            chosen = list(self.cleaned_data['skladniki'])
            for ingredient in chosen:
                DodatkoweSkladniki(zamowiona=zamowionapizza,
                                   skladnik=ingredient).save()


class DelOneForm(Form):
    def __init__(self, name, model, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.model = model
        self.name = name
        self.fields[name] = forms.ModelChoiceField(
            queryset=model.objects.all())

    def save(self, commit=True):
        if commit:
            object = self.cleaned_data[self.name]
            instance = self.model.objects.get(id=object.id)
            instance.delete()


class DelOneFormFunctor(object):
    def __init__(self, name, model):
        self.model = model
        self.name = name

    def __call__(self, *args, **kwargs):
        return DelOneForm(self.name, self.model, *args, **kwargs)
