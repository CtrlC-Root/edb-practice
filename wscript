#!/usr/bin/env waf

def configure(ctx):
    """Configure the project."""

    ctx.find_program('gcc', var='CPP')
    ctx.env.append_value('CPPFLAGS', ['-std=c++20', '-Wall', '-Werror'])
    ctx.env.append_value('LFLAGS', ['-lstdc++'])


def build(ctx):
    """Build the project."""

    ctx.recurse('pkg/dictdb')
    ctx.recurse('pkg/dict')
