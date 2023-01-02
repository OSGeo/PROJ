from sphinx.domains.std import StandardDomain

class MyStandardDomain(StandardDomain):

    def resolve_xref(self, env, fromdocname, builder, typ, target, node, contnode):
        if typ == 'program':
            typ = 'ref'
        return StandardDomain.resolve_xref(self, env, fromdocname, builder, typ, target, node, contnode)

def setup(app):
    # Override the "program" role to be an alias of "ref"
    MyStandardDomain.roles["program"] = StandardDomain.roles["ref"]
    app.add_domain(MyStandardDomain, override=True)
    return { 'parallel_read_safe': True, 'parallel_write_safe': True }
