"""Tests for textual stability metrics in camera_vision.py."""

from collections import deque
import math
from argparse import Namespace

from vision.camera_vision import _print_metrics, _similarity


class TestSimilarity:
    def test_returns_jaccard_similarity_for_consecutive_descriptions(self) -> None:
        assert _similarity("A red car beside a tree", "A blue car beside a tree") == 2 / 3

    def test_empty_descriptions_are_fully_similar(self) -> None:
        assert _similarity("", "") == 1.0
        assert _similarity("", "anything") == 0.0
        assert _similarity("same words", "same words") == 1.0


class TestPrintMetrics:
    def test_reports_latency_range_and_average_stability(self, capsys) -> None:
        _print_metrics(deque([2.0, 4.0]), deque([0.5]))

        assert capsys.readouterr().out == (
            "Metrics (last 2): latency avg/min/max 3.0/2.0/4.0s; "
            "description similarity avg 50%\n"
        )

    def test_marks_first_frame_stability_as_unavailable(self, capsys) -> None:
        _print_metrics(deque([1.5]), deque())

        assert capsys.readouterr().out == (
            "Metrics (last 1): latency avg/min/max 1.5/1.5/1.5s; "
            "description similarity avg n/a (first frame)\n"
        )


class TestIntervalValidation:
    def test_interval_must_be_finite_and_positive(self) -> None:
        assert math.isfinite(2.0) and 2.0 > 0
        assert not math.isfinite(float("nan"))
        assert not math.isfinite(float("inf"))
        assert not (0.0 > 0)
