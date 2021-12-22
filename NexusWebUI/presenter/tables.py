import django_tables2 as tables
from django_tables2 import A


def shorten_hash_text(record):
    return record['hash'][:10] + '...' + record['hash'][-10:]


class OverviewTable(tables.Table):
    height = tables.Column(orderable=False)
    hash = tables.LinkColumn('presenter:detail', args=[A('hash')], orderable=False, text=shorten_hash_text,
                             verbose_name='Hash', attrs={"a": {"target": "_blank"}})
    block_reward = tables.Column(orderable=False)
    time = tables.Column(orderable=False)
    network_diff = tables.Column(orderable=False)

    class Meta:
        template_name = "django_tables2/bootstrap4.html"
        attrs = {
            'class': ' table table-dark table-hover table-striped',
        }
        sequence = ('height', 'block_reward', 'time', 'network_diff', 'hash')


class AccountPayoutsTable(tables.Table):
    time = tables.Column(orderable=False)
    amount = tables.Column(orderable=False)
    hash = tables.LinkColumn('presenter:detail', args=[A('hash')], orderable=False, text=shorten_hash_text,
                             verbose_name='Hash', attrs={"a": {"target": "_blank"}})

    class Meta:
        template_name = "django_tables2/bootstrap4.html"
        attrs = {
            'class': ' table table-dark table-hover table-striped',
        }
