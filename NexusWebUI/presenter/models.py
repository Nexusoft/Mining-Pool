from django.db import models


# class ValidAccounts(models.Model):
#     account = models.CharField(max_length=255, primary_key=True, verbose_name='Account')

class MiningCalcHardware(models.Model):
    model_name = models.CharField(max_length=100, primary_key=True, verbose_name="Model Name")
    hashrate = models.DecimalField(decimal_places=4, max_digits=20)
    power_consumption = models.DecimalField(decimal_places=4, max_digits=20)

    def __str__(self):
        return self.model_name
