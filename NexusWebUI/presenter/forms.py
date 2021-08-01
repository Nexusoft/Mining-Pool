from django import forms
from .rpc_requests import get_account, socket_connect


class WalletSearchForm(forms.Form):
    wallet_id = forms.CharField(max_length=100)

    def clean(self):
        cleaned_data = super(WalletSearchForm, self).clean()

        socket = socket_connect(_ip='127.0.0.1', _port=5000)
        get_account_json = get_account(_socket=socket, _account=cleaned_data['wallet_id'])

        if 'error' in get_account_json:
            print("Error when validating Wallet ID:")
            print(get_account_json)
            if get_account_json['error']['code'] == -10:
                raise forms.ValidationError(f'The Wallet ID you entered is not valid!')
            if get_account_json['error']['code'] == -11:
                raise forms.ValidationError(f'The Wallet ID you entered does not exist')
        else:
            print("Wallet ID valid")
            print((get_account_json))
