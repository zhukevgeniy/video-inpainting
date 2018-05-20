from setuptools import setup, find_packages

from os import path

here = path.abspath(path.dirname(__file__))

# Get the long description from the README file
with open(path.join(here, 'README.md'), encoding='utf-8') as f:
    long_description = f.read()

setup(
    name="Video-Inpainting",
    version="0.1.1",
    packages=find_packages(exclude=['tests']),
    package_data={
        '': ['*.txt']
    },
    author="Yaugen Zhuk",
    author_email="yaugen.zhuk@gmail.com",
    description="Video Inpainting Algorithm",
    long_description=long_description,
    license="MIT",

)
