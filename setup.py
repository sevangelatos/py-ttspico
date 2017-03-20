#!/usr/bin/env python
from setuptools import setup, find_packages
import os
from distutils.core import Extension
from os.path import join

version = '0.1'

lang_dir = join('picopi', 'pico', 'lang')
lang_files = [os.path.join(lang_dir, f) for f in 
    os.listdir(lang_dir) if f.endswith(".bin")]

pico_src_dir = join('picopi', 'pico', 'lib')
source_files = [os.path.join(pico_src_dir, f) for f in 
    os.listdir(pico_src_dir) if f.endswith(".c")]

source_files += ['cttsmodule.c', 
'picopi/pico/tts/tts_engine.c', 
'picopi/pico/tts/langfiles.c']

ext_ctts = Extension('ctts', sources = source_files)

setup(name='ttspico',
      version=version,
      description="Python simple text to speech module.",
      long_description=open("README.rst").read(),
      # Get more strings from http://pypi.python.org/pypi?%3Aaction=list_classifiers
      classifiers=[
        "Programming Language :: Python",
		"Development Status :: 3 - Alpha",
		"Topic :: Multimedia :: Sound/Audio :: Speech",
		"Environment :: Console",
		"Intended Audience :: Developers",
		"Operating System :: POSIX",
		"License :: OSI Approved :: Apache Software License"
        ],
      include_dirs = ['picopi/pico/tts/', pico_src_dir],
      keywords='speech text-to-speech svox',
      author='Spiros Evangelatos',
      author_email='sevangelatos@gmail.com',
      url='http://github.com/sevangelatos/py-ttspico',
	  # download_url = 'https://github.com/sevangelatos/py-ttspico/archive/0.3.tar.gz',
      license='Apache License 2.0',
      packages=find_packages(exclude=['ez_setup']),
      namespace_packages=[],
      include_package_data=True,
      zip_safe=False,
      ext_modules = [ext_ctts],
      py_modules=['ttspico'],
      install_requires=[
          'setuptools',
          # -*- Extra requirements: -*-
      ],
      entry_points="""
      # -*- Entry points: -*-
      """,
      data_files=[('languages', lang_files)]
      )
