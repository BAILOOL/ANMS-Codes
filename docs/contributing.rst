Contributing
============
All development is done on GitHub: https://github.com/BAILOOL/ANMS-Codes


Git Workflow
============
- ``master`` branch is where all up-to-date stable code is maintained. At this moment it combines ``master`` and ``develop`` branches in one as there is no need for releases.
- ``feature/feature-name`` is a branch for any improvement, bugs, refactoring or documentation. This branch is the one used to push changes to ``master`` through Pull Requests (PR).

To create a pull request:
=======================

1. Fork the repository.
2. Clone it.
3. Install `Super-Linter <https://github.com/github/super-linter>`_:

.. code-block:: bash

    docker pull github/super-linter:latest

4. Make desired changes to the code.

5. Verify new changes pass Super-Linter:

.. code-block:: bash

    docker run -e RUN_LOCAL=true -e VALIDATE_CPP=false -e VALIDATE_JSCPD=false -v /path/to/local/codebase:/tmp/lint github/super-linter

6. Push code to your forked repo.

7. Create pull request.
