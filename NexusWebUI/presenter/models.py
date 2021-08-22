from django.db import models


class ValidAccounts(models.Model):
    account = models.CharField(max_length=255, primary_key=True, verbose_name='Account')
