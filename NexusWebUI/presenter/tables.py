import django_tables2 as tables


class OverviewTable(tables.Table):
    height = tables.Column(orderable=False)
    hash = tables.Column(orderable=False)
    block_reward = tables.Column(orderable=False)
    time = tables.Column(orderable=False)
    network_diff = tables.Column(orderable=False)
    payment_method = tables.Column(orderable=False)

    class Meta:
        template_name = "django_tables2/bootstrap4.html"
        attrs = {
            'class': ' table table-dark table-hover table-striped',
        }
