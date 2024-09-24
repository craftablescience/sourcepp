#!/usr/bin/env python3

import matplotlib.pyplot as plt
import pandas as pd
import pathlib
from _bench import get_lib_name, find_bench_executable, read_data


def create_plot(exe: str, data: pd.DataFrame) -> None:
    df = data

    df['library'] = df['name'].apply(lambda x: 'kvpp' if 'kvpp' in x else 'speedykeyv')
    df['benchmark'] = df['name'].apply(lambda x: ' '.join([c.capitalize() for c in x.split('_', 2)[-1].split('_')]))
    df['real_time'] = df['real_time'].apply(lambda x: x / 1000000)

    df_kvpp = df[df['library'] == 'kvpp'].set_index('benchmark')
    df_speedykeyv = df[df['library'] == 'speedykeyv'].set_index('benchmark')
    df_merged = df_kvpp[['real_time']].join(df_speedykeyv[['real_time']], lsuffix='_kvpp', rsuffix='_speedykeyv')

    df_merged.plot(kind='bar', figsize=(10, 6))
    plt.title('KVPP')
    plt.xlabel('Data Size')
    plt.ylabel('Time (ms)')
    plt.xticks(rotation=45, ha='right')
    plt.legend(['kvpp', 'speedykeyv'])
    plt.tight_layout()

    plt.savefig(str(pathlib.Path(exe).parent / (get_lib_name(__file__) + '_bench.png')))


def main() -> None:
    exe = find_bench_executable(__file__)
    data = read_data(exe, __file__)

    create_plot(exe, data)


if __name__ == '__main__':
    main()
