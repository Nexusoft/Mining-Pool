from django.urls import path

from django.conf import settings
from django.conf.urls.static import static
from django.contrib.staticfiles.urls import staticfiles_urlpatterns
from . import views

app_name = 'presenter'

urlpatterns = [
                  path('', views.IndexView.as_view(), name='index'),
                  path('overview/', views.block_overview_list, name='overview'),
                  path('block_detail/<str:block_id>', views.block_detail, name='block_detail'),
                  path('wallet/', views.wallet_detail, name='wallet'),
              ] + static(settings.MEDIA_URL, document_root=settings.MEDIA_ROOT)

if settings.DEBUG:
    urlpatterns += staticfiles_urlpatterns()
