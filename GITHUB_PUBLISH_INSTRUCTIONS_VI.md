# GitHub publish — Lakakey (Larx Vo)

The connected GitHub account is `larxvo`. The repository `larxvo/lakakey` does not yet exist, and the available connector cannot create new repositories. This means this package has NOT been uploaded to GitHub.

To publish:
1. In GitHub, create a new empty repository named `lakakey` under user `larxvo` (public or private, your choice).
2. Upload the **contents of the `source` directory** from this ZIP to that repository, including hidden `.github` folder.
3. GitHub > Actions > `Build Lakakey Go Windows preview EXE (not IME)` > Run workflow. Download artifact `LakakeyPreview-Windows-x64`.
4. Do NOT publish as a stable UniKey replacement until the Windows TSF IME works and passes actual Windows 10/11 compatibility tests.

When repo has been created, the connected GitHub app can be used to commit source files on your behalf. Binary GitHub Release asset upload is not directly exposed by the current connector.
