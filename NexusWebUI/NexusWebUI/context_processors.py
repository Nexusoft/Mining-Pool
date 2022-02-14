from django.conf import settings


def dynamic_title(request):
    return {'SITE_NAME': settings.SITE_NAME}
