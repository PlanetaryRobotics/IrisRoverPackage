# Crude tool to grab the memory allocation data from a linkInfo XML file
# (TI's cg_xml is very deprecated and seems unable to do this).
#
# Takes 1 argument: path to `_linkInfo.xml` file to open.

import os
import sys
from lxml import etree
from rich import print as rich_print
from rich.progress import Progress, BarColumn

def ColoredProgress(color: str, on_color: str="bright_black", bar_width=40) -> Progress:
    return Progress(
        "[progress.description]{task.description}",
        BarColumn(bar_width=bar_width, complete_style=color, style=on_color),
        "[progress.percentage]{task.percentage:>3.0f}%"
    )

def mem_table_row(progress, data):
    return progress.add_task(
        f"[{data['color']}] {data['name']:<13} "
        f"""{f"(@0x{data['origin']:x}):":>12}"""
        f""" {f"{data['used_space']:,}":>10}B   /"""
        f""" {f"{data['length']:,}":>10}B"""
        f"""    ({f"{data['unused_space']:,}":>10}B free)""",
        completed=data['used_space'],
        total=data['length']
    )

if __name__ == "__main__":
    file_path: str = sys.argv[1]
    try:
        xml = etree.parse(file_path)
    except OSError:
        print(f"Build file {file_path} does not exist. Maybe build failed?")
        exit()
    # Grab all displayed memory areas:
    memory_areas = xml.xpath('//memory_area[@display="yes"]')
    # Extract key data:
    memory_area_data = [{
        'name': area.findtext('name'),
        'color': area.attrib.get('color', 'blue'),
        'page_id': int(area.findtext('page_id'), 16),
        'origin': int(area.findtext('origin'), 16),
        'length': int(area.findtext('length'), 16),
        'used_space': int(area.findtext('used_space'), 16),
        'unused_space': int(area.findtext('unused_space'), 16),
        'area': area
    } for i, area in enumerate(memory_areas)]

    # Display as nice table with little progress bars:
    rich_print(f"\n[b][u]{os.path.basename(file_path)} Memory Details:[/u][/b]")
    for data in memory_area_data:
        with ColoredProgress(data['color']) as progress:
            mem_table_row(progress, data)
        # Add details of each logical group (lg), sorted by size:
        logical_groups = []
        for space in data['area'].xpath('.//allocated_space'):
            for lgr in space.xpath('.//logical_group_ref'):
                if 'idref' not in lgr.attrib:
                    continue
                lg_id = lgr.attrib['idref']
                lgs = xml.xpath(f'//logical_group[@id="{lg_id}"]')
                for lg in lgs:
                    name = lg.findtext('name')
                    color = lg.attrib.get('color', 'cyan')
                    size = int(lg.findtext('size'), 16)
                    logical_groups.append(dict(
                        name=name,
                        color=color,
                        size=size
                    ))
        logical_groups = sorted(logical_groups, key=lambda x: x['size'], reverse=True)
        for i, lg in enumerate(logical_groups):
            name, color, size = lg['name'], lg['color'], lg['size']
            color = ('dim ' if i%2 else '') + color
            with ColoredProgress(color, bar_width=81) as progress:
                progress.add_task(
                    f"[{color}]    {name:<13} ({f'{size:,}':>10}B)",
                    completed=size,
                    total=data['length']
                )
    
    rich_print(f"\n\n[b][u]{os.path.basename(file_path)} Memory Summary:[/u][/b]")
    for i, data in enumerate(memory_area_data):
        data['color'] = ('dim ' if i%2 else '') + data['color']
        with ColoredProgress(data['color']) as progress:
            mem_table_row(progress, data)
