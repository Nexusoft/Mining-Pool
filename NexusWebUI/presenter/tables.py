import django_tables2 as tables
from django_tables2 import A


def shorten_hash_text(record):
    return record['hash'][:10] + '...' + record['hash'][-10:]


class OverviewTable(tables.Table):
    height = tables.Column(orderable=False)
    hash = tables.LinkColumn('presenter:detail', args=[A('hash')], orderable=False, text=shorten_hash_text,
                             verbose_name='Hash', attrs={"a": {"target": "_blank"}})
    time = tables.Column(orderable=False)
    network_diff = tables.Column(orderable=False)
    is_orphan = tables.Column(orderable=False)

    class Meta:
        template_name = "django_tables2/bootstrap4.html"
        attrs = {
            'class': ' table table-dark table-hover table-striped',
        }
        sequence = ('height', 'time', 'network_diff', 'hash', 'is_orphan')
        row_attrs = {
            'is_orphan': lambda record: record['is_orphan']
        }


class AccountPayoutsTable(tables.Table):
    time = tables.Column(orderable=False)
    amount = tables.Column(orderable=False)
    shares = tables.Column(orderable=False)
    hash = tables.LinkColumn('presenter:detail', args=[A('hash')], orderable=False, text=shorten_hash_text,
                             verbose_name='Hash', attrs={"a": {"target": "_blank"}})

    class Meta:
        template_name = "django_tables2/bootstrap4.html"
        attrs = {
            'class': ' table table-dark table-hover table-striped',
        }


class Top5FindersTable(tables.Table):
    display_name = tables.Column(orderable=False)
    num_blocks = tables.Column(orderable=False)

    class Meta:
        template_name = "django_tables2/bootstrap4.html"
        attrs = {
            'class': ' table table-dark table-hover table-striped',
        }


class LongestChainTable(tables.Table):
    display_name = tables.Column(orderable=False)
    chain = tables.Column(orderable=False)

    class Meta:
        template_name = "django_tables2/bootstrap4.html"
        attrs = {
            'class': ' table table-dark table-hover table-striped',
        }
