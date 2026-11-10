#!/usr/bin/env python3
"""Exercise patch installation against a pristine checkout of the real dependency."""

import contextlib
import io
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch

import prepare_atmosphere_libs as prepare


class LibraryPreparationTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="wgnx-libs-")
        self.addCleanup(self.temp.cleanup)
        self.libs = Path(self.temp.name) / "libs"
        subprocess.run(
            ["git", "clone", "--quiet", "--shared", "--no-checkout", str(prepare.LIBRARIES), str(self.libs)],
            check=True,
        )
        self.git("sparse-checkout", "set", "--no-cone", "/libstratosphere/include/stratosphere/sf/hipc/",
                 "/libstratosphere/source/sf/hipc/", "/libstratosphere/source/socket/impl/")
        self.git("checkout", "--quiet", "--detach", prepare.REVISION)

    def git(self, *args):
        return subprocess.run(
            ["git", "-C", str(self.libs), *args], check=True,
            capture_output=True, text=True, encoding="utf-8",
        ).stdout

    def prepare(self):
        with patch.object(prepare, "LIBRARIES", self.libs), \
                contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
            return prepare.main()

    def test_pristine_checkout_and_repeated_build(self):
        self.assertEqual(self.prepare(), 0)
        first_diff = self.git("diff", "--binary")
        self.assertTrue(first_diff)
        self.git("apply", "--reverse", "--check", str(prepare.PATCH))
        self.assertEqual(self.prepare(), 0)
        self.assertEqual(self.git("diff", "--binary"), first_diff)
        # Upstream owns this cleanup now; the project must not patch it again.
        self.assertNotIn("diff --git a/libstratosphere/source/sf/hipc/sf_hipc_server_session_manager.cpp", first_diff)

    def test_conflicting_edit_is_preserved_without_partial_application(self):
        header = self.libs / "libstratosphere/include/stratosphere/sf/hipc/sf_hipc_server_session_manager.hpp"
        source = header.read_text(encoding="utf-8")
        self.assertIn("ForwardServiceDeleter", source)
        header.write_text(source.replace("ForwardServiceDeleter", "LocalForwardServiceDeleter"), encoding="utf-8")
        before = self.git("diff", "--binary")
        self.assertEqual(self.prepare(), 1)
        self.assertEqual(self.git("diff", "--binary"), before)

    def test_unexpected_dependency_revision_is_rejected(self):
        self.git("-c", "user.name=Test", "-c", "user.email=test@example.invalid",
                 "commit", "--quiet", "--allow-empty", "-m", "Unsupported test revision")
        self.assertEqual(self.prepare(), 1)
        self.assertEqual(self.git("diff", "--binary"), "")


if __name__ == "__main__":
    unittest.main()
