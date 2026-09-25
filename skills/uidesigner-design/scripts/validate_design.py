"""Portable preflight; canonical Designer validation remains authoritative."""
import json
import re
import sys
from pathlib import Path


def validate(document, schemas):
    errors = []
    def check(ok, message):
        if not ok:
            errors.append(message)
    check(document.get('format') == 'upp-ui-designer-next', 'Unsupported document format')
    check(document.get('schema') == 4, 'Expected schema 4')
    nodes = document.get('nodes', [])
    check(isinstance(nodes, list) and bool(nodes), 'Expected nonempty nodes array')
    if not isinstance(nodes, list) or not nodes:
        return errors
    if not all(isinstance(n, dict) for n in nodes):
        return errors + ['Every node must be an object']
    ids = [n.get('id') for n in nodes]
    if not all(type(i) is int and i > 0 for i in ids):
        return errors + ['IDs must be positive integers']
    check(len(set(ids)) == len(ids), 'Duplicate node IDs')
    by_id = {n['id']: n for n in nodes}
    check(nodes[0].get('type') == 'Window' and nodes[0].get('parent') == 0, 'First node must be Window root')
    names = set()
    for node in nodes:
        prefix = f"Node {node['id']}: "
        name = node.get('name', '')
        check(bool(re.fullmatch(r'[A-Za-z_][A-Za-z_0-9]*', name)), prefix + 'invalid name')
        check(name not in names, prefix + 'duplicate name')
        names.add(name)
        children = node.get('children', [])
        if not isinstance(children, list) or not all(type(c) is int for c in children):
            errors.append(prefix + 'children must be integer IDs')
            continue
        check(len(set(children)) == len(children), prefix + 'duplicate child')
        for child in children:
            check(child in by_id and by_id[child].get('parent') == node['id'], prefix + 'child/parent mismatch')
        if node is not nodes[0]:
            parent = by_id.get(node.get('parent'))
            check(parent is not None and node['id'] in parent.get('children', []), prefix + 'missing reciprocal parent')
        seen = set()
        current = node
        while current and current.get('parent'):
            if current['id'] in seen:
                errors.append(prefix + 'cycle')
                break
            seen.add(current['id'])
            current = by_id.get(current.get('parent'))
        if node is nodes[0]:
            continue
        path = schemas / (node.get('type', '') + '.json')
        if not path.is_file():
            errors.append(prefix + 'unknown control type')
            continue
        spec = json.loads(path.read_text(encoding='utf-8-sig'))
        fields = {f['id']: f for f in spec['properties']}
        props = node.get('properties', {})
        if not isinstance(props, dict):
            errors.append(prefix + 'properties must be an object')
            continue
        for key, value in props.items():
            if key in ('grid_row', 'grid_column'):
                parent = by_id.get(node.get('parent'), {})
                check(parent.get('type') == 'UiGridLayout', prefix + 'grid placement requires Grid parent')
                check(type(value) is int and value >= 0, prefix + 'invalid grid placement')
                continue
            field = fields.get(key)
            if field is None:
                errors.append(prefix + 'unknown property ' + key)
                continue
            choices = [c['value'] for c in field.get('choices', [])]
            check(not choices or value in choices, prefix + 'invalid choice for ' + key)
            kind = field.get('kind')
            if value is not None:
                if kind == 'Boolean':
                    check(type(value) is bool, prefix + key + ' must be boolean')
                if kind in ('Integer', 'Int'):
                    check(type(value) is int, prefix + key + ' must be integer')
                if isinstance(value, (int, float)) and not isinstance(value, bool):
                    for bound, compare in [('minimum', lambda a, b: a >= b), ('maximum', lambda a, b: a <= b)]:
                        if field.get(bound) is not None:
                            check(compare(value, field[bound]), prefix + key + ' outside ' + bound)
        theme_fields = {f['id'] for f in spec.get('theme_fields', [])}
        for key in node.get('theme_overrides', {}):
            check(key in theme_fields, prefix + 'unknown theme field ' + key)
        if children:
            check(spec.get('child_adapter') not in ('', 'none', None), prefix + 'control cannot host children')
        if node.get('type') == 'UiTitleCard':
            check(len(children) <= 1, prefix + 'TitleCard accepts one hosted control')
        for child in children:
            required = {'UiTab': 'UiTabPage', 'UiAccordion': 'UiAccordionSection'}.get(node.get('type'))
            if required:
                check(by_id.get(child, {}).get('type') == required, prefix + 'expected ' + required)
    return errors


if __name__ == '__main__':
    try:
        source = Path(sys.argv[1])
        problems = validate(json.loads(source.read_text(encoding='utf-8-sig')),
                            Path(__file__).resolve().parents[1] / 'references' / 'controls')
        for problem in problems:
            print(problem)
        print(f"Portable preflight: {'FAIL' if problems else 'PASS'}; canonical/visual checks are separate")
        sys.exit(bool(problems))
    except (IndexError, ValueError, OSError, TypeError) as exc:
        print(f'Invalid input: {exc}')
        sys.exit(1)
