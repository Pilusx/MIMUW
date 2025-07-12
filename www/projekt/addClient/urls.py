# Add URL maps to redirect the base URL to our application
from django.conf.urls import include, url
from django.http import HttpResponseRedirect
from . import views


urlpatterns = views.urls + [
    url(r'', views.index)
]
