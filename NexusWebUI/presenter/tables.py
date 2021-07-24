import django_tables2 as tables
from django_tables2 import A


class OverviewTable(tables.Table):
    height = tables.Column(orderable=False)
    hash = tables.LinkColumn('presenter:block_detail', args=[A('hash')], orderable=False)
    block_reward = tables.Column(orderable=False)
    time = tables.Column(orderable=False)
    network_diff = tables.Column(orderable=False)
    payment_method = tables.Column(orderable=False)

    class Meta:
        template_name = "django_tables2/bootstrap4.html"
        attrs = {
            'class': ' table table-dark table-hover table-striped',
        }


class AccountWorksTable(tables.Table):
    id = tables.Column(orderable=False)
    status = tables.Column(orderable=False)
    hslast10 = tables.Column(orderable=False)
    hslast1d = tables.Column(orderable=False)
    lastshare = tables.Column(orderable=False)
    rejectratio = tables.Column(orderable=False)

    class Meta:
        template_name = "django_tables2/bootstrap4.html"
        attrs = {
            'class': ' table table-dark table-hover table-striped',
        }


class AccountPayoutsTable(tables.Table):
    time = tables.Column(orderable=False)
    amount = tables.Column(orderable=False)
    state = tables.Column(orderable=False)
    txhash = tables.Column(orderable=False)

    class Meta:
        template_name = "django_tables2/bootstrap4.html"
        attrs = {
            'class': ' table table-dark table-hover table-striped',
        }

