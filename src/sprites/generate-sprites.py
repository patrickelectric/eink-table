#!/usr/bin/env /usr/bin.python3

import requests
import sys
import os

from PIL import Image
from io import BytesIO

def to_rgb332(r, g, b):
    return ((r & 0xE0) | ((g >> 3) & 0x1C) | (b >> 6))

def download_sprite(url):
    response = requests.get(url)
    if response.status_code == 200:
        return Image.open(BytesIO(response.content))
    return None

def save_lvgl_c_file(name, img, output_dir):
    img = img.convert('RGB')
    pixels = list(img.getdata())
    width, height = img.size
    rgb332_bytes = [to_rgb332(r, g, b) for r, g, b in pixels]

    c_path = os.path.join(output_dir, f"{name}.c")
    with open(c_path, 'w') as f:
        f.write(
            '#pragma once\n\n'
            '#ifdef __has_include\n'
            '#if __has_include("lvgl.h")\n'
            '#ifndef LV_LVGL_H_INCLUDE_SIMPLE\n'
            '#define LV_LVGL_H_INCLUDE_SIMPLE\n'
            '#endif\n'
            '#endif\n'
            '#endif\n\n'
            '#if defined(LV_LVGL_H_INCLUDE_SIMPLE)\n'
            '#include "lvgl.h"\n'
            '#else\n'
            '#include "lvgl/lvgl.h"\n'
            '#endif\n\n'
            '#ifndef LV_ATTRIBUTE_MEM_ALIGN\n'
            '#define LV_ATTRIBUTE_MEM_ALIGN\n'
            '#endif\n\n'
            f'#ifndef LV_ATTRIBUTE_IMG_{name.upper()}\n'
            f'#define LV_ATTRIBUTE_IMG_{name.upper()}\n'
            '#endif\n\n'
        )

        f.write(f'const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_{name.upper()} uint8_t {name}_map[] = {{\n')
        for i in range(0, len(rgb332_bytes), 12):
            line = rgb332_bytes[i:i+12]
            f.write('  ' + ', '.join(f'0x{b:02x}' for b in line) + ',\n')
        f.write('};\n\n')

        data_size = len(rgb332_bytes)
        f.write(
            f'const lv_img_dsc_t {name} = {{\n'
            '  .header.cf = LV_IMG_CF_TRUE_COLOR,\n'
            '  .header.always_zero = 0,\n'
            '  .header.reserved = 0,\n'
            f'  .header.w = {width},\n'
            f'  .header.h = {height},\n'
            f'  .data_size = {data_size},\n'
            f'  .data = {name}_map,\n'
            '};\n'
        )

def generate_all_h(output_dir):
    c_files = [f for f in os.listdir(output_dir) if f.endswith('.c') and f != 'all.c']
    with open(os.path.join(output_dir, 'all.h'), 'w') as f:
        f.write(
            '#ifdef __has_include\n'
            '#if __has_include("lvgl.h")\n'
            '#ifndef LV_LVGL_H_INCLUDE_SIMPLE\n'
            '#define LV_LVGL_H_INCLUDE_SIMPLE\n'
            '#endif\n'
            '#endif\n'
            '#endif\n\n'
            '#if defined(LV_LVGL_H_INCLUDE_SIMPLE)\n'
            '#include "lvgl.h"\n'
            '#else\n'
            '#include "lvgl/lvgl.h"\n'
            '#endif\n\n'
        )

        for c_file in c_files:
            sprite_name = c_file[:-2]
            f.write(f'LV_IMG_DECLARE({sprite_name});\n')

        f.write(
            'const lv_img_dsc_t** all_sprites() {\n'
            '   static const lv_img_dsc_t* sprites[] = {\n'
        )
        for c_file in c_files:
            sprite_name = c_file[:-2]
            f.write(f'      &{sprite_name},\n')
        f.write(
            '   };\n'
            '   return sprites;\n'
            '}\n'
        )

def main(output_dir):
    if os.path.exists(os.path.join(output_dir, 'all.h')):
        print("all.h already exists, skipping generation")
        return

    os.makedirs(output_dir, exist_ok=True)
    url = "https://pokeapi.co/api/v2/pokemon?limit=151"
    response = requests.get(url)
    data = response.json()

    for pkm in data['results']:
        name = pkm['name']
        pkm_data = requests.get(pkm['url']).json()
        sprite_url = pkm_data['sprites']['versions']['generation-i']['red-blue']['front_gray']
        if sprite_url:
            name = name.replace('-', '_')
            output_file = os.path.join(output_dir, f"{name}.c")
            if os.path.exists(output_file):
                print(f"Skipping {name}.c - file already exists")
                continue

            img = download_sprite(sprite_url)
            if img:
                save_lvgl_c_file(name, img, output_dir)
                print(f"Saved {name}.c")

    generate_all_h(output_dir)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py output_directory")
        sys.exit(1)
    output_dir = sys.argv[1]
    main(output_dir)
