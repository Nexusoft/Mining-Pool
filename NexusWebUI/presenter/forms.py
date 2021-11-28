import logging
from django import forms
from django.conf import settings
from django.shortcuts import redirect

# from .models import ValidAccounts
from .rpc_requests import get_account_header, socket_connect, get_account

logger = logging.getLogger('NexusWebUI')


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
        get_account_json = get_account(_socket=socket, _account=wallet_id)

        print("get_account_json: ", get_account_json)

        if 'error' in get_account_json:
            logger.info(f"Error when validating Wallet ID: {get_account_json}")
            if get_account_json['error']['code'] == -10:
                logger.info("Invalid Wallet ID")
                raise forms.ValidationError(-10)
            if get_account_json['error']['code'] == -11:
                logger.info("Unknown Wallet ID")
                raise forms.ValidationError(-11)

