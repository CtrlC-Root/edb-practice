#!/usr/bin/env python


def options(ctx):
    """
    Define configuration options.
    """

    ctx.load('compiler_cxx')


def configure(ctx):
    """
    Configure the project.
    """

    # linux: -std=c++20
    # macos: -std=c++2a

    ctx.load('compiler_cxx')
    ctx.env.append_value('CXXFLAGS', ['-std=c++2a', '-Wall', '-Werror'])


def build(ctx):
    """
    Build the project.
    """

    ctx.recurse('pkg/libdictdb')
    ctx.recurse('pkg/dictdb')
    ctx.recurse('pkg/dict')
