from functools import wraps

from ucm.shared.metrics import ucmmetrics


def test_wrap(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        print(f"========>> Running in {func.__name__}:")
        result = func(*args, **kwargs)
        print()
        return result

    return wrapper


@test_wrap
def metrics_with_update_stats():
    ucmmetrics.set_up(100)
    ucmmetrics.create_stats("counter_1", "counter")
    ucmmetrics.update_stats("counter_1", 1.2)

    ucmmetrics.create_stats("gauge_1", "gauge")
    ucmmetrics.update_stats("gauge_1", 2.2)

    ucmmetrics.create_stats("histogram_1", "histogram")
    ucmmetrics.update_stats("histogram_1", 1)

    counters, gauges, histograms = ucmmetrics.get_all_stats_and_clear()
    print(
        f"After clear then get counters: {counters}, gauges: {gauges}, histograms: {histograms}"
    )
    assert counters["counter_1"] == 1.2
    assert gauges["gauge_1"] == 2.2
    assert histograms["histogram_1"][-1] == 1.0

    ucmmetrics.update_stats(
        {
            "counter_1": 5,
            "gauge_1": 6,
            "histogram_1": 7,
        }
    )

    ucmmetrics.update_stats(
        {
            "counter_1": 5,
            "gauge_1": 6.6,
            "histogram_1": 8,
        }
    )
    counters, gauges, histograms = ucmmetrics.get_all_stats_and_clear()
    assert counters["counter_1"] == 10.0
    assert gauges["gauge_1"] == 6.6
    assert len(histograms["histogram_1"]) == 2
    print(
        f"After clear then get counters: {counters}, gauges: {gauges}, histograms: {histograms}"
    )


if __name__ == "__main__":
    metrics_with_update_stats()
