import logging
from django import forms
from django.conf import settings
from django.shortcuts import redirect

# from .models import ValidAccounts
from .rpc_requests import get_account_header, socket_connect, get_account

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

        logger.info("Sending Request for Wallet ID verficiation to Backend")

        socket = socket_connect(_ip=getattr(settings, "POOL_IP", None),
                                _port=getattr(settings, "POOL_PORT", None))
        # get_account_json = get_account(_socket=socket, _account=wallet_id)
        get_account_header_json = get_account_header(_socket=socket, _account=wallet_id)

        print("get_account_header_json: ", get_account_header_json)

        if 'error' in get_account_header_json:
            logger.info(f"Error when validating Wallet ID: {get_account_header_json}")
            if get_account_header_json['error']['code'] == -10:
                logger.info("Invalid Wallet ID")
                raise forms.ValidationError(-10)
            if get_account_header_json['error']['code'] == -11:
                logger.info("Unknown Wallet ID")
                raise forms.ValidationError(-11)


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
        self.fields['hardware'].widget.attrs['style'] = 'width: 340px'




