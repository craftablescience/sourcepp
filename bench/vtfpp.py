#!/usr/bin/env python3

import matplotlib.pyplot as plt
import pandas as pd
import pathlib
from _bench import get_lib_name, find_bench_executable, read_data


def create_plot_conversions_to_rgba8888(exe: str, data: pd.DataFrame) -> None:
    df = data[data['name'].apply(lambda name: 'convert' in name and 'to' in name and 'DXT' not in name and 'BC' not in name and 'ATI' not in name)]

    df['library'] = df['name'].apply(lambda x: 'vtfpp' if 'vtfpp' in x else 'vtflib')
    df['benchmark'] = df['name'].apply(lambda x: x.split('_')[-3])
    df['real_time'] = df['real_time'].apply(lambda x: x / 1000000)

    df_vtfpp = df[df['library'] == 'vtfpp'].set_index('benchmark')
    df_vtflib = df[df['library'] == 'vtflib'].set_index('benchmark')
    df_merged = df_vtfpp[['real_time']].join(df_vtflib[['real_time']], lsuffix='_vtfpp', rsuffix='_vtflib')

    df_merged.plot(kind='bar', figsize=(10, 6))
    plt.title('VTFPP - Conversions to RGBA8888 (256x256)')
    plt.xlabel('Format')
    plt.ylabel('Time (ms)')
    plt.xticks(rotation=45, ha='right')
    plt.legend(['vtfpp', 'vtflib'])
    plt.tight_layout()

    plt.savefig(str(pathlib.Path(exe).parent / (get_lib_name(__file__) + '_bench_conversions_256x256_to_rgba8888.png')))


def create_plot_creation(exe: str, data: pd.DataFrame) -> None:
    df = data[data['name'].apply(lambda name: 'create' in name)]

    df['library'] = df['name'].apply(lambda x: 'vtfpp' if 'vtfpp' in x else 'vtflib')
    df['benchmark'] = df['name'].apply(lambda x: x.split('_')[-1].capitalize())
    df['real_time'] = df['real_time'].apply(lambda x: x / 1000000)

    df_vtfpp = df[df['library'] == 'vtfpp'].set_index('benchmark')
    df_vtflib = df[df['library'] == 'vtflib'].set_index('benchmark')
    df_merged = df_vtfpp[['real_time']].join(df_vtflib[['real_time']], lsuffix='_vtfpp', rsuffix='_vtflib')

    df_merged.plot(kind='bar', figsize=(10, 6))
    plt.title('VTFPP - Creation (2048x2048 RGBA8888)')
    plt.xlabel('Type')
    plt.ylabel('Time (ms)')
    plt.xticks(rotation=45, ha='right')
    plt.legend(['vtfpp', 'vtflib'])
    plt.tight_layout()

    plt.savefig(str(pathlib.Path(exe).parent / (get_lib_name(__file__) + '_bench_creation_2048x2048_rgba8888.png')))


def create_plot_compute_reflectivity(exe: str, data: pd.DataFrame) -> None:
    df = data[data['name'].apply(lambda name: 'compute_reflectivity' in name)]

    df['library'] = df['name'].apply(lambda x: 'vtfpp' if 'vtfpp' in x else 'vtflib')
    df['benchmark'] = df['name'].apply(lambda x: x.split('_')[-1] + ' Frame' + ('s' if int(x.split('_')[-1]) > 1 else ''))
    df['real_time'] = df['real_time'].apply(lambda x: x / 1000000)

    df_vtfpp = df[df['library'] == 'vtfpp'].set_index('benchmark')
    df_vtflib = df[df['library'] == 'vtflib'].set_index('benchmark')
    df_merged = df_vtfpp[['real_time']].join(df_vtflib[['real_time']], lsuffix='_vtfpp', rsuffix='_vtflib')

    df_merged.plot(kind='bar', figsize=(10, 6))
    plt.title('VTFPP - Compute Reflectivity (2048x2048 RGBA8888)')
    plt.xlabel('Type')
    plt.ylabel('Time (ms)')
    plt.xticks(rotation=45, ha='right')
    plt.legend(['vtfpp', 'vtflib'])
    plt.tight_layout()

    plt.savefig(str(pathlib.Path(exe).parent / (get_lib_name(__file__) + '_bench_compute_reflectivity_2048x2048_rgba8888.png')))


def main() -> None:
    exe = find_bench_executable(__file__)
    if not exe:
        exit(1)
    data = read_data(exe, __file__)

    create_plot_conversions_to_rgba8888(exe, data)
    create_plot_creation(exe, data)
    create_plot_compute_reflectivity(exe, data)


if __name__ == '__main__':
    main()
