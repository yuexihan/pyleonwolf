try:
	from setuptools import setup, Extension
except ImportError:
	from distutils.core import setup, Extension

module = Extension(
	'leonwolf',
	sources=[
		'pyleonwolf.c'
	],
	depends=[
		'common.h',
		'LPSTree.h',
		'LPSTree.c'
	]
)

setup(
	name='pyleonwolf',
	version='1.0',
	ext_modules=[module],

	description=(
		'leonwolf module'
	),
	author='Leon Wolf',
	author_email='463691529@qq.com'
)