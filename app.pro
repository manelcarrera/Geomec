TEMPLATE = subdirs

# addSubdirs(subdirs,deps): Adds directories to the project that depend on
# other directories

defineTest( addSubdirs ) {
	for(subdirs, 1) {
		entries = $$files($$subdirs)
		for(entry, entries) {
			name = $$replace(entry, [/\\\\], _)
			name = $$replace(name, - , _)
			SUBDIRS += $$name
			eval ($${name}.subdir = $$entry)
			for(dep, 2): {
				tempval = $$replace(dep, [/\\\\], _)
				eval ($${name}.depends += $$replace(tempval, - , _) )
			}
			export ($${name}.subdir)
			export ($${name}.depends)
		}
	}
	export (SUBDIRS)
}

# the sub-directories

# libraries

addSubdirs (lib/lib01)

addSubdirs (app/app01, lib/lib01 )
