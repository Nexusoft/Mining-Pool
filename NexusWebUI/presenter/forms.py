import logging
from django import forms
from django.conf import settings
from django.shortcuts import redirect
from .rest_request import rest_request

# from .models import ValidAccounts
# from .rpc_requests import get_account_header, socket_connect, get_account

logger = logging.getLogger('NexusWebUI')

HARDWARE = (
    ('', '...Choose...'),
    (33, 'GTX 1070'),
    (50, 'GTX 1080 TI'),
    (67, 'RTX 2060'),
    (135, 'RTX 3090'),
)


class WalletSearchForm(forms.Form):
    """
    Checks if a given Wallet ID is already in the local DB (validated), exists (insert) or return one of two Errors
    """
    wallet_id = forms.CharField(max_length=100)

    def clean(self):
        cleaned_data = super(WalletSearchForm, self).clean()

        if not cleaned_data:
            logger.info("Invalid Wallet ID")
            raise forms.ValidationError(-10)

        wallet_id = cleaned_data['wallet_id']

        logger.info("Sending Request for Wallet ID verification to Backend")
        logger.info(f"Wallet ID: {wallet_id}")

        print("Fetchting account/header")
        account_header = rest_request(method="account/header", parameters={'account': wallet_id})
        print(f"account_header: {account_header}")

        if account_header is None:
            logger.error(f"Connectivity Issue when checking Wallet ID")
            raise forms.ValidationError('pool_error')
        else:
            status_code = account_header.status_code

            if status_code != 200:
                logger.info(f"Invalid Account Request, Status: {status_code}")
                print(f"Invalid Account Request, Status: {status_code}")
                if status_code == 400:
                    logger.info("Invalid Wallet ID")
                    print("Invalid Wallet ID")
                    raise forms.ValidationError("invalid_account")
                elif status_code == 404:
                    logger.info("Unknown Wallet ID")
                    print("Unknown Wallet ID")
                    raise forms.ValidationError("unknown_account")
                else:
                    logger.warning(f"Received unknown Status Code: {status_code}")
                    print(f"Received unknown Status Code: {status_code}")
                    raise forms.ValidationError("unknown_error")


class CalcForm(forms.Form):
    hardware = forms.ChoiceField(choices=HARDWARE, label='Hardware')
    network_difficulty = forms.IntegerField(label='Network Difficulty', help_text="Usually around 8")
    block_reward = forms.FloatField(label="Block Reward (NXS)")
    exchange_rate = forms.FloatField(label="Exchange Rate (USD/NXS)")
    power_consumption = forms.FloatField(label="Power Consumption (W)")
    electricity_cost = forms.FloatField(label="Electricity Cost ($/KWH)")

    def __init__(self, *args, **kwargs):
        super(CalcForm, self).__init__(*args, **kwargs)
        for visible in self.visible_fields():
            visible.field.widget.attrs['class'] = 'form-control'

        self.fields['hardware'].widget.attrs['class'] = 'btn btn-light dropdown-toggle'
        self.fields['hardware'].widget.attrs['style'] = 'width: 355px'
