from setuptools import setup, find_packages

setup(
    name="Video-Inpainting",
    version="0.1.1",
    packages=find_packages(),

    package_data={
        '': ['*.txt']
    },
    author="Yaugen Zhuk",
    author_email="yaugen.zhuk@gmail.com",
    description="Video Inpainting Algorithm",
    license="MIT",

)
