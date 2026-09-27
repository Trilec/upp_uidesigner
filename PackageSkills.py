"""Refresh portable engineering references and create deterministic skill bundles."""
import argparse
import shutil
import subprocess
import sys
from pathlib import Path
import zipfile

ROOT = Path(__file__).resolve().parent


def package(ui_root, output):
    subprocess.run([sys.executable, str(ui_root / 'scripts/PackageSkills.py')], check=True)
    # Ui owns the native development skill; Designer distributes an identical copy.
    canonical = ui_root / 'skills/upp-ui-development'
    if not (canonical / 'SKILL.md').is_file():
        raise FileNotFoundError('Missing canonical Ui development skill: ' + str(canonical))
    shutil.copytree(canonical, ROOT / 'skills/upp-ui-development', dirs_exist_ok=True)
    output.mkdir(parents=True, exist_ok=True)
    for name in ('uidesigner-design', 'upp-ui-development'):
        source = ROOT / 'skills' / name
        files = sorted(p for p in source.rglob('*') if p.is_file()
                       and '__pycache__' not in p.parts and p.suffix not in ('.pyc', '.zip'))
        target = output / (name + '.zip')
        with zipfile.ZipFile(target, 'w', zipfile.ZIP_DEFLATED) as archive:
            for path in files:
                info = zipfile.ZipInfo(path.relative_to(source.parent).as_posix())
                info.compress_type = zipfile.ZIP_DEFLATED
                archive.writestr(info, path.read_bytes())
        sections = []
        for path in sorted(files, key=lambda p: (p.name != 'SKILL.md', str(p))):
            if path.suffix not in ('.md', '.json'):
                continue
            sections.append('## File: ' + path.relative_to(source).as_posix() + '\n\n'
                            + path.read_text(encoding='utf-8') + '\n')
        (output / (name + '-chat.md')).write_text('\n'.join(sections), encoding='utf-8')
        print(f'{target}: {len(files)} files')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--ui-root', type=Path, default=ROOT.parent / 'upp_Ui')
    parser.add_argument('--output', type=Path, default=ROOT / 'skills',
                        help='Directory for uploadable ZIPs and chat references (default: skills/)')
    args = parser.parse_args()
    package(args.ui_root, args.output)
