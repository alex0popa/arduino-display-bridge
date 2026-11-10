"""Verify MCP panel validation and its serialized command sequence."""

import unittest
from unittest.mock import MagicMock, patch

import server


class PanelTests(unittest.TestCase):
    def test_panel_commands_preserve_column_spacing_and_normalize_text(self):
        with patch.object(server, "_send_commands", return_value="ok") as send:
            self.assertEqual(server.write_panel(["città", " " * 16 + "second"]), "ok")
            send.assert_called_once_with(["B", "P|0|citta", "P|1|                second", "F"])

    def test_invalid_panel_does_not_send(self):
        with patch.object(server, "_send_commands") as send:
            for rows in (["x" * 49], ["x"] * 11, ["bad\nrow"]):
                with self.subTest(rows=rows), self.assertRaises(ValueError):
                    server.write_panel(rows)
            send.assert_not_called()

    def test_missing_confirmation_closes_connection_before_retry(self):
        failed = MagicMock(port="first")
        failed.readline.return_value = b""
        recovered = MagicMock(port="second")
        recovered.readline.return_value = b"OK\n"
        with patch.object(server, "_get_connection", side_effect=[failed, recovered]):
            with self.assertRaisesRegex(RuntimeError, "empty"):
                server.write_panel(["first"])
            failed.close.assert_called_once_with()
            self.assertEqual(server.write_panel(["second"]), "Command sent on port second.")
        self.assertEqual(recovered.write.call_args_list[0].args, (b"B\n",))


if __name__ == "__main__":
    unittest.main()
