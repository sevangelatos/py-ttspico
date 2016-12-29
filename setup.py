#!/usr/bin/env python
from setuptools import setup, find_packages
import os

from distutils.core import Extension

version = '0.2'

extLevensthein = Extension('ctts',
                           sources = ['cttsmodule.c', '../tts/tts_engine.c'],
                           libraries=['svoxpico'],
                           )

setup(name='python-tts',
      version=version,
      description="Python simple text to speech module.",
      long_description=open("README.rst").read(),
      # Get more strings from http://pypi.python.org/pypi?%3Aaction=list_classifiers
      classifiers=[
        "Programming Language :: Python",
        ],
      include_dirs = ['../tts/'],
      keywords='speech text-to-speech svox',
      author='Spiros Evangelatos',
      author_email='sevangelatos@gmail.com',
      url='http://github.com/sevangelatos/picopy',
      license='GPL',
      packages=find_packages(exclude=['ez_setup']),
      namespace_packages=[],
      include_package_data=True,
      zip_safe=False,
      ext_modules = [extLevensthein],
      py_modules=['tts'],
      install_requires=[
          'setuptools',
          # -*- Extra requirements: -*-
      ],
      entry_points="""
      # -*- Entry points: -*-
      """,
      )
