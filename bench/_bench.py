import os
import pandas as pd
import pathlib
import subprocess


SEARCH_PATHS: list[str] = [
    '../cmake-build-release',
    '../cmake-build-debug',
    '../build',
]

METRICS: list[str] = [
    'real_time',
    'cpu_time',
    'bytes_per_second',
    'items_per_second',
    'iterations',
]


def get_lib_name(filename) -> str:
    return pathlib.Path(filename).stem


def find_bench_executable(filename) -> str | None:
    for search_path in SEARCH_PATHS:
        path = search_path + '/' + get_lib_name(filename) + '_bench'
        if os.path.isfile(path):
            return path
        path = search_path + '/' + get_lib_name(filename) + '_bench.exe'
        if os.path.isfile(path):
            return path


def create_data(exe: str, filename: str) -> str:
    output = str(pathlib.Path(exe).parent / (get_lib_name(filename) + '_bench.csv'))
    with open(output, 'w') as out_file:
        subprocess.run([exe, '--benchmark_format=csv'], stdout=out_file)
    return output


def read_data(exe: str, filename: str) -> pd.DataFrame:
    try:
        data = pd.read_csv(create_data(exe, filename))
    except ValueError:
        exit(1)
    return data
