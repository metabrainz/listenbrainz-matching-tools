import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="lb-matching-tools",
    author="Robert Kaye",
    author_email="rob@metabrainz.org",
    version="0.1.0",
    description="ListenBrainz tools for matching metadata to and from MusicBrainz.",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/metabrainz/listenbrainz-matching-tools",
    packages=setuptools.find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: GNU General Public License v2 (GPLv2)",
        "Operating System :: OS Independent",
    ],
    python_requires='>=3.7',
    install_requires=[ ],
    use_scm_version=True,
    setup_requires=['setuptools_scm'],
)
