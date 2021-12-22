from django.urls import path

from django.conf import settings
from django.conf.urls.static import static
from django.contrib.staticfiles.urls import staticfiles_urlpatterns
from . import views

app_name = 'presenter'

urlpatterns = [
                  path('', views.block_overview_list, name='index'),
                  path('overview/', views.block_overview_list, name='overview'),
                  path('error/', views.ErrorView.as_view(), name='error'),
                  path('error_pool/', views.ErrorPoolView.as_view(), name='error_pool'),
                  path('error_invalid_wallet/', views.ErrorInvalidWalletIdView.as_view(), name='error_invalid_wallet'),
                  path('error_unknown_wallet/', views.ErrorUnknownWallet.as_view(), name='error_unknown_wallet'),
                  path('wallet/', views.wallet_detail, name='wallet'),
                  path('detail/<str:hash>/', views.block_detail, name='detail'),
                  path('mining_calc/', views.mining_calc, name='mining_calc'),
              ] + static(settings.MEDIA_URL, document_root=settings.MEDIA_ROOT)

if settings.DEBUG:
    urlpatterns += staticfiles_urlpatterns()
