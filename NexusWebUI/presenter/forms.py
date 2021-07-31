from django import forms


class WalletSearchForm(forms.Form):
    wallet_id = forms.CharField(max_length=100)

    def clean(self):
        cleaned_data = super(WalletSearchForm, self).clean()
        if cleaned_data['wallet_id'] != '1234':
            # Todo run get_account rpc here
            raise forms.ValidationError(u'The Wallet ID you entered is not valid, please try again')
